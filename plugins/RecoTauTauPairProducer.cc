// system include files

#include <cmath>
#include <memory>
#include <string>
#include <vector>

// user include files

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/EDPutToken.h"

#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/tautau_selection_reco.h"

using namespace edm;
using namespace std;
using namespace pat;
using namespace tautau_selection_reco;


class RecoTauTauPairProducer : public one::EDProducer<one::SharedResources> {

public:
    explicit RecoTauTauPairProducer(const ParameterSet&);
    ~RecoTauTauPairProducer();
    
private:
    void produce(Event&, const EventSetup&) override;
    void beginJob() override;
    void endJob() override;

    EDGetTokenT<vector<Electron>> electrons_;
    EDGetTokenT<vector<Muon>> muons_;
    EDGetTokenT<vector<Tau>> taus_;
    EDGetTokenT<vector<Electron>> vetoElectrons_;
    EDGetTokenT<vector<Muon>> vetoMuons_;

    EDPutTokenT<vector<Electron>> pairElectrons_;
    EDPutTokenT<vector<Muon>> pairMuons_;
    EDPutTokenT<vector<Tau>> pairTaus_;
};


void RecoTauTauPairProducer::beginJob() {};


void RecoTauTauPairProducer::endJob() {};


RecoTauTauPairProducer::RecoTauTauPairProducer(const ParameterSet& iConfig) {
    electrons_ = consumes<vector<Electron>>(iConfig.getParameter<InputTag>("electrons"));
    muons_ = consumes<vector<Muon>>(iConfig.getParameter<InputTag>("muons"));
    taus_ = consumes<vector<Tau>>(iConfig.getParameter<InputTag>("taus"));
    vetoElectrons_ = consumes<vector<Electron>>(iConfig.getParameter<InputTag>("vetoElectrons"));
    vetoMuons_ = consumes<vector<Muon>>(iConfig.getParameter<InputTag>("vetoMuons"));

    pairElectrons_ = produces<vector<Electron>>("pairElectrons");
    pairMuons_ = produces<vector<Muon>>("pairMuons");
    pairTaus_ = produces<vector<Tau>>("pairTaus");
}


RecoTauTauPairProducer::~RecoTauTauPairProducer() {}


void RecoTauTauPairProducer::produce(Event& event, const EventSetup& setup) {
    Handle<vector<Electron>> electrons;
    Handle<vector<Muon>> muons;
    Handle<vector<Tau>> taus;
    Handle<vector<Electron>> vetoElectrons;
    Handle<vector<Muon>> vetoMuons;

    event.getByToken(electrons_, electrons);
    event.getByToken(muons_, muons);
    event.getByToken(taus_, taus);
    event.getByToken(vetoElectrons_, vetoElectrons);
    event.getByToken(vetoMuons_, vetoMuons);

    unique_ptr<vector<Electron>> pairElectrons = make_unique<vector<Electron>>();
    unique_ptr<vector<Muon>> pairMuons = make_unique<vector<Muon>>();
    unique_ptr<vector<Tau>> pairTaus = make_unique<vector<Tau>>();

    TauTauPairAlgorithm tauTauPairAlgo = TauTauPairAlgorithm(*electrons, *muons, *taus, *vetoElectrons, *vetoMuons);
    tauTauPairAlgo.execute();

    TauTauFinalState recoFinalState = tauTauPairAlgo.getFinalState();
    if (recoFinalState == TauTauFinalState::et) {
        const pair<Electron, Tau> etPair = tauTauPairAlgo.getPairET();
        pairElectrons->push_back(etPair.first);
        pairTaus->push_back(etPair.second);
    } else if (recoFinalState == TauTauFinalState::mt) {
        const pair<Muon, Tau> mtPair = tauTauPairAlgo.getPairMT();
        pairMuons->push_back(mtPair.first);
        pairTaus->push_back(mtPair.second);
    } else if (recoFinalState == TauTauFinalState::tt) {
        const pair<Tau, Tau> ttPair = tauTauPairAlgo.getPairTT();
        pairTaus->push_back(ttPair.first);
        pairTaus->push_back(ttPair.second);
    }

    event.put(pairElectrons_, move(pairElectrons));
    event.put(pairMuons_, move(pairMuons));
    event.put(pairTaus_, move(pairTaus));
}


//define this as a plug-in
DEFINE_FWK_MODULE(RecoTauTauPairProducer);
