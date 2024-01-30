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


class TauTriggerNtuplizer: public one::EDAnalyzer<one::WatchRuns, one::SharedResources> {

    public:
        explicit TauTriggerNtuplizer(const ParameterSet&);
        ~TauTriggerNtuplizer() {}
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
        EDGetTokenT<vector<TriggerObjectStandAlone>> triggerObjects_;
        EDGetTokenT<vector<Electron>> pairElectrons_;
        EDGetTokenT<vector<Muon>> pairMuons_;
        EDGetTokenT<vector<Tau>> pairTaus_;
        EDGetTokenT<vector<reco::GenParticle>> slimmedGenParticles_;

        EDGetTokenT<GenEventInfoProduct> genEvtInfo_;
        vector<string> hltPathList_;
        bool isMC_;
        bool isEmb_;
        string triggerResultsProcess_;

        Service<TFileService> fs_;

        vector<shared_ptr<HighLevelTriggerPath>> hltPaths_;

        TTree* eventsTree_;
        long int lumiEv_;
        long int runEv_;
        long int eventEv_;
        bool isElTau_;
        bool isMuTau_;
        bool isTauTau_;
        float genWeight_;
        vector<float> genParticlePt_;
        vector<float> genParticleEta_;
        vector<float> genParticlePhi_;
        vector<float> genParticleMass_;
        vector<int> genParticleCharge_;
        vector<int> genParticlePdgId_;
        vector<float> pairElectronPt_;
        vector<float> pairElectronEta_;
        vector<float> pairElectronPhi_;
        vector<float> pairElectronMass_;
        vector<int> pairElectronCharge_;
        vector<int> pairElectronPdgId_;
        vector<float> pairMuonPt_;
        vector<float> pairMuonEta_;
        vector<float> pairMuonPhi_;
        vector<float> pairMuonMass_;
        vector<int> pairMuonCharge_;
        vector<int> pairMuonPdgId_;
        vector<float> pairTauPt_;
        vector<float> pairTauEta_;
        vector<float> pairTauPhi_;
        vector<float> pairTauMass_;
        vector<int> pairTauCharge_;
        vector<int> pairTauPdgId_;
        vector<float> triggerObjectPt_;
        vector<float> triggerObjectEta_;
        vector<float> triggerObjectPhi_;
        vector<float> triggerObjectMass_;
        vector<int> triggerObjectCharge_;
        vector<int> triggerObjectPdgId_;
        vector<int> triggerObjectType_;
        vector<int> triggerObjectHLTPathIndex_;
        vector<int> triggerObjectModuleIndex_;
        vector<int> eventHltPathIndex_;
        vector<int> eventHltPathLastModule_;
        vector<int> eventHltPathLastModuleState_;

        TTree* hltTree_;
        long int runTr_;
        string hltGlobalTag_;
        string hltTableName_;
        string hltPathName_;
        string hltPathVersion_;
        int hltPathIndex_;
        vector<string> hltPathModules_;
        vector<string> hltPathModulesSaveTags_;
};


