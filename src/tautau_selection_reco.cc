// system include files
#include <vector>

// user include files
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

#include "Math/Vector4D.h"

#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/tautau_selection_reco.h"
#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/util.h"

using namespace pat;
using namespace ROOT::Math;
using namespace std;
using namespace util;


namespace tautau_selection_reco {


const bool comparePairs(const pair<SortScore, SortScore>& prevPair,  const pair<SortScore, SortScore>& nextPair) {
    const SortScore prevDauOne = prevPair.first;
    const SortScore prevDauTwo = prevPair.second;
    const SortScore nextDauOne = nextPair.first;
    const SortScore nextDauTwo = nextPair.second;

    // compare isolation of the first objects in the pair
    if (!approxEqual(prevDauOne.isoScore, nextDauOne.isoScore)) {
        return prevDauOne.isoScore > nextDauOne.isoScore;
    }

    // compare the pt of the first objects in the pair
    if (!approxEqual(prevDauOne.ptScore, nextDauOne.ptScore)) {
        return prevDauOne.ptScore > nextDauOne.ptScore;
    }

    // compare isolation of the second objects in the pair
    if (!approxEqual(prevDauTwo.isoScore, nextDauTwo.isoScore)) {
        return prevDauTwo.isoScore > nextDauTwo.isoScore;
    }

    // compare the pt of the second objects in the pair
    if (!approxEqual(prevDauTwo.ptScore, nextDauTwo.ptScore)) {
        return prevDauTwo.ptScore > nextDauTwo.ptScore;
    }

    return false;
}


TauTauPairAlgorithm::TauTauPairAlgorithm(
    const vector<Electron>& electrons,
    const vector<Muon>& muons,
    const vector<Tau>& taus,
    const vector<Electron>& vetoElectrons,
    const vector<Muon>& vetoMuons
) {
    electrons_ = electrons;
    muons_ = muons;
    taus_ = taus;
    vetoElectrons_ = vetoElectrons;
    vetoMuons_ = vetoMuons;

    hasBeenExecuted_ = false;
    finalState_ = TauTauFinalState::unknown;
    pair<Electron, Tau> etPair_;
    pair<Muon, Tau> mtPair_;
    pair<Tau, Tau> ttPair_;   
}


void TauTauPairAlgorithm::execute() {
    // execute the pair finding algorithm for the three considered final states
    const bool foundET = findPairET();
    const bool foundMT = findPairMT();
    const bool foundTT = findPairTT();

    // flag for not having found a valid pair
    const bool foundNone = !(foundET || foundMT || foundTT);

    // consistency check if the final state is well-defined
    if (
        static_cast<int>(foundET) + static_cast<int>(foundMT) + static_cast<int>(foundTT) + static_cast<int>(foundNone) != 1)
    {
        throw logic_error("TauTauPairAlgorithm: found pair in more than one final state");
    }

    // set final state to none if no tau pair has been found
    if (foundNone) {
        finalState_ = TauTauFinalState::unknown;
    }

    // set execution flag
    hasBeenExecuted_ = true;
}


const TauTauFinalState TauTauPairAlgorithm::getFinalState() const {
    if (!hasBeenExecuted_) {
        throw runtime_error("TauTauPairAlgorithm: algorithm has not been run with execute() method");
    }
    return finalState_;
}


const pair<Electron, Tau> TauTauPairAlgorithm::getPairET() const {
    if (!hasBeenExecuted_) {
        throw runtime_error("TauTauPairAlgorithm: algorithm has not been run with execute() method");
    }
    if (!(finalState_ == TauTauFinalState::et)) {
        throw runtime_error("TauTauPairAlgorithm: trying to get electron-tau pair in event with final state different from electron-tau");
    }
    return etPair_;
}


const pair<Muon, Tau> TauTauPairAlgorithm::getPairMT() const {
    if (!hasBeenExecuted_) {
        throw runtime_error("TauTauPairAlgorithm: algorithm has not been run with execute() method");
    }
    if (!(finalState_ == TauTauFinalState::mt)) {
        throw runtime_error("TauTauPairAlgorithm: trying to get muon-tau pair in event with final state different from muon-tau");
    }
    return mtPair_;
}


const pair<Tau, Tau> TauTauPairAlgorithm::getPairTT() const {
    if (!hasBeenExecuted_) {
        throw runtime_error("TauTauPairAlgorithm: algorithm has not been run with execute() method");
    }
    if (!(finalState_ == TauTauFinalState::tt)) {
        throw runtime_error("TauTauPairAlgorithm: trying to get muon-tau pair in event with final state different from muon-tau");
    }
    return ttPair_;
}


const bool TauTauPairAlgorithm::findPairET() {
    vector<pair<size_t, size_t>> pairIndex = vector<pair<size_t, size_t>>();
    vector<pair<SortScore, SortScore>> pairSortScore = vector<pair<SortScore, SortScore>>();

    // find pairs that fulfill the charge, the ID and the deltaR requirements
    for (size_t iEle = 0; iEle < electrons_.size(); ++iEle) {
        const Electron& electron = electrons_[iEle];
        for (size_t iTau = 0; iTau < taus_.size(); ++iTau) {
            const Tau& tau = taus_[iTau];
            const PackedCandidate* leadChargedHadrCand = dynamic_cast<const PackedCandidate*>(tau.leadChargedHadrCand().get());
            const float dz = leadChargedHadrCand ? abs(leadChargedHadrCand->dz()) : 0.;
            if (
                (dz < 0.2)
                && (electron.charge() * tau.charge() < 0)
                && ((tau.tauID("byTightDeepTau2017v2p1VSe") > 0.5) && (tau.tauID("byVLooseDeepTau2017v2p1VSmu") > 0.5) && (tau.tauID("byTightDeepTau2017v2p1VSjet") > 0.5))
                && (getDeltaR(electron.p4(), tau.p4()) > 0.5)
            ) {
                pairIndex.push_back(pair<size_t, size_t>(iEle, iTau));
                pairSortScore.push_back(pair<SortScore, SortScore>({
                    SortScore{-electron.userFloat("PFIsoAll"), electron.pt()},
                    SortScore{tau.tauID("byDeepTau2017v2p1VSjetraw"), tau.pt()}
                }));
            }
        }
    }

    // sort the pairs
    vector<size_t> indexSorted = vector<size_t>();
    for (size_t i = 0; i < pairIndex.size(); ++i) {
        indexSorted.push_back(i);
    }
    stable_sort(
        indexSorted.begin(),
        indexSorted.end(),
        [&pairSortScore] (size_t i1, size_t i2) { return comparePairs(pairSortScore[i1], pairSortScore[i2]); }
    );

    // calculate vetoes
    bool isVetoed = !((vetoElectrons_.size() == 1) && (vetoMuons_.size() == 0));

    // return false if no valid tau tau pair has been found or the event is vetoed
    if (indexSorted.size() == 0 || isVetoed) {
        return false;
    }

    // set final state and e-tau pair
    finalState_ = TauTauFinalState::et;
    etPair_ = pair<Electron, Tau>({electrons_[pairIndex[indexSorted[0]].first], taus_[pairIndex[indexSorted[0]].second]});

    // return that algorithm has been run successfully
    return true;
}


const bool TauTauPairAlgorithm::findPairMT() {
    vector<pair<size_t, size_t>> pairIndex = vector<pair<size_t, size_t>>();
    vector<pair<SortScore, SortScore>> pairSortScore = vector<pair<SortScore, SortScore>>();

    // find pairs that fulfill the charge, the ID and the deltaR requirements
    for (size_t iMuon = 0; iMuon < muons_.size(); ++iMuon) {
        const Muon& muon = muons_[iMuon];
        for (size_t iTau = 0; iTau < taus_.size(); ++iTau) {
            const Tau& tau = taus_[iTau];
            const PackedCandidate* leadChargedHadrCand = dynamic_cast<const PackedCandidate*>(tau.leadChargedHadrCand().get());
            const float dz = leadChargedHadrCand ? abs(leadChargedHadrCand->dz()) : 0.;
            if (
                (dz < 0.2)
                && (muon.charge() * tau.charge() < 0)
                && ((tau.tauID("byVVLooseDeepTau2017v2p1VSe") > 0.5) && (tau.tauID("byTightDeepTau2017v2p1VSmu") > 0.5) && (tau.tauID("byTightDeepTau2017v2p1VSjet") > 0.5))
                && (getDeltaR(muon.p4(), tau.p4()) > 0.5)
            ) {
                double muonIso = (
                    muon.pfIsolationR04().sumChargedHadronPt +
                    max(muon.pfIsolationR04().sumNeutralHadronEt + muon.pfIsolationR04().sumPhotonEt - 0.5 * muon.pfIsolationR04().sumPUPt, 0.0)
                ) / muon.pt();
                pairIndex.push_back(pair<size_t, size_t>(iMuon, iTau));
                pairSortScore.push_back(pair<SortScore, SortScore>({
                    SortScore{-muonIso, muon.pt()},
                    SortScore{tau.tauID("byDeepTau2017v2p1VSjetraw"), tau.pt()}
                }));
            }
        }
    }

    // sort the pairs
    vector<size_t> indexSorted = vector<size_t>();
    for (size_t i = 0; i < pairIndex.size(); ++i) {
        indexSorted.push_back(i);
    }
    stable_sort(
        indexSorted.begin(),
        indexSorted.end(),
        [&pairSortScore] (size_t i1, size_t i2) { return comparePairs(pairSortScore[i1], pairSortScore[i2]); }
    );

    // calculate vetoes
    bool isVetoed = !((vetoElectrons_.size() == 0) && (vetoMuons_.size() == 1));

    // return false if no valid tau tau pair has been found or the event is vetoed
    if (indexSorted.size() == 0 || isVetoed) {
        return false;
    }

    // set final state and mu-tau pair
    finalState_ = TauTauFinalState::mt;
    mtPair_ = pair<Muon, Tau>({muons_[pairIndex[indexSorted[0]].first], taus_[pairIndex[indexSorted[0]].second]});

    // return that algorithm has been run successfully
    return true;
}


const bool TauTauPairAlgorithm::findPairTT() {
    vector<pair<size_t, size_t>> pairIndex = vector<pair<size_t, size_t>>();
    vector<pair<SortScore, SortScore>> pairSortScore = vector<pair<SortScore, SortScore>>();

    // find pairs that fulfill the charge, the ID and the deltaR requirements
    for (size_t iTau1 = 0; iTau1 < taus_.size(); ++iTau1) {
        const Tau& tau1 = taus_[iTau1];
        const PackedCandidate* leadChargedHadrCand1 = dynamic_cast<const PackedCandidate*>(tau1.leadChargedHadrCand().get());
        const float dz1 = leadChargedHadrCand1 ? abs(leadChargedHadrCand1->dz()) : 0.;
        if (dz1 >= 0.2) {
            continue;
        }
        for (size_t iTau2 = 0; iTau2 < taus_.size(); ++iTau2) {
            if (iTau1 == iTau2) {
                continue;
            }
            const Tau& tau2 = taus_[iTau2];
            const PackedCandidate* leadChargedHadrCand2 = dynamic_cast<const PackedCandidate*>(tau2.leadChargedHadrCand().get());
            const float dz2 = leadChargedHadrCand2 ? abs(leadChargedHadrCand2->dz()) : 0.;
            if (
                (dz2 < 0.2)
                && (tau1.charge() * tau2.charge() < 0)
                && ((tau1.tauID("byVVLooseDeepTau2017v2p1VSe") > 0.5) && (tau1.tauID("byVLooseDeepTau2017v2p1VSmu") > 0.5) && (tau1.tauID("byTightDeepTau2017v2p1VSjet") > 0.5))
                && ((tau2.tauID("byVVLooseDeepTau2017v2p1VSe") > 0.5) && (tau2.tauID("byVLooseDeepTau2017v2p1VSmu") > 0.5) && (tau2.tauID("byTightDeepTau2017v2p1VSjet") > 0.5))
                && (getDeltaR(tau1.p4(), tau2.p4()) > 0.5)
            ) {
                pairIndex.push_back(pair<size_t, size_t>(iTau1, iTau2));
                pairSortScore.push_back(pair<SortScore, SortScore>({
                    SortScore{tau1.tauID("byDeepTau2017v2p1VSjetraw"), tau1.pt()},
                    SortScore{tau2.tauID("byDeepTau2017v2p1VSjetraw"), tau2.pt()}
                }));
            }
        }
    }

    // sort the pairs
    vector<size_t> indexSorted = vector<size_t>();
    for (size_t i = 0; i < pairIndex.size(); ++i) {
        indexSorted.push_back(i);
    }
    stable_sort(
        indexSorted.begin(),
        indexSorted.end(),
        [&pairSortScore] (size_t i1, size_t i2) { return comparePairs(pairSortScore[i1], pairSortScore[i2]); }
    );

    // calculate vetoes
    bool isVetoed = !((vetoElectrons_.size() == 0) && (vetoMuons_.size() == 0));

    // return false if no valid tau tau pair has been found or the event is vetoed
    if (indexSorted.size() == 0 || isVetoed) {
        return false;
    }

    // set final state and mu-tau pair
    finalState_ = TauTauFinalState::tt;
    ttPair_ = pair<Tau, Tau>({taus_[pairIndex[indexSorted[0]].first], taus_[pairIndex[indexSorted[0]].second]});

    // return that algorithm has been run successfully
    return true;
}


}; // end namespace tautau_selection_reco