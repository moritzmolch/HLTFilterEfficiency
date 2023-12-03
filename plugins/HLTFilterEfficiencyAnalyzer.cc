// system include files
#include <memory>
#include <cmath>
#include <regex>


// user include files

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "FWCore/Common/interface/TriggerNames.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "HLTrigger/HLTcore/interface/HLTConfigProvider.h"


class HLTHistogramCollection {

public:
    HLTHistogramCollection();
    HLTHistogramCollection(edm::Service<TFileService>&);

    TH1D& get(const std::string&, const unsigned int&, const bool&);
    TH1D& create(const std::string&, const unsigned int&, const bool&, const std::vector<std::string>&);
    const bool contains(const std::string&) const;
    const bool contains(const std::string&, const unsigned int&, const bool&) const;
    void fill(const std::string&, const unsigned int&, const std::string&, const edm::hlt::HLTState&);

private:
    const std::string makeKey(const std::string& hltPath, const unsigned int& run, const bool& onlySaveTags) const;

    edm::Service<TFileService> fs_;
    std::map<std::string, TH1D*> histograms_;
};


HLTHistogramCollection::HLTHistogramCollection() {}


HLTHistogramCollection::HLTHistogramCollection(edm::Service<TFileService>& fs) {
    fs_ = fs;
}


TH1D& HLTHistogramCollection::create(
    const std::string& hltPath,
    const unsigned int& run,
    const bool& onlySaveTags,
    const std::vector<std::string>& filterNames
) {
    std::string key = makeKey(hltPath, run, onlySaveTags);

    if (contains(key)) {
        throw std::invalid_argument("histogram key already exists");
    }

    const char* name = key.c_str();
    const char* title = hltPath.c_str();

    int nBins = filterNames.size() + 1;
    TH1D* h = fs_->make<TH1D>(name, title, nBins, 0, nBins);
    h->GetXaxis()->SetBinLabel(1, "nEvents");
    for (int i = 2; i <= h->GetNbinsX(); ++i) {
        h->GetXaxis()->SetBinLabel(i, filterNames.at(i - 2).c_str());
    }

    histograms_[key] = h;

    return *h;
}


const bool HLTHistogramCollection::contains(const std::string& hltPath, const unsigned int& run, const bool& onlySaveTags) const {
    std::string key = makeKey(hltPath, run, onlySaveTags);
    for (const auto& item : histograms_) {
        if (item.first == key) {
            return true;
        }
    }
    return false;
}


const bool HLTHistogramCollection::contains(const std::string& key) const {
    for (const auto& item : histograms_) {
        if (item.first == key) {
            return true;
        }
    }
    return false;
}


TH1D& HLTHistogramCollection::get(const std::string& hltPath, const unsigned int& run, const bool& onlySaveTags) {
    std::string key = makeKey(hltPath, run, onlySaveTags);
    for (auto& item : histograms_) {
        if (item.first == key) {
            return *item.second;
        }
    }

    throw std::out_of_range("histogram key not found");
}


void HLTHistogramCollection::fill(const std::string& hltPath, const unsigned int& run, const std::string& lastFilter, const edm::hlt::HLTState& hltState) {
    TH1D& hFull = get(hltPath, run, false);
    TH1D& hOnlySaveTags = get(hltPath, run, true);

    for (int i = 1; i <= hFull.GetNbinsX(); ++i) {
        std::string binLabel = std::string(hFull.GetXaxis()->GetBinLabel(i));

        bool breakAtEnd = false;
        if (binLabel == lastFilter) {
            if (hltState == edm::hlt::Pass) {
                breakAtEnd = true;
            } else {
                break;
            }
        }

        hFull.AddBinContent(i);

        for (int j = 1; j <= hOnlySaveTags.GetNbinsX(); ++j) {
            std::string binLabelOnlySaveTags = std::string(hOnlySaveTags.GetXaxis()->GetBinLabel(j));
            if (binLabelOnlySaveTags == binLabel) {
                hOnlySaveTags.AddBinContent(j);
            }
        }

        if (breakAtEnd) {
            break;
        }
    }

}


const std::string HLTHistogramCollection::makeKey(const std::string& hltPath, const unsigned int& run, const bool& onlySaveTags) const {
    char buffer[1024];
    snprintf(buffer, 1024, "%s__%u__%u", hltPath.c_str(), run, static_cast<unsigned int>(onlySaveTags));
    return std::string(buffer);
}


class HLTEventAnalyzer: public edm::one::EDAnalyzer<edm::one::WatchRuns, edm::one::SharedResources> {

    public:
        explicit HLTEventAnalyzer(const edm::ParameterSet&);
        ~HLTEventAnalyzer() {}
        static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);

    private:
        virtual void beginJob() override;
        virtual void endJob() override;
        virtual void beginRun(const edm::Run&, const edm::EventSetup&) override;
        virtual void endRun(const edm::Run&, const edm::EventSetup&) override;
        virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
        bool isSelectedHLTPath(const std::string& path);

        HLTConfigProvider hltConfig_;

        edm::EDGetTokenT<edm::TriggerResults> triggerResults_;
        edm::EDGetTokenT<pat::PackedTriggerPrescales> triggerPrescales_;
        std::vector<std::string> hltPathsSelected_;

        edm::Service<TFileService> fs_;

        unsigned int runIndex_;
        std::vector<std::string> hltPaths_;
        HLTHistogramCollection hltHistograms_;
};