TauTriggerNtuplizer::TauTriggerNtuplizer(const ParameterSet& iConfig) {
    triggerResults_ = consumes<TriggerResults>(iConfig.getParameter<InputTag>("triggerResults"));
    triggerObjects_ = consumes<vector<TriggerObjectStandAlone>>(iConfig.getParameter<InputTag>("triggerObjects"));
    pairElectrons_ = consumes<vector<Electron>>(iConfig.getParameter<InputTag>("pairElectrons"));
    pairMuons_ = consumes<vector<Muon>>(iConfig.getParameter<InputTag>("pairMuons"));
    pairTaus_ = consumes<vector<Tau>>(iConfig.getParameter<InputTag>("pairTaus"));
    slimmedGenParticles_ = mayConsume<vector<reco::GenParticle>>(iConfig.getParameter<InputTag>("slimmedGenParticles"));

    hltPathList_ = iConfig.getUntrackedParameter<vector<string>>("hltPathList", hltPathList_);
    triggerResultsProcess_ = iConfig.getParameter<InputTag>("triggerResults").process();
    isMC_ = iConfig.getUntrackedParameter<bool>("isMC", isMC_);
    isEmb_ = iConfig.getUntrackedParameter<bool>("isEmb", isEmb_);

    if (isMC_ || isEmb_) {
        genEvtInfo_ = consumes<GenEventInfoProduct>(iConfig.getParameter<InputTag>("generator"));
    }

    hltPaths_ = vector<shared_ptr<HighLevelTriggerPath>>();

    lumiEv_ = -1;
    runEv_ = -1;
    eventEv_ = -1;
    isElTau_ = false;
    isMuTau_ = false;
    isTauTau_ = false;
    genWeight_ = 1.;
    genParticlePt_ = vector<float>();
    genParticleEta_ = vector<float>();
    genParticlePhi_ = vector<float>();
    genParticleMass_ = vector<float>();
    genParticleCharge_ = vector<int>();
    genParticlePdgId_ = vector<int>();
    pairElectronPt_ = vector<float>();
    pairElectronEta_ = vector<float>();
    pairElectronPhi_ = vector<float>();
    pairElectronMass_ = vector<float>();
    pairElectronCharge_ = vector<int>();
    pairElectronPdgId_ = vector<int>();
    pairMuonPt_ = vector<float>();
    pairMuonEta_ = vector<float>();
    pairMuonPhi_ = vector<float>();
    pairMuonMass_ = vector<float>();
    pairMuonCharge_ = vector<int>();
    pairMuonPdgId_ = vector<int>();
    pairTauPt_ = vector<float>();
    pairTauEta_ = vector<float>();
    pairTauPhi_ = vector<float>();
    pairTauMass_ = vector<float>();
    pairTauCharge_ = vector<int>();
    pairTauPdgId_ = vector<int>();
    triggerObjectPt_ = vector<float>();
    triggerObjectEta_ = vector<float>();
    triggerObjectPhi_ = vector<float>();
    triggerObjectMass_ = vector<float>();
    triggerObjectCharge_ = vector<int>();
    triggerObjectPdgId_ = vector<int>();
    triggerObjectType_ = vector<int>();
    triggerObjectHLTPathIndex_ = vector<int>();
    triggerObjectModuleIndex_ = vector<int>();
    eventHltPathIndex_ = vector<int>();
    eventHltPathLastModule_ = vector<int>();
    eventHltPathLastModuleState_ = vector<int>();

    runTr_ = -1;
    hltGlobalTag_ = "";
    hltTableName_ = "";
    hltPathName_ = "";
    hltPathVersion_ = "";
    hltPathIndex_ = -1;
    hltPathModules_ = vector<string>();
    hltPathModulesSaveTags_ = vector<string>();

    usesResource();
}

void TauTriggerNtuplizer::fillDescriptions(ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}

