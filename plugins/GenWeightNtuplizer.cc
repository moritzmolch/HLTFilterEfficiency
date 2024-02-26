// system include files
#include <memory>
#include <cmath>
#include <regex>


// user include files

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"

#include "SimDataFormats/GeneratorProducts/interface/GenEventInfoProduct.h"

#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/HighLevelTriggerPath.h"

#include <TTree.h>

using namespace edm;
using namespace pat;
using namespace std;


class GenWeightNtuplizer: public one::EDAnalyzer<one::SharedResources> {

    public:
        explicit GenWeightNtuplizer(const ParameterSet&);
        ~GenWeightNtuplizer() {}
        static void fillDescriptions(ConfigurationDescriptions& descriptions);

    private:
        virtual void beginJob() override;
        virtual void endJob() override;
        virtual void analyze(const Event&, const EventSetup&) override;

        EDGetTokenT<GenEventInfoProduct> genEvtInfo_;

        TTree* genWeightTree_;
        long int lumi_;
        long int run_;
        long int event_;
        float genWeight_;

        Service<TFileService> fs_;
};


GenWeightNtuplizer::GenWeightNtuplizer(const ParameterSet& iConfig) {
    genEvtInfo_ = consumes<GenEventInfoProduct>(iConfig.getParameter<InputTag>("generator"));

    lumi_ = -1;
    run_ = -1;
    event_ = -1;
    genWeight_ = 0.;

    usesResource();
}


void GenWeightNtuplizer::fillDescriptions(ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}


void GenWeightNtuplizer::beginJob() {
    genWeightTree_ = fs_->make<TTree>("genWeights", "genWeights");
    genWeightTree_->Branch("lumi", &lumi_, "lumi/L");
    genWeightTree_->Branch("run", &run_,  "run/L");
    genWeightTree_->Branch("event", &event_, "event/L");
    genWeightTree_->Branch("genWeight", &genWeight_, "genWeight/F");
}


void GenWeightNtuplizer::analyze(const Event& event, const EventSetup& setup) {
    Handle<GenEventInfoProduct> genEvtInfo;

    event.getByToken(genEvtInfo_, genEvtInfo);

    lumi_ = event.luminosityBlock();
    run_ = event.id().run();
    event_ = event.id().event();
    genWeight_ = static_cast<float>(genEvtInfo->weight());

    genWeightTree_->Fill();
}


void GenWeightNtuplizer::endJob() {}


//define this as a plug-in
DEFINE_FWK_MODULE(GenWeightNtuplizer);
