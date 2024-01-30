// system include files

#include <cmath>
#include <memory>
#include <string>
#include <vector>

// user include files

#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"

#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/tautau_selection_gen.h"

using namespace edm;
using namespace std;
using namespace reco;
using namespace tautau_selection_gen;


class GenHZGammaToLLFilter : public one::EDFilter<one::SharedResources> {

public:
    explicit GenHZGammaToLLFilter(const ParameterSet&);
    ~GenHZGammaToLLFilter();
    
private:
    void beginJob() override;
    void endJob() override;
    bool filter(Event&, const EventSetup&) override;

    EDGetTokenT<vector<GenParticle>> slimmedGenParticles_;
};


void GenHZGammaToLLFilter::beginJob() {};


void GenHZGammaToLLFilter::endJob() {};


GenHZGammaToLLFilter::GenHZGammaToLLFilter(const ParameterSet& iConfig) {
    slimmedGenParticles_ = consumes<vector<GenParticle>>(iConfig.getParameter<InputTag>("slimmedGenParticles"));
}


GenHZGammaToLLFilter::~GenHZGammaToLLFilter() {}


bool GenHZGammaToLLFilter::filter(Event& event, const EventSetup& setup) {
    Handle<vector<GenParticle>> slimmedGenParticles;

    event.getByToken(slimmedGenParticles_, slimmedGenParticles);

    // reject ill-defined events
    if (slimmedGenParticles->size() != 3) {
        return false;
    }

    const GenParticle& genBoson = slimmedGenParticles->at(0);
    const GenParticle& genLep1 = slimmedGenParticles->at(1);
    const GenParticle& genLep2 = slimmedGenParticles->at(2);

    // get the final state
    HZGammaFinalState genFinalState = getHZGammaFinalState(genBoson);

    if (genFinalState == HZGammaFinalState::unknown) {
        LogWarning("GenHZGammaToLLFilter") << "failed to find generator-level lepton pair";
    }

    if ((genFinalState != HZGammaFinalState::et) && (genFinalState != HZGammaFinalState::mt) && (genFinalState != HZGammaFinalState::tt)) {
        return false;
    }

    if (max(genLep1.p4().pt(), genLep2.p4().pt()) <= 17) {
        return false;
    }
 
    const Candidate::LorentzVector& p4Dilep = genLep1.p4() + genLep2.p4();
    if (p4Dilep.mass() <= 20) {
        return false;
    }

    return true;
}


//define this as a plug-in
DEFINE_FWK_MODULE(GenHZGammaToLLFilter);
