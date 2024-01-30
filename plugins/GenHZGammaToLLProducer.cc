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


class GenHZGammaToLLProducer : public one::EDProducer<one::SharedResources> {

public:
    explicit GenHZGammaToLLProducer(const ParameterSet&);
    ~GenHZGammaToLLProducer();
    
private:
    void produce(Event&, const EventSetup&) override;
    void beginJob() override;
    void endJob() override;

    EDGetTokenT<vector<GenParticle>> genParticles_;

    EDPutTokenT<vector<GenParticle>> slimmedGenParticles_;
};


void GenHZGammaToLLProducer::beginJob() {};


void GenHZGammaToLLProducer::endJob() {};


GenHZGammaToLLProducer::GenHZGammaToLLProducer(const ParameterSet& iConfig) {
    genParticles_ = consumes<vector<GenParticle>>(iConfig.getParameter<InputTag>("genParticles"));

    slimmedGenParticles_ = produces<vector<GenParticle>>("slimmedGenParticles");
}


GenHZGammaToLLProducer::~GenHZGammaToLLProducer() {}


void GenHZGammaToLLProducer::produce(Event& event, const EventSetup& setup) {
    Handle<GenParticleCollection> genParticles;
    event.getByToken(genParticles_, genParticles);

    GenParticle& genBoson = *(new GenParticle);
    HZGammaFinalState genFinalState;
    unique_ptr<vector<GenParticle>> slimmedGenParticles = make_unique<vector<GenParticle>>();

    // find the generator-level boson and put it into the event
    try {
        genBoson = getHZGammaBoson(*genParticles);
    } catch (runtime_error& e) {
        //for (const GenParticle& genParticle : *genParticles) {
        //    cout << "PDG ID " << genParticle.pdgId() << endl;
        //    for (unsigned int i = 0; i < genParticle.numberOfMothers(); ++i) {
        //        cout << "  Mother " << i + 1 << ":" << genParticle.mother(i)->pdgId() << endl;
        //    }
        //}
        LogWarning("GenHZGammaToLLProducer") << "failed to find generator-level boson: " << e.what();
        event.put(slimmedGenParticles_, move(slimmedGenParticles));
        return;
    } catch (exception& e) {
        throw;
    }
    slimmedGenParticles->push_back(genBoson);

    // get the final state and put it into the event
    genFinalState = getHZGammaFinalState(genBoson);

    // get the pdgID of the lepton pair
    int pdgId = 0;
    if (genFinalState == HZGammaFinalState::eePrompt) {
        pdgId = 11;
    } else if (genFinalState == HZGammaFinalState::mmPrompt) {
        pdgId = 13;
    } else if (
        (genFinalState == HZGammaFinalState::et)
        || (genFinalState == HZGammaFinalState::mt)
        || (genFinalState == HZGammaFinalState::tt)
        || (genFinalState == HZGammaFinalState::ee)
        || (genFinalState == HZGammaFinalState::mm)
        || (genFinalState == HZGammaFinalState::em)
    ) {
        pdgId = 15;
    } else {
        event.put(slimmedGenParticles_, move(slimmedGenParticles));
        return;
    }

    // put the H/Z/Gamma -> ll decay products into the event
    const pair<GenParticle, GenParticle> lepPair = getHZGammaLeptonPair(genBoson, pdgId);
    slimmedGenParticles->push_back(lepPair.first);
    slimmedGenParticles->push_back(lepPair.second);
    event.put(slimmedGenParticles_, move(slimmedGenParticles));
}

//define this as a plug-in
DEFINE_FWK_MODULE(GenHZGammaToLLProducer);
