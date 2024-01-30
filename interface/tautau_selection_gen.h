#ifndef GUARD_TAUTAU_SELECTION_GEN_H
#define GUARD_TAUTAU_SELECTION_GEN_H

// system include files
#include <vector>

// user include files
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"

using namespace reco;
using namespace std;


namespace tautau_selection_gen {


enum HZGammaFinalState {
    eePrompt,
    mmPrompt,
    et,
    mt,
    tt,
    ee,
    mm,
    em,
    unknown
};


enum TauFinalState {
    tauToE,
    tauToM,
    tauToT,
    tauToUnknown
};


const GenParticle& getHZGammaBoson(const vector<GenParticle>&);


const pair<GenParticle, GenParticle> getHZGammaLeptonPair(const GenParticle&, const int&);


const TauFinalState getTauFinalState(const GenParticle&);


const HZGammaFinalState getHZGammaFinalState(const GenParticle&);


}; // end namespace tautau_selection_gen

# endif // end GUARD_TAUTAU_SELECTION_GEN_H