void TauTriggerNtuplizer::beginJob() {
    eventsTree_ = fs_->make<TTree>("Events", "Events");
    eventsTree_->Branch("lumi", &lumiEv_, "lumi/L");
    eventsTree_->Branch("run", &runEv_,  "run/L");
    eventsTree_->Branch("event", &eventEv_, "event/L");
    eventsTree_->Branch("isElTau", &isElTau_, "isElTau/O");
    eventsTree_->Branch("isMuTau", &isMuTau_, "isMuTau/O");
    eventsTree_->Branch("isTauTau", &isTauTau_, "isTauTau/O");
    eventsTree_->Branch("genWeight", &genWeight_, "genWeight/F");
    eventsTree_->Branch("genParticlePt", &genParticlePt_);
    eventsTree_->Branch("genParticleEta", &genParticleEta_);
    eventsTree_->Branch("genParticlePhi", &genParticlePhi_);
    eventsTree_->Branch("genParticleMass", &genParticleMass_);
    eventsTree_->Branch("genParticleCharge", &genParticleCharge_);
    eventsTree_->Branch("genParticlePdgId", &genParticlePdgId_);
    eventsTree_->Branch("pairElectronPt", &pairElectronPt_);
    eventsTree_->Branch("pairElectronEta", &pairElectronEta_);
    eventsTree_->Branch("pairElectronPhi", &pairElectronPhi_);
    eventsTree_->Branch("pairElectronMass", &pairElectronMass_);
    eventsTree_->Branch("pairElectronCharge", &pairElectronCharge_);
    eventsTree_->Branch("pairElectronPdgId", &pairElectronPdgId_);
    eventsTree_->Branch("pairMuonPt", &pairMuonPt_);
    eventsTree_->Branch("pairMuonEta", &pairMuonEta_);
    eventsTree_->Branch("pairMuonPhi", &pairMuonPhi_);
    eventsTree_->Branch("pairMuonMass", &pairMuonMass_);
    eventsTree_->Branch("pairMuonCharge", &pairMuonCharge_);
    eventsTree_->Branch("pairMuonPdgId", &pairMuonPdgId_);
    eventsTree_->Branch("pairTauPt", &pairTauPt_);
    eventsTree_->Branch("pairTauEta", &pairTauEta_);
    eventsTree_->Branch("pairTauPhi", &pairTauPhi_);
    eventsTree_->Branch("pairTauMass", &pairTauMass_);
    eventsTree_->Branch("pairTauCharge", &pairTauCharge_);
    eventsTree_->Branch("pairTauPdgId", &pairTauPdgId_);
    eventsTree_->Branch("triggerObjectPt", &triggerObjectPt_);
    eventsTree_->Branch("triggerObjectEta", &triggerObjectEta_);
    eventsTree_->Branch("triggerObjectPhi", &triggerObjectPhi_);
    eventsTree_->Branch("triggerObjectMass", &triggerObjectMass_);
    eventsTree_->Branch("triggerObjectCharge", &triggerObjectCharge_);
    eventsTree_->Branch("triggerObjectPdgId", &triggerObjectPdgId_);
    eventsTree_->Branch("triggerObjectType", &triggerObjectType_);
    eventsTree_->Branch("triggerObjectHLTPathIndex", &triggerObjectHLTPathIndex_);
    eventsTree_->Branch("triggerObjectModuleIndex", &triggerObjectModuleIndex_);
    eventsTree_->Branch("hltPathIndex", &eventHltPathIndex_);
    eventsTree_->Branch("hltPathLastModule", &eventHltPathLastModule_);
    eventsTree_->Branch("hltPathLastModuleState", &eventHltPathLastModuleState_);

    hltTree_ = fs_->make<TTree>("HLT", "HLT");
    hltTree_->Branch("run", &runTr_, "run/L");
    hltTree_->Branch("hltTableName", &hltTableName_);
    hltTree_->Branch("hltGlobalTag", &hltGlobalTag_);
    hltTree_->Branch("hltPathName", &hltPathName_);
    hltTree_->Branch("hltPathVersion", &hltPathVersion_);
    hltTree_->Branch("hltPathIndex", &hltPathIndex_);
    hltTree_->Branch("hltPathModules", &hltPathModules_);
    hltTree_->Branch("hltPathModulesSaveTags", &hltPathModulesSaveTags_);
}

void TauTriggerNtuplizer::beginRun(const Run& run, const EventSetup& setup) {

    bool changed = true;

    // initialize the config for this run
    hltConfig_.init(run, setup, triggerResultsProcess_, changed);

    if (changed) {
        // empty the old filter list, paths of different runs might have different version numbers
        hltPaths_.clear();

        // get the table name and the global tag
        hltTableName_ = hltConfig_.tableName();
        hltGlobalTag_ = hltConfig_.globalTag();
    
        runTr_ = run.id().run();

        // loop through all trigger paths and process paths that have been selected in the python config file
        vector<string> triggerNames = hltConfig_.triggerNames();
        for (size_t i = 0; i < triggerNames.size(); ++i) {
            string fullName = triggerNames.at(i);
            if (! isSelectedHLTPath(fullName)) {
                continue;
            }
            shared_ptr<HighLevelTriggerPath> hltPath = make_shared<HighLevelTriggerPath>(fullName, i, hltConfig_.moduleLabels(fullName),  hltConfig_.saveTagsModules(fullName));
            hltPaths_.push_back(hltPath);

            hltPathModules_.clear();
            hltPathModulesSaveTags_.clear();

            hltPathName_ = hltPath->fullName();
            hltPathVersion_ = "v";
            hltPathIndex_ = hltPath->index();
            hltPathModulesSaveTags_.clear();
            for (const string& module : hltPath->modules()) {
                hltPathModules_.push_back(module);
            }
            for (const string& module : hltPath->modulesSaveTags()) {
                hltPathModulesSaveTags_.push_back(module);
            }
            hltTree_->Fill();
        }
    }
}

