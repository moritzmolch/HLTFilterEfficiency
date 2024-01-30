#ifndef GUARD_TAUTAU_SELECTION_RECO_H
#define GUARD_TAUTAU_SELECTION_RECO_H

// system include files
#include <vector>

// user include files
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

#include "Math/Vector4D.h"

#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/util.h"

using namespace pat;
using namespace ROOT::Math;
using namespace std;


namespace tautau_selection_reco {


enum TauTauFinalState {
    et,
    mt,
    tt,
    ee,
    mm,
    em,
    unknown
};


struct SortScore {
    double isoScore;
    double ptScore;
};


const bool comparePairs(const pair<SortScore, SortScore>&,  const pair<SortScore, SortScore>&);


class TauTauPairAlgorithm {

public:
    TauTauPairAlgorithm(const vector<Electron>&, const vector<Muon>&, const vector<Tau>&, const vector<Electron>&, const vector<Muon>&);

    void execute();
    const TauTauFinalState getFinalState() const;
    const pair<Electron, Tau> getPairET() const;
    const pair<Muon, Tau> getPairMT() const;
    const pair<Tau, Tau> getPairTT() const;

private:
    const bool findPairET();
    const bool findPairMT();
    const bool findPairTT();

    vector<Electron> electrons_;
    vector<Muon> muons_;
    vector<Tau> taus_;
    vector<Electron> vetoElectrons_;
    vector<Muon> vetoMuons_;

    bool hasBeenExecuted_;
    TauTauFinalState finalState_;
    pair<Electron, Tau> etPair_;
    pair<Muon, Tau> mtPair_;
    pair<Tau, Tau> ttPair_;
}; // end class TauTauPairAlgorithm


}; // end namespace tautau_selection_reco

# endif // end GUARD_TAUTAU_SELECTION_RECO_H