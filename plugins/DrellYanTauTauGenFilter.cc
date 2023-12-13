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

enum ZDecayMode {
    UnknownZ,
    ElElPrompt,
    MuMuPrompt,
    ElTauHad,
    MuTauHad,
    TauHadTauHad,
    ElEl,
    MuMu,
    ElMu
};


enum TauDecayMode {
    UnknownTau,
    El,
    Mu,
    TauHad
};


class DrellYanGenTauTauFilter : public one::EDFilter<one::SharedResources> {

    public:
        explicit DrellYanGenTauTauFilter(const ParameterSet&);
        ~DrellYanGenTauTauFilter();
    
    private:
        bool filter(Event&, const EventSetup&) override;
        const GenParticle* getZBoson(const GenParticleCollection&, bool&);
        const ZDecayMode getZDecayModeFromString(const string&);
        const ZDecayMode getZBosonDecayMode(const GenParticle*);
        const ZDecayMode getTauTauDecayMode(const GenParticle*);
        const TauDecayMode getTauLeptonDecayMode(const GenParticle*);
        const bool passesEmbeddingDiLeptonSelection(const GenParticle*);

        EDGetTokenT<GenParticleCollection> genParticles_;
        string finalStateName_;
        ZDecayMode finalState_;
};


DrellYanGenTauTauFilter::DrellYanGenTauTauFilter(const ParameterSet& iConfig) {
    genParticles_ = consumes<GenParticleCollection>(iConfig.getParameter<InputTag>("genparticles"));
    finalStateName_ = iConfig.getUntrackedParameter<string>("finalState", finalStateName_);
    finalState_ = getZDecayModeFromString(finalStateName_);
}


DrellYanGenTauTauFilter::~DrellYanGenTauTauFilter() {}


const ZDecayMode DrellYanGenTauTauFilter::getZDecayModeFromString(const string& zDecayModeName) {
    if (zDecayModeName == "ElElPrompt") {
        return ZDecayMode::ElElPrompt;
    } else if (zDecayModeName == "MuMuPrompt") {
        return ZDecayMode::MuMuPrompt;
    } else if (zDecayModeName == "ElEl") {
        return ZDecayMode::ElEl;
    } else if (zDecayModeName == "MuMu") {
        return ZDecayMode::MuMu;
    } else if (zDecayModeName == "ElMu") {
        return ZDecayMode::ElMu;
    } else if (zDecayModeName == "ElTauHad") {
        return ZDecayMode::ElTauHad;
    } else if (zDecayModeName == "MuTauHad") {
        return ZDecayMode::MuTauHad;
    } else if (zDecayModeName == "TauHadTauHad") {
        return ZDecayMode::TauHadTauHad;
    }
            
    throw invalid_argument("Z decay mode not known");
}


const GenParticle* DrellYanGenTauTauFilter::getZBoson(
    const GenParticleCollection& genParticles, bool& wasFound
) {
    for (const GenParticle& particle : genParticles) {
        if (abs(particle.pdgId()) != 23) {
            // continue with the next particle if this is not a Z boson
            continue;
        }

        if (particle.isLastCopy() && particle.numberOfDaughters() == 2) {
            // if this is the last copy of a Z boson and it has exactly two daughter particles,
            // this is the particle that we wanted to find
            wasFound = true;
            return &particle;
        } else if (particle.isLastCopy() && particle.numberOfDaughters() != 2) {
            // this should not happen, last copies of a Z boson in DY -> leptons simulation
            // should always have exacly two daughters
            char buffer[256];
            snprintf(
                buffer,
                256,
                "number of Z Boson daughters is %lu, expected exactly 2 daughter particles",
                particle.numberOfDaughters()
            );
            throw invalid_argument(buffer);
        }
    }

    // if no valid Z boson has been found, return a null pointer
    wasFound = false;
    return nullptr;
}


