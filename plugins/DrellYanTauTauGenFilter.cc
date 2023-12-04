// system include files

#include <memory>
#include <cmath>


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

        EDGetTokenT<GenParticleCollection> genParticles_;
};


DrellYanGenTauTauFilter::DrellYanGenTauTauFilter(const ParameterSet& iConfig) {
    genParticles_ = consumes<GenParticleCollection>(iConfig.getParameter<InputTag>("genparticles"));
}


bool DrellYanGenTauTauFilter::filter(Event& event, const EventSetup& setup) {

    Handle<GenParticleCollection> genParticles;
    event.getByToken(genParticles_, genParticles);

    // flag if the current event is Z -> tau tau at generator-level
    bool isZTauTau = false;

    // candidate taus from the Z -> tau tau decay 
    //vector<Candidate&> tauCandidates = vector<Candidate&>();

    // iterate through the collection of generator-level particles
    for (size_t i = 0; i < genParticles->size(); ++i) {

        // the Z boson candidate
        const GenParticle& zBoson = (*genParticles)[i];

        // clear list of tau candidates for each new Z boson candidate
        //tauCandidates.clear();

        // if this is not a Z boson (PDG ID 23), skip this particle
        if (abs(zBoson.pdgId()) != 23) {
            continue;
        }

        // count the daughter taus of the selected Z boson
        // also save the indices of tau candidates from the Z boson decay
        size_t nDaughter = static_cast<size_t>(zBoson.numberOfDaughters());
        unsigned int nTaus = 0;
        for (size_t j = 0; j < nDaughter; ++j) {
            const Candidate* d = zBoson.daughter(j);
            if (abs(d->pdgId()) == 15) {
                nTaus += 1;
                //tauCandidates.push_back(d);
            }
        }

        // if this is a Z boson with exactly two tau leptons as daughter particles, set the flag to ``true``
        if (nTaus == 2) {
            isZTauTau = true;
            break;
        }

    }

    std::cout << isZTauTau << std::endl;

    return isZTauTau;

    // if this is not a Z -> tau tau event, it is skipped
    //if (!isZTauTau) {
    //    return false;
    //}

    // further investigate the tau tau final state

}


//define this as a plug-in
DEFINE_FWK_MODULE(DrellYanGenTauTauFilter);
