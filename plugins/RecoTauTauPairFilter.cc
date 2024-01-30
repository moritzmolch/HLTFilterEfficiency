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
#include "FWCore/Framework/interface/one/EDFilter.h"
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


class RecoTauTauPairFilter : public one::EDFilter<one::SharedResources> {

public:
    explicit RecoTauTauPairFilter(const ParameterSet&);
    ~RecoTauTauPairFilter();
    
private:
    bool filter(Event&, const EventSetup&) override;
    void beginJob() override;
    void endJob() override;

    EDGetTokenT<vector<Electron>> pairElectrons_;
    EDGetTokenT<vector<Muon>> pairMuons_;
    EDGetTokenT<vector<Tau>> pairTaus_;
};


void RecoTauTauPairFilter::beginJob() {};


void RecoTauTauPairFilter::endJob() {};


RecoTauTauPairFilter::RecoTauTauPairFilter(const ParameterSet& iConfig) {
    pairElectrons_ = consumes<vector<Electron>>(iConfig.getParameter<InputTag>("pairElectrons"));
    pairMuons_ = consumes<vector<Muon>>(iConfig.getParameter<InputTag>("pairMuons"));
    pairTaus_ = consumes<vector<Tau>>(iConfig.getParameter<InputTag>("pairTaus"));
}


RecoTauTauPairFilter::~RecoTauTauPairFilter() {}


bool RecoTauTauPairFilter::filter(Event& event, const EventSetup& setup) {
    Handle<vector<Electron>> pairElectrons;
    Handle<vector<Muon>> pairMuons;
    Handle<vector<Tau>> pairTaus;

    event.getByToken(pairElectrons_, pairElectrons);
    event.getByToken(pairMuons_, pairMuons);
    event.getByToken(pairTaus_, pairTaus);

    if (
        ((pairElectrons->size() == 1) && (pairMuons->size() == 0) && (pairTaus->size() == 1))
        || ((pairElectrons->size() == 0) && (pairMuons->size() == 1) && (pairTaus->size() == 1))
        || ((pairElectrons->size() == 0) && (pairMuons->size() == 0) && (pairTaus->size() == 2))
    ) {
        return true;
    }

    return false;
}


//define this as a plug-in
DEFINE_FWK_MODULE(RecoTauTauPairFilter);