const ZDecayMode DrellYanGenTauTauFilter::getZBosonDecayMode(
    const GenParticle* zBoson
) {

    // stage immediately out if the input particle is not valid
    if (abs(zBoson->pdgId()) != 23 || zBoson->numberOfDaughters() != 2) {
        return ZDecayMode::UnknownZ;
    }

    // get the PDG IDs of both daughters to get the flavour of the lepton pair
    int pdgId1 = abs(zBoson->daughter(0)->pdgId());
    int pdgId2 = abs(zBoson->daughter(1)->pdgId());

    if (pdgId1 == 11 && pdgId2 == 11) {
        // this is just Z -> e e
        return ZDecayMode::ElElPrompt;
    } else if (pdgId1 == 13 && pdgId2 == 13) {
        // this is just Z -> mu mu
        return ZDecayMode::MuMuPrompt;
    } else if (pdgId1 == 15 && pdgId2 == 15) {
        // this is Z -> tau tau, final states of lepton pair have to be distinguished
        return getTauTauDecayMode(zBoson);
    }

    // when this point is reached, the state of the Z decay is unknown
    return ZDecayMode::UnknownZ;
};


const ZDecayMode DrellYanGenTauTauFilter::getTauTauDecayMode(
    const GenParticle* zBoson
) {
    // stage immediately out if the input particle is not valid
    if (abs(zBoson->pdgId()) != 23 || zBoson->numberOfDaughters() != 2) {
        return ZDecayMode::UnknownZ;
    }

    // get the decay modes of the two individual tau leptons
    vector<TauDecayMode> decayModes = vector<TauDecayMode>();
    for (unsigned int i = 0; i < zBoson->numberOfDaughters(); ++i) {
        const Candidate* candidate = zBoson->daughter(i);
        const GenParticle* daughter = static_cast<const GenParticle*>(candidate);
        decayModes.push_back(getTauLeptonDecayMode(daughter));
    }

    // the list must have length 2, otherwise some checks before have not been strict enough
    if (decayModes.size() != 2) {
        throw out_of_range("vector of tau decay modes must have length of 2");
    }

    // evaluate the decay mode 
    vector<pair<int, int>> indices = {{0, 1}, {1, 0}};
    for (const pair<int, int>& index : indices) {
        int i = index.first;
        int j = index.second;
        
        if ((decayModes.at(i) == TauDecayMode::UnknownTau) || (decayModes.at(j) == TauDecayMode::UnknownTau)) {
            return ZDecayMode::UnknownZ;
        } else if ((decayModes.at(i) == TauDecayMode::El) && (decayModes.at(j) == TauDecayMode::El)) {
            return ZDecayMode::ElEl;
        } else if ((decayModes.at(i) == TauDecayMode::Mu) && (decayModes.at(j) == TauDecayMode::Mu)) {
            return ZDecayMode::MuMu;
        } else if ((decayModes.at(i) == TauDecayMode::El) && (decayModes.at(j) == TauDecayMode::Mu)) {
            return ZDecayMode::ElMu;
        } else if ((decayModes.at(i) == TauDecayMode::El) && (decayModes.at(j) == TauDecayMode::TauHad)) {
            return ZDecayMode::ElTauHad;
        } else if ((decayModes.at(i) == TauDecayMode::Mu) && (decayModes.at(j) == TauDecayMode::TauHad)) {
            return ZDecayMode::MuTauHad;
        } else if ((decayModes.at(i) == TauDecayMode::TauHad) && (decayModes.at(j) == TauDecayMode::TauHad)) {
            return ZDecayMode::TauHadTauHad;
        }
    }

    // when this point is reached, the decay mode is not defined

    return ZDecayMode::UnknownZ;
}