HLTEventAnalyzer::HLTEventAnalyzer(const edm::ParameterSet& iConfig) {
    triggerResults_ = consumes<edm::TriggerResults>(iConfig.getParameter<edm::InputTag>("results"));
    triggerPrescales_ = consumes<pat::PackedTriggerPrescales>(iConfig.getParameter<edm::InputTag>("prescales"));
    hltPathsSelected_ = iConfig.getUntrackedParameter<std::vector<std::string>>("hlt_paths", hltPathsSelected_);

    runIndex_ = 0;
    hltPaths_ = std::vector<std::string>();
    hltHistograms_ = HLTHistogramCollection(fs_);
}


void HLTEventAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    edm::ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}


void HLTEventAnalyzer::beginJob() {}


void HLTEventAnalyzer::endJob() {}


void HLTEventAnalyzer::beginRun(const edm::Run& run, const edm::EventSetup& setup) {

    // set the run index
    runIndex_ = run.runAuxiliary().run();

    std::string process = "SIMembeddingHLT";
    bool changed = true;

    // initialize the config for this run
    hltConfig_.init(run, setup, process, changed);

    // empty the old filter list, paths of different runs might have different version numbers
    hltPaths_.clear();

    // get the table name and the global tag
    std::string tableName = hltConfig_.tableName();
    std::string globalTag = hltConfig_.globalTag();

    // loop through all trigger paths and process paths that have been selected in the python config file
    std::vector<std::string> hltPathsConfig = hltConfig_.triggerNames();
    for (const std::string& hltPath : hltPathsConfig) {

        if (! isSelectedHLTPath(hltPath)) {
            continue;
        }

        hltPaths_.push_back(hltPath);

        if (! hltHistograms_.contains(hltPath, runIndex_, false)) {
            std::vector<std::string> filterNames = hltConfig_.moduleLabels(hltPath);
            hltHistograms_.create(hltPath, runIndex_, false, filterNames);
        }

        if (! hltHistograms_.contains(hltPath, runIndex_, true)) {
            std::vector<std::string> filterNames = hltConfig_.saveTagsModules(hltPath);
            hltHistograms_.create(hltPath, runIndex_, true, filterNames);
        }

    }

}


void HLTEventAnalyzer::endRun(const edm::Run& run, const edm::EventSetup& setup) {}


void HLTEventAnalyzer::analyze(const edm::Event& event, const edm::EventSetup& setup) {
    /**
    if (sample_ == "sim") {
        edm::Handle<reco::GenParticleCollection> genParticles;
        iEvent.getByToken(genParticles_, genParticles);

        bool isZTauTau = false;
        for (size_t i = 0; i < genParticles->size(); ++i) {
            const reco::GenParticle& p = (*genParticles)[i];
            if (abs(p.pdgId()) != 23) {
                continue;
            }

            size_t nDaughter = (size_t) p.numberOfDaughters();
            int nTaus = 0;
            for (size_t j = 0; j < nDaughter; ++j) {
                const reco::Candidate* d = p.daughter(j);
                if (abs(d->pdgId()) == 15) {
                    nTaus += 1;
                }
            }

            if (nTaus == 2) {
                isZTauTau = true;
            }

            break;
        }

        if (! isZTauTau) return ;
    }
    */

    edm::Handle<edm::TriggerResults> triggerResults;
    edm::Handle<pat::PackedTriggerPrescales> triggerPrescales;

    event.getByToken(triggerResults_, triggerResults);
    event.getByToken(triggerPrescales_, triggerPrescales);


    for (const std::string& hltPath : hltPaths_) {
        unsigned int hltPathIndex = hltConfig_.triggerIndex(hltPath);
        std::string lastFilter = hltConfig_.moduleLabel(hltPathIndex, (*triggerResults).index(hltPathIndex));
        edm::hlt::HLTState hltState = (*triggerResults).state(hltPathIndex);
        //hltPath.fill(filterLastRunIndex, hltPathState);

        hltHistograms_.fill(hltPath, runIndex_, lastFilter, hltState);
    }
}


bool HLTEventAnalyzer::isSelectedHLTPath(const std::string& path) {
    for (const std::string& selPath : hltPathsSelected_) {
        char buffer[512];
        snprintf(buffer, 512, "^%s_v\\d+$", selPath.c_str());
        std::string selPathPattern(buffer);
        std::regex selPathRegex(selPathPattern);
        std::smatch match;
        if (std::regex_match(path, match, selPathRegex)) {
            return true;
        }
    }
    return false;
}


//define this as a plug-in
DEFINE_FWK_MODULE(HLTEventAnalyzer);