void TauTriggerNtuplizer::analyze(const Event& event, const EventSetup& setup) {
    Handle<TriggerResults> triggerResults;
    Handle<vector<TriggerObjectStandAlone>> triggerObjects;
    Handle<vector<Electron>> pairElectrons;
    Handle<vector<Muon>> pairMuons;
    Handle<vector<Tau>> pairTaus;

    event.getByToken(triggerResults_, triggerResults);
    event.getByToken(triggerObjects_, triggerObjects);
    event.getByToken(pairElectrons_, pairElectrons);
    event.getByToken(pairMuons_, pairMuons);
    event.getByToken(pairTaus_, pairTaus);

    lumiEv_ = event.luminosityBlock();
    runEv_ = event.id().run();
    eventEv_ = event.id().event();

    isElTau_ = false;
    isMuTau_ = false;
    isTauTau_ = false;

    if ((pairElectrons->size() == 1) && (pairMuons->size() == 0) && (pairTaus->size() == 1)) {
        isElTau_ = true;
    } else if ((pairElectrons->size() == 0) && (pairMuons->size() == 1) && (pairTaus->size() == 1)) {
        isMuTau_ = true;
    } else if ((pairElectrons->size() == 0) && (pairMuons->size() == 0) && (pairTaus->size() == 2)) {
        isTauTau_ = true;
    }

    genWeight_ = 1.;
    if (isMC_ || isEmb_) {
        Handle<GenEventInfoProduct> genEvtInfo;
        event.getByToken(genEvtInfo_, genEvtInfo);
        genWeight_ = static_cast<float>(genEvtInfo->weight());
    }

    genParticlePt_.clear();
    genParticleEta_.clear();
    genParticlePhi_.clear();
    genParticleMass_.clear();
    genParticleCharge_.clear();
    genParticlePdgId_.clear();

    if (isMC_) {
        Handle<vector<reco::GenParticle>> slimmedGenParticles;
        event.getByToken(slimmedGenParticles_, slimmedGenParticles);
        for (const reco::GenParticle& genParticle : *slimmedGenParticles) {
            genParticlePt_.push_back(genParticle.pt());
            genParticleEta_.push_back(genParticle.eta());
            genParticlePhi_.push_back(genParticle.phi());
            genParticleMass_.push_back(genParticle.mass());
            genParticleCharge_.push_back(genParticle.charge());
            genParticlePdgId_.push_back(genParticle.pdgId());
        }
    }

    pairElectronPt_.clear();
    pairElectronEta_.clear();
    pairElectronPhi_.clear();
    pairElectronMass_.clear();
    pairElectronCharge_.clear();
    pairElectronPdgId_.clear();

    for (const Electron& electron : *pairElectrons) {
        pairElectronPt_.push_back(electron.pt());
        pairElectronEta_.push_back(electron.eta());
        pairElectronPhi_.push_back(electron.phi());
        pairElectronMass_.push_back(electron.mass());
        pairElectronCharge_.push_back(electron.charge());
        pairElectronPdgId_.push_back(electron.pdgId());
    }

    pairMuonPt_.clear();
    pairMuonEta_.clear();
    pairMuonPhi_.clear();
    pairMuonMass_.clear();
    pairMuonCharge_.clear();
    pairMuonPdgId_.clear();

    for (const Muon& muon : *pairMuons) {
        pairMuonPt_.push_back(muon.pt());
        pairMuonEta_.push_back(muon.eta());
        pairMuonPhi_.push_back(muon.phi());
        pairMuonMass_.push_back(muon.mass());
        pairMuonCharge_.push_back(muon.charge());
        pairMuonPdgId_.push_back(muon.pdgId());
    }

    pairTauPt_.clear();
    pairTauEta_.clear();
    pairTauPhi_.clear();
    pairTauMass_.clear();
    pairTauCharge_.clear();
    pairTauPdgId_.clear();

    for (const Tau& tau : *pairTaus) {
        pairTauPt_.push_back(tau.pt());
        pairTauEta_.push_back(tau.eta());
        pairTauPhi_.push_back(tau.phi());
        pairTauMass_.push_back(tau.mass());
        pairTauCharge_.push_back(tau.charge());
        pairTauPdgId_.push_back(tau.pdgId());
    }

    eventHltPathIndex_.clear();
    eventHltPathLastModule_.clear();
    eventHltPathLastModuleState_.clear();

    for (const shared_ptr<HighLevelTriggerPath> hltPath : hltPaths_) {
        int hltPathIndex = hltConfig_.triggerIndex(hltPath->fullName());
        eventHltPathIndex_.push_back(hltPathIndex);
        eventHltPathLastModule_.push_back((*triggerResults).index(hltPathIndex));
        eventHltPathLastModuleState_.push_back((*triggerResults).state(hltPathIndex));
    }

    triggerObjectPt_.clear();
    triggerObjectEta_.clear();
    triggerObjectPhi_.clear();
    triggerObjectMass_.clear();
    triggerObjectCharge_.clear();
    triggerObjectPdgId_.clear();
    triggerObjectType_.clear();
    triggerObjectHLTPathIndex_.clear();
    triggerObjectModuleIndex_.clear();

    for (const TriggerObjectStandAlone& trigObj : *triggerObjects) {

        for (const string& trigObjPathName : trigObj.pathNames(false, false)) {

            for (const shared_ptr<HighLevelTriggerPath> hltPath : hltPaths_) {
                if (hltPath->fullName() != trigObjPathName) {
                    continue;
                }
                const int hltPathIndex = hltPath->index();

                for (const string& trigObjModule : trigObj.filterLabels()) {
                    if (!hltPath->isInModulesSaveTags(trigObjModule)) {
                        continue;
                    }
                    const int trigObjModuleIndex = hltPath->moduleIndex(trigObjModule);

                    for (const int& trigObjType : trigObj.triggerObjectTypes()) {
                        triggerObjectPt_.push_back(trigObj.pt());
                        triggerObjectEta_.push_back(trigObj.eta());
                        triggerObjectPhi_.push_back(trigObj.phi());
                        triggerObjectMass_.push_back(trigObj.mass());
                        triggerObjectCharge_.push_back(trigObj.charge());
                        triggerObjectPdgId_.push_back(trigObj.pdgId());
                        triggerObjectHLTPathIndex_.push_back(hltPathIndex);
                        triggerObjectModuleIndex_.push_back(trigObjModuleIndex);
                        triggerObjectType_.push_back(trigObjType);
                    }
                }
            }
        }
    }

    eventsTree_->Fill();
}


void TauTriggerNtuplizer::endJob() {}


const bool TauTriggerNtuplizer::isSelectedHLTPath(const string& path) {
    for (const string& name : hltPathList_) {
        // the selected HLT path name can differ from the actual name by a version suffix
        // make a regex matching in order to identify whether we have a selected HLT path here
        char buffer[1024];
        snprintf(buffer, 1024, "^%s_v\\d+$", name.c_str());
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

void TauTriggerNtuplizer::endRun(const Run& run, const EventSetup& setup) {}


//define this as a plug-in
DEFINE_FWK_MODULE(TauTriggerNtuplizer);
