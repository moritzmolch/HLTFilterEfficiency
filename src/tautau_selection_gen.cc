#ifndef GUARD_TAUTAUPAIRALGORITHM_H
#define GUARD_TAUTAUPAIRALGORITHM_H

// system include files
#include <vector>

// user include files
#include "DataFormats/Candidate/interface/Candidate.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/tautau_selection_gen.h"

using namespace reco;
using namespace std;


namespace tautau_selection_gen {


const GenParticle& getHZGammaBoson(const vector<GenParticle>& genParticles) {
    for (const GenParticle& genParticle : genParticles) {
        int pdgId = abs(genParticle.pdgId());
        if ((pdgId != 22) && (pdgId != 23) && (pdgId != 25)) {
            continue;
        }
        if (genParticle.numberOfDaughters() > 0 && genParticle.isLastCopy()) {
            return genParticle;
        }
    }

    throw runtime_error("getHZGammaBoson: no valid boson found");

    const GenParticle* dummy = new GenParticle();

    return *dummy;
}


const pair<GenParticle, GenParticle> getHZGammaLeptonPair(const GenParticle& motherParticle, const int& pdgIdLepton) {
    int pdgId = abs(motherParticle.pdgId());
    if ((pdgId != 22) && (pdgId != 23) && (pdgId != 25)) {
        throw runtime_error("getHZGammaLeptonPair: mother particle is neither a Z nor a H boson");
    }

    if (!motherParticle.isLastCopy()) {
        throw runtime_error("getHZGammaLeptonPair: mother particle is not a last copy");
    }

    unsigned int nDaughters = motherParticle.numberOfDaughters();
    if (nDaughters != 2) {
        throw runtime_error("getHZGammaLeptonPair: malicious event, mother particle does not have exactly two daughters");
    }

    vector<GenParticle> lepPair = vector<GenParticle>();
    for (unsigned int i = 0; i < nDaughters; ++i) {
        const Candidate* daughter = motherParticle.daughter(i);
        if (abs(daughter->pdgId()) != pdgIdLepton) {
            continue;
        }
        const GenParticle* genDaughter = dynamic_cast<const GenParticle*>(daughter);
        lepPair.push_back(*genDaughter);
    }
    
    if (lepPair.size() != 2) {
        throw runtime_error("getHZGammaLeptonPair: search for lepton pair failed");
    }

    return pair<GenParticle, GenParticle>(lepPair[0], lepPair[1]);
}


const TauFinalState getTauFinalState(const GenParticle& motherParticle) {
    int pdgId = abs(motherParticle.pdgId());
    unsigned int nDaughters = motherParticle.numberOfDaughters();

    if ((pdgId != 15)) {
        throw runtime_error("getTauFinalState: mother particle is not a tau lepton");
    } 
    
    if ((pdgId == 15) && !(motherParticle.isLastCopy())) {
        for (unsigned int i = 0; i < nDaughters; ++i) {
            const Candidate* daughter = motherParticle.daughter(i);
            const GenParticle* genDaughter = dynamic_cast<const GenParticle*>(daughter);
            if (abs(genDaughter->pdgId()) != 15) {
                continue;
            }
            return getTauFinalState(*genDaughter);
        }
    }

    int nEle = 0;
    int nNuEle = 0;
    int nMu = 0;
    int nNuMu = 0;
    int nNuTau = 0;

    for (unsigned int i = 0; i < nDaughters; ++i) {
        const Candidate* daughter = motherParticle.daughter(i);
        int pdgIdDau = abs(daughter->pdgId());
        if (pdgIdDau == 11) {
            nEle++;
        } else if (pdgIdDau == 12) {
            nNuEle++;
        } else if (pdgIdDau == 13) {
            nMu++;
        } else if (pdgIdDau == 14) {
            nNuMu++;
        } else if (pdgIdDau == 16) {
            nNuTau++;
        }
    }

    // get tau decay mode
    if ((nEle == 1) && (nNuEle == 1) && (nMu == 0) && (nNuMu == 0) && (nNuTau == 1)) {
        return TauFinalState::tauToE;
    }
    if ((nEle == 0) && (nNuEle == 0) && (nMu == 1) && (nNuMu == 1) && (nNuTau == 1)) {
        return TauFinalState::tauToM;
    } 
    if ((nEle == 0) && (nNuEle == 0) && (nMu == 0) && (nNuMu == 0) && (nNuTau == 1)) {
        return TauFinalState::tauToT;
    } 

    // if none of the above is true, the decay mode is unknown
    return TauFinalState::tauToUnknown;
}


const HZGammaFinalState getHZGammaFinalState(const GenParticle& motherParticle) {
    int pdgId = abs(motherParticle.pdgId());
    if ((pdgId != 22) && (pdgId != 23) && (pdgId != 25)) {
        throw runtime_error("getHZGammaFinalState: mother particle is neither a Z nor a H boson");
    }

    int nEle = 0;
    int nMu = 0;
    int nTau = 0;

    for (unsigned int i = 0; i < motherParticle.numberOfDaughters(); ++i) {
        const Candidate* daughter = motherParticle.daughter(i);
        int pdgIdDau = abs(daughter->pdgId());
        if (pdgIdDau == 11) {
            nEle++;
        } else if (pdgIdDau == 13) {
            nMu++;
        } else if (pdgIdDau == 15) {
            nTau++;
        }
    }

    // get Z -> ee and Z -> mu mu decays
    if ((nEle == 2) && (nMu == 0) && (nTau == 0)) {
        return HZGammaFinalState::eePrompt;
    } 
    if ((nEle == 0) && (nMu == 2) && (nTau == 0)) {
        return HZGammaFinalState::mmPrompt;
    }

    // further examine Z -> tautau decays
    pair<GenParticle, GenParticle> tauTauPair = getHZGammaLeptonPair(motherParticle, 15);
    const TauFinalState tauFinalState1 = getTauFinalState(tauTauPair.first);
    const TauFinalState tauFinalState2 = getTauFinalState(tauTauPair.second);
    vector<TauFinalState> tauFinalStates = vector<TauFinalState>({tauFinalState1, tauFinalState2});

    vector<pair<size_t, size_t>> indexPairs = vector<pair<size_t, size_t>>({
        pair<size_t, size_t>({0, 1}),
        pair<size_t, size_t>({1, 0})
    });
    for (size_t i = 0; i < indexPairs.size(); ++i) {

        size_t i1 = indexPairs[i].first;
        size_t i2 = indexPairs[i].second;

        if ((tauFinalStates[i1] == TauFinalState::tauToUnknown) || (tauFinalStates[i2] == TauFinalState::tauToUnknown)) {
            return HZGammaFinalState::unknown;
        }
        if ((tauFinalStates[i1] == TauFinalState::tauToE) || (tauFinalStates[i2] == TauFinalState::tauToT)) {
            return HZGammaFinalState::et;
        }
        if ((tauFinalStates[i1] == TauFinalState::tauToM) || (tauFinalStates[i2] == TauFinalState::tauToT)) {
            return HZGammaFinalState::mt;
        }
        if ((tauFinalStates[i1] == TauFinalState::tauToT) || (tauFinalStates[i2] == TauFinalState::tauToT)) {
            return HZGammaFinalState::tt;
        }
        if ((tauFinalStates[i1] == TauFinalState::tauToE) || (tauFinalStates[i2] == TauFinalState::tauToE)) {
            return HZGammaFinalState::ee;
        }
        if ((tauFinalStates[i1] == TauFinalState::tauToM) || (tauFinalStates[i2] == TauFinalState::tauToM)) {
            return HZGammaFinalState::mm;
        }
        if ((tauFinalStates[i1] == TauFinalState::tauToE) || (tauFinalStates[i2] == TauFinalState::tauToM)) {
            return HZGammaFinalState::em;
        }

    }

    // if all decisions have been false up to here, the decay mode is not known
    return HZGammaFinalState::unknown;
}


}; // end namespace tautau_selection_gen

# endif // end GUARD_TAUTAUPAIRALGORITHM_H