const TauDecayMode DrellYanGenTauTauFilter::getTauLeptonDecayMode(const GenParticle* tauLepton) {
    // stage immediately out if the input particle is not valid
    if (abs(tauLepton->pdgId()) != 15) {
        return TauDecayMode::UnknownTau;
    }

    if (!(tauLepton->isLastCopy()) && (tauLepton->numberOfDaughters() > 0)) {
        // if this is not the last copy of a tau lepton, get to it by executing this method
        // recursively
        for (unsigned int i = 0; i < tauLepton->numberOfDaughters(); ++i) {
            const Candidate* candidate = tauLepton->daughter(i);
            const GenParticle* tauDaughter = static_cast<const GenParticle*>(candidate);
            if (tauLepton->pdgId() == tauDaughter->pdgId()) {
                return DrellYanGenTauTauFilter::getTauLeptonDecayMode(tauDaughter);
            }
        }

        // if no daughter particle with the same PDG ID like the original tau lepton has been
        // found, something is wrong with th event
        throw invalid_argument(
            "tau lepton is not last copy but does also not contain a matching tau lepton in the list of daughter particles"
        );
    } else if (!(tauLepton->isLastCopy()) && (tauLepton->numberOfDaughters() == 0)) {
        // this should not happen, if a tau lepton is not a last copy, it should have at least one
        // daughter particle
        char buffer[256];
        snprintf(
            buffer,
            256,
            "number of tau lepton daughters is %lu, expected at least one daughter particle",
            tauLepton->numberOfDaughters()
        );
        throw invalid_argument(buffer);
    }

    // counters for daughter particles
    int nEl = 0; // number of electrons
    int nMu = 0; // number of muons
    int nNuEl = 0; // number of electron-neutrinos
    int nNuMu = 0; // number of muon-neutrinos
    int nNuTau = 0; // number of tau-neutrinos

    // iterate through the daughters of the tau lepton and increase counters
    for (unsigned int i = 0; i < tauLepton->numberOfDaughters(); ++i) {
        const Candidate* daughter = tauLepton->daughter(i);
        if (abs(daughter->pdgId()) == 11) { 
            nEl += 1;
        } else if (abs(daughter->pdgId()) == 12) {
            nNuEl += 1;
        } else if (abs(daughter->pdgId()) == 13) {
            nMu += 1;
        } else if (abs(daughter->pdgId()) == 14) {
            nNuMu += 1;
        } else if (abs(daughter->pdgId()) == 16) {
            nNuTau += 1;
        }
    }

    // depending on the number of particles of different flavors in the final state, decide which
    // tau decay mode is returned
    if ((nEl == 1) && (nMu == 0) && (nNuEl == 1) && (nNuMu == 0) && (nNuTau == 1)) {
        return TauDecayMode::El;
    } else if ((nEl == 0) && (nMu == 1) && (nNuEl == 0) && (nNuMu == 1) && (nNuTau == 1)) {
        return TauDecayMode::Mu;
    } else if ((nEl == 0) && (nMu == 0) && (nNuEl == 0) && (nNuMu == 0) && (nNuTau == 1)) {
        return TauDecayMode::TauHad;
    }

    // if no decision has been made until here, the state of the decay is undefined
    return TauDecayMode::UnknownTau;
}


const bool DrellYanGenTauTauFilter::passesEmbeddingDiLeptonSelection(
    const GenParticle* zBoson
) {
    // stage immediately out if the input particle is not valid
    if (abs(zBoson->pdgId()) != 23 || zBoson->numberOfDaughters() != 2) {
        return ZDecayMode::UnknownZ;
    }

    // get the four-vectors of the leading and the trailing lepton
    const reco::Candidate::LorentzVector& p_1 = zBoson->daughter(0)->p4();
    const reco::Candidate::LorentzVector& p_2 = zBoson->daughter(1)->p4();

    // pt of the leading lepton must be greater than 17 GeV
    if (max(p_1.pt(), p_2.pt()) <= 17) {
        return false;
    }

    // invariant mass of the lepton pair must be greater than 20 GeV
    const reco::Candidate::LorentzVector& p_dilep = p_1 + p_2;
    if (p_dilep.mass() <= 20) {
        return false;
    }

    // if the selections above are passed, return that this event is selected
    return true;
}


bool DrellYanGenTauTauFilter::filter(Event& event, const EventSetup& setup) {

    Handle<GenParticleCollection> genParticles;
    event.getByToken(genParticles_, genParticles);

    bool wasFound = false;
    const GenParticle* zBoson = getZBoson((*genParticles), wasFound);
    
    // stage out if no valid Z boson has been found
    if (!wasFound || zBoson == nullptr) {
        return false;
    }

    // check if the event passes the embedding-like dilepton selection
    if (!passesEmbeddingDiLeptonSelection(zBoson)) {
        return false;
    }

    // get the Z boson decay mode
    const ZDecayMode zDecayMode = getZBosonDecayMode(zBoson);

    return (zDecayMode == finalState_);
}


//define this as a plug-in
DEFINE_FWK_MODULE(DrellYanGenTauTauFilter);
