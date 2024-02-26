// system include files

#include <cmath>
#include <memory>
#include <string>
#include <vector>

// user include files

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDProducer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/EDGetToken.h"
#include "FWCore/Utilities/interface/EDPutToken.h"

#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/tautau_selection_gen.h"

using namespace edm;
using namespace std;
using namespace reco;
using namespace tautau_selection_gen;


class TauTauGenParticlesProducer : public one::EDProducer<one::SharedResources> {

public:
    explicit TauTauGenParticlesProducer(const ParameterSet&);
    ~TauTauGenParticlesProducer();
    
private:
    void produce(Event&, const EventSetup&) override;
    void beginJob() override;
    void endJob() override;

    EDGetTokenT<vector<GenParticle>> genParticles_;

    EDPutTokenT<vector<GenParticle>> tauTauGenParticles_;
};


void TauTauGenParticlesProducer::beginJob() {};


void TauTauGenParticlesProducer::endJob() {};


TauTauGenParticlesProducer::TauTauGenParticlesProducer(const ParameterSet& iConfig) {
    genParticles_ = consumes<vector<GenParticle>>(iConfig.getParameter<InputTag>("genParticles"));
    tauTauGenParticles_ = produces<vector<GenParticle>>("tauTauGenParticles");
}


TauTauGenParticlesProducer::~TauTauGenParticlesProducer() {}


void TauTauGenParticlesProducer::produce(Event& event, const EventSetup& setup) {
    Handle<GenParticleCollection> genParticles;
    event.getByToken(genParticles_, genParticles);

    unique_ptr<vector<GenParticle>> tauTauGenParticles = make_unique<vector<GenParticle>>();

    GenParticle lepton1;
    GenParticle lepton2;
    bool foundEE = false;
    bool foundMM = false;
    bool foundTT = false;

    foundEE = setPairLeptons(*genParticles, 11, lepton1, lepton2); 
    if (!foundEE) {
        foundMM = setPairLeptons(*genParticles, 13, lepton1, lepton2); 
    }
    if (!foundEE && !foundMM) {
        foundTT = setPairLeptons(*genParticles, 15, lepton1, lepton2); 
    }
    if (!foundEE && !foundMM && !foundTT) {
        LogWarning("TauTauGenParticlesProducer") << "failed to find generator-level dilepton pair";
        event.put(tauTauGenParticles_, move(tauTauGenParticles));
        return;
    } 

    tauTauGenParticles->push_back(lepton1);
    tauTauGenParticles->push_back(lepton2);
    for (const GenParticle& genParticle : getDirectDaughters(lepton1)) {
        tauTauGenParticles->push_back(genParticle);
    }
    for (const GenParticle& genParticle : getDirectDaughters(lepton2)) {
        tauTauGenParticles->push_back(genParticle);
    }
    event.put(tauTauGenParticles_, move(tauTauGenParticles));
}


//define this as a plug-in
DEFINE_FWK_MODULE(TauTauGenParticlesProducer);
