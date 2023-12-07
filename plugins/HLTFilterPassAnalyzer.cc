// system include files
#include <memory>
#include <cmath>
#include <regex>


// user include files

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/HLTFilterPassHistogram.h"
#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/HLTHistogramStore.h"


using namespace edm;
using namespace pat;
using namespace std;
using namespace hltTools;


class HLTFilterPassAnalyzer: public one::EDAnalyzer<one::WatchRuns, one::SharedResources> {

    public:
        explicit HLTFilterPassAnalyzer(const ParameterSet&);
        ~HLTFilterPassAnalyzer() {}
        static void fillDescriptions(ConfigurationDescriptions& descriptions);

    private:
        virtual void beginJob() override;
        virtual void endJob() override;
        virtual void beginRun(const Run&, const EventSetup&) override;
        virtual void endRun(const Run&, const EventSetup&) override;
        virtual void analyze(const Event&, const EventSetup&) override;
        const bool isSelectedHLTPath(const string& path);

        HLTConfigProvider hltConfig_;

        EDGetTokenT<TriggerResults> triggerResults_;
        EDGetTokenT<PackedTriggerPrescales> triggerPrescales_;
        vector<string> hltPathsSelected_;
        string triggerResultsProcess_;

        Service<TFileService> fs_;

        vector<string> runHLTPaths_;
        HLTHistogramStore histStore_;
};


HLTFilterPassAnalyzer::HLTFilterPassAnalyzer(const ParameterSet& iConfig) {
    triggerResults_ = consumes<TriggerResults>(iConfig.getParameter<InputTag>("results"));
    triggerPrescales_ = consumes<PackedTriggerPrescales>(iConfig.getParameter<InputTag>("prescales"));
    hltPathsSelected_ = iConfig.getUntrackedParameter<vector<string>>("hlt_paths", hltPathsSelected_);
    triggerResultsProcess_ = iConfig.getParameter<InputTag>("results").process();

    runHLTPaths_ = vector<string>();
    histStore_ = HLTHistogramStore();

    usesResource();
}


void HLTFilterPassAnalyzer::fillDescriptions(ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}


void HLTFilterPassAnalyzer::beginRun(const Run& run, const EventSetup& setup) {

    bool changed = true;

    // initialize the config for this run
    hltConfig_.init(run, setup, triggerResultsProcess_, changed);

    // empty the old filter list, paths of different runs might have different version numbers
    runHLTPaths_.clear();

    // get the table name and the global tag
    string tableName = hltConfig_.tableName();
    string globalTag = hltConfig_.globalTag();

    // loop through all trigger paths and process paths that have been selected in the python config file
    vector<string> hltPathsConfig = hltConfig_.triggerNames();
    for (const string& hltPath : hltPathsConfig) {

        if (! isSelectedHLTPath(hltPath)) {
            continue;
        }

        // add the HLT path to the list of HLT paths processed during this run
        runHLTPaths_.push_back(hltPath);

        // create the HLT filter pass histogram
        if ( ! histStore_.contains(hltPath) ) {
            shared_ptr<HLTFilterPassHistogram> hist = make_shared<HLTFilterPassHistogram>(
                hltPath,
                hltConfig_.moduleLabels(hltPath)
            );
            hist->SetFilterNamesSaveTags(hltConfig_.saveTagsModules(hltPath));
            histStore_.put(hltPath, hist);
        }

    }

}


void HLTFilterPassAnalyzer::analyze(const Event& event, const EventSetup& setup) {

    // get the trigger results for this event
    Handle<TriggerResults> triggerResults;
    event.getByToken(triggerResults_, triggerResults);

    // analyzer each HLT path selected in the config:
    // - read out the name of the last running filter
    // - read out the status of the last running filter (Ready, Exception, Fail, Pass)
    // - fill the filter pass histogram for this event
    for (const string& hltPath : runHLTPaths_) {
        unsigned int hltPathIndex = hltConfig_.triggerIndex(hltPath);
        string lastFilter = hltConfig_.moduleLabel(hltPathIndex, (*triggerResults).index(hltPathIndex));
        hlt::HLTState hltState = (*triggerResults).state(hltPathIndex);

        shared_ptr<HLTFilterPassHistogram> hist = histStore_.get(hltPath);
        hist->fill(lastFilter, hltState == hlt::Pass);
    }
}


void HLTFilterPassAnalyzer::endJob() {

    for (HLTHistogramStore::StoreIterator it = histStore_.begin(); it != histStore_.end(); ++it) {
        shared_ptr<HLTFilterPassHistogram> h = *it;
        fs_->make<TH1D>(h->getTH1D());
        fs_->make<TH1D>(h->getTH1DSaveTags());
    }
}


const bool HLTFilterPassAnalyzer::isSelectedHLTPath(const string& path) {
    for (const string& selPath : hltPathsSelected_) {
        // the selected HLT path name can differ from the actual name by a version suffix
        // make a regex matching in order to identify whether we have a selected HLT path here
        char buffer[1024];
        snprintf(buffer, 1024, "^%s_v\\d+$", selPath.c_str());
        string selPathPattern(buffer);
        regex selPathRegex(selPathPattern);
        smatch match;
        if (regex_match(path, match, selPathRegex)) {
            return true;
        }
    }

    // if no match has been found, this HLT path is not in the list of selected HLT paths
    return false;
}


//
// dummy implementations of EDAnalyzer methods that are not used
//


void HLTFilterPassAnalyzer::beginJob() {}


void HLTFilterPassAnalyzer::endRun(const Run& run, const EventSetup& setup) {}


//define this as a plug-in
DEFINE_FWK_MODULE(HLTFilterPassAnalyzer);
