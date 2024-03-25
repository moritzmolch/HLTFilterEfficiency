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

#include "TauAnalysis/TauTriggerNtuples/interface/tautau_selection_gen.h"

using namespace edm;
using namespace std;
using namespace reco;
using namespace tautau_selection_gen;


class TauTauGenParticlesFilter : public one::EDFilter<one::SharedResources> {

public:
    explicit TauTauGenParticlesFilter(const ParameterSet&);
    ~TauTauGenParticlesFilter();

private:
    void beginJob() override;
    void endJob() override;
    bool filter(Event&, const EventSetup&) override;

    EDGetTokenT<vector<GenParticle>> tauTauGenParticles_;
};


void TauTauGenParticlesFilter::beginJob() {};


void TauTauGenParticlesFilter::endJob() {};


TauTauGenParticlesFilter::TauTauGenParticlesFilter(const ParameterSet& iConfig) {
    tauTauGenParticles_ = consumes<vector<GenParticle>>(iConfig.getParameter<InputTag>("tauTauGenParticles"));
}


TauTauGenParticlesFilter::~TauTauGenParticlesFilter() {}


bool TauTauGenParticlesFilter::filter(Event& event, const EventSetup& setup) {
    Handle<vector<GenParticle>> tauTauGenParticles;

    event.getByToken(tauTauGenParticles_, tauTauGenParticles);

    // reject ill-defined events
    if (tauTauGenParticles->size() < 2) {
        return false;
    }

    // get the first two particles, which are the leptons from the Z/gamma* decay
    const GenParticle& lepton1 = tauTauGenParticles->at(0);
    const GenParticle& lepton2 = tauTauGenParticles->at(1);

    // get the final state
    HZGammaFinalState genFinalState = getDileptonFinalState(lepton1, lepton2);

    if (genFinalState == HZGammaFinalState::unknown) {
        LogWarning("TauTauGenParticlesFilter") << "failed to find generator-level lepton pair";
    }

    // only keep events in the Z/gamma* -> tau tau final states, in which the tau tau pair decay is fullhadronic and semileptonic
    if ((genFinalState == HZGammaFinalState::et) || (genFinalState == HZGammaFinalState::mt) || (genFinalState == HZGammaFinalState::tt)) {
        return true;
    }

    return false;
}


//define this as a plug-in
DEFINE_FWK_MODULE(TauTauGenParticlesFilter);
