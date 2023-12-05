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


// namespaces

using namespace edm;
using namespace std;
using namespace reco;


class DrellYanGenTauTauFilter : public one::EDFilter<one::SharedResources> {

    public:
        explicit DrellYanGenTauTauFilter(const ParameterSet&);
        ~DrellYanGenTauTauFilter() {}

        enum class TauDecayMode {
            El = 11,
            Mu = 13,
            Tau = 15,
            Unknown = 0,
        };

    private:
        bool filter(edm::Event&, const edm::EventSetup &);
        DrellYanGenTauTauFilter::TauDecayMode getTauDecayMode(const shared_ptr<GenParticle>);

        EDGetTokenT<GenParticleCollection> genParticles_;
        string finalState_;
};


DrellYanGenTauTauFilter::DrellYanGenTauTauFilter(const ParameterSet& iConfig) {
    genParticles_ = consumes<GenParticleCollection>(iConfig.getParameter<InputTag>("genparticles"));
    finalState_ = iConfig.getUntrackedParameter<string>("finalState", finalState_);
}


bool DrellYanGenTauTauFilter::filter(Event& event, const EventSetup& setup) {

    Handle<GenParticleCollection> genParticles;
    event.getByToken(genParticles_, genParticles);

    // flag if the current event is Z -> tau tau at generator-level
    bool isZTauTau = false;

    // candidate taus from the Z -> tau tau decay 
    vector<shared_ptr<GenParticle>> tauCandidates = vector<shared_ptr<GenParticle>>();

    // iterate through the collection of generator-level particles
    for (size_t i = 0; i < genParticles->size(); ++i) {

        // the Z boson candidate
        const GenParticle& zBoson = (*genParticles)[i];

        // clear list of tau candidates for each new Z boson candidate
        tauCandidates.clear();

        // if this is not a Z boson (PDG ID 23), skip this particle
        if (abs(zBoson.pdgId()) != 23) {
            continue;
        }

        // count the daughter taus of the selected Z boson
        // also save the indices of tau candidates from the Z boson decay
        size_t nDaughter = static_cast<size_t>(zBoson.numberOfDaughters());
        unsigned int nTaus = 0;
        for (size_t j = 0; j < nDaughter; ++j) {
            const Candidate* cand = zBoson.daughter(j);
            const GenParticle* daughter = dynamic_cast<const GenParticle*>(cand);
            if (abs(daughter->pdgId()) == 15) {
                nTaus += 1;
                tauCandidates.push_back(make_shared<GenParticle>(*daughter));
            }
        }

        // if this is a Z boson with exactly two tau leptons as daughter particles, set the flag to ``true``
        if (nTaus == 2) {
            isZTauTau = true;
            break;
        }

    }

    if (finalState_ == "all") {
        return isZTauTau;
    }

    // investigate the tau tau final state
    vector<DrellYanGenTauTauFilter::TauDecayMode> tauDecayModes = vector<DrellYanGenTauTauFilter::TauDecayMode>();
    for (const shared_ptr<GenParticle> tauCand : tauCandidates) {
        tauDecayModes.push_back(getTauDecayMode(tauCand));
    }


    string finalStateName = "unknown";
    vector<pair<int, int>> indexPairs = vector<pair<int, int>>();
    indexPairs.push_back(pair<int, int>(0, 1));
    indexPairs.push_back(pair<int, int>(1, 0));
    for (const pair<int, int>& indexPair : indexPairs) {
        int i = indexPair.first;
        int j = indexPair.second;
        if (tauDecayModes.at(i) == DrellYanGenTauTauFilter::TauDecayMode::El && tauDecayModes.at(j) == DrellYanGenTauTauFilter::TauDecayMode::Tau) {
            finalStateName = "ElTau";
            break;
        } else if (tauDecayModes.at(i) == DrellYanGenTauTauFilter::TauDecayMode::Mu && tauDecayModes.at(j) == DrellYanGenTauTauFilter::TauDecayMode::Tau) {
            finalStateName = "MuTau";
            break;
        } else if (tauDecayModes.at(i) == DrellYanGenTauTauFilter::TauDecayMode::El && tauDecayModes.at(j) == DrellYanGenTauTauFilter::TauDecayMode::Mu) {
            finalStateName = "ElMu";
            break;
        } else if (tauDecayModes.at(i) == DrellYanGenTauTauFilter::TauDecayMode::Tau && tauDecayModes.at(j) == DrellYanGenTauTauFilter::TauDecayMode::Tau) {
            finalStateName = "TauTau";
            break;
        } else if (tauDecayModes.at(i) == DrellYanGenTauTauFilter::TauDecayMode::El && tauDecayModes.at(j) == DrellYanGenTauTauFilter::TauDecayMode::El) {
            finalStateName = "ElEl";
            break;
        } else if (tauDecayModes.at(i) == DrellYanGenTauTauFilter::TauDecayMode::Mu && tauDecayModes.at(j) == DrellYanGenTauTauFilter::TauDecayMode::Mu) {
            finalStateName = "MuMu";
            break;
        }
    }

    std::cout << "UNKNOWN FINAL STATE!" << std::endl;

    return isZTauTau && (finalState_ == finalStateName);
}


DrellYanGenTauTauFilter::TauDecayMode DrellYanGenTauTauFilter::getTauDecayMode(const shared_ptr<GenParticle> tauCandidate) {
    int eNeutrinos = 0;
    int muNeutrinos = 0;
    int tauNeutrinos = 0;
    size_t nDaughters = static_cast<size_t>(tauCandidate->numberOfDaughters());
    for (size_t i = 0; i < nDaughters; ++i) {
        const Candidate* cand = tauCandidate->daughter(i);
        const GenParticle* d = dynamic_cast<const GenParticle*>(cand);
        const shared_ptr<GenParticle> daughter = make_shared<GenParticle>(*d);

        if (daughter->pdgId() == tauCandidate->pdgId()) {
            return DrellYanGenTauTauFilter::getTauDecayMode(daughter);
        } else if (abs(daughter->pdgId()) == 12) {
            eNeutrinos += 1;
        } else if (abs(daughter->pdgId()) == 14) {
            muNeutrinos += 1;
        } else if (abs(daughter->pdgId()) == 16) {
            tauNeutrinos += 1;
        }
    }

    if (tauNeutrinos == 1 && eNeutrinos == 1 && muNeutrinos == 0) {
        return DrellYanGenTauTauFilter::TauDecayMode::El;
    } else if (tauNeutrinos == 1 && eNeutrinos == 0 && muNeutrinos == 1) {
        return DrellYanGenTauTauFilter::TauDecayMode::Mu;
    } else if (tauNeutrinos == 1 && eNeutrinos == 0 && muNeutrinos == 0) {
        return DrellYanGenTauTauFilter::TauDecayMode::Tau;
    } else {
        return DrellYanGenTauTauFilter::TauDecayMode::Unknown;
    }
}


//define this as a plug-in
DEFINE_FWK_MODULE(DrellYanGenTauTauFilter);
