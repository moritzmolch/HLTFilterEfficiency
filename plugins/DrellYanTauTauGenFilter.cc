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

    private:
        bool filter(edm::Event&, const edm::EventSetup &);
        string getTauDecayMode(const Candidate*);

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

    string genDecayMode1 = "Unknown";
    string genDecayMode2 = "Unknown";

    // iterate through the collection of generator-level particles
    for (size_t i = 0; i < genParticles->size(); ++i) {

        // the Z boson candidate
        const GenParticle& zBoson = (*genParticles)[i];

        // if this is not a Z boson (PDG ID 23), skip this particle
        if (abs(zBoson.pdgId()) != 23) {
            continue;
        }

        // count the daughter taus of the selected Z boson
        // also save the indices of tau candidates from the Z boson decay
        size_t nDaughter = static_cast<size_t>(zBoson.numberOfDaughters());
        unsigned int nTaus = 0;
        for (size_t j = 0; j < nDaughter; ++j) {

            const Candidate* daughter = zBoson.daughter(j);
            if (abs(daughter->pdgId()) == 15) {
                nTaus += 1;

                // determine decay mode of tau leptons
                if (nTaus == 1) {
                    genDecayMode1 = getTauDecayMode(daughter);
                } else if (nTaus == 2) {
                    genDecayMode2 = getTauDecayMode(daughter);
                }

            }
        }

        // if this is a Z boson with exactly two tau leptons as daughter particles, set the flag to ``true``
        if (nTaus == 2) {
            isZTauTau = true;
            break;
        }

        // reset genDecayMode information
        genDecayMode1 = "Unknown";
        genDecayMode2 = "Unknown";
    }

    if (finalState_ == "all" || !(isZTauTau)) {
        return isZTauTau;
    }

    string finalStateName;
    if (genDecayMode1 == "Unknown") {
        finalStateName = "Unknown";
    } else if (genDecayMode1 == "El") {
        if (genDecayMode2 == "Unknown") {
            finalStateName = "Unknown";
        } else {
            finalStateName = genDecayMode1 + genDecayMode2; 
        }
    } else if (genDecayMode1 == "Mu") {
        if (genDecayMode2 == "Unknown") {
            finalStateName = "Unknown";
        } else if (genDecayMode2 == "El") {
            finalStateName = "ElMu";
        } else {
            finalStateName = genDecayMode1 + genDecayMode2; 
        }
    } else if (genDecayMode1 == "Tau") {
        if (genDecayMode2 == "Unknown") {
            finalStateName = "Unknown";
        } else if (genDecayMode2 == "El") {
            finalStateName = "ElTau";
        } else if (genDecayMode2 == "Mu") {
            finalStateName = "MuTau";
        } else {
            finalStateName = genDecayMode1 + genDecayMode2; 
        }
    } else {
        finalStateName = "Unknown";
    }

    if (finalStateName == "Unknown") {
        std::cout << "WARNING: UNKNOWN FINAL STATE!" << std::endl;
    }

    return isZTauTau && (finalState_ == finalStateName);
}


string DrellYanGenTauTauFilter::getTauDecayMode(const Candidate* candidate) {
    int eNeutrinos = 0;
    int muNeutrinos = 0;
    int tauNeutrinos = 0;
    size_t nDaughters = static_cast<size_t>(candidate->numberOfDaughters());
    for (size_t i = 0; i < nDaughters; ++i) {
        const Candidate* daughter = candidate->daughter(i);

        if (daughter->pdgId() == candidate->pdgId()) {
            return DrellYanGenTauTauFilter::getTauDecayMode(daughter);
        }

        if (abs(daughter->pdgId()) == 12) {
            eNeutrinos += 1;
        } else if (abs(daughter->pdgId()) == 14) {
            muNeutrinos += 1;
        } else if (abs(daughter->pdgId()) == 16) {
            tauNeutrinos += 1;
        }
    }

    if (tauNeutrinos == 1 && eNeutrinos == 1 && muNeutrinos == 0) {
        return "El";
    } else if (tauNeutrinos == 1 && eNeutrinos == 0 && muNeutrinos == 1) {
        return "Mu";
    } else if (tauNeutrinos == 1 && eNeutrinos == 0 && muNeutrinos == 0) {
        return "Tau";
    }
    
    return "Unknown";
}


//define this as a plug-in
DEFINE_FWK_MODULE(DrellYanGenTauTauFilter);
