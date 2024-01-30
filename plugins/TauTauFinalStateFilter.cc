// system include files
#include <memory>
#include <cmath>
#include <regex>
#include <vector>
#include <algorithm>
#include <iostream>

// user include files

#include "CommonTools/UtilAlgos/interface/TFileService.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/Tau.h"

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDFilter.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/InputTag.h"


using namespace edm;
using namespace pat;
using namespace std;


class TauTauFinalStateFilter : public one::EDFilter<one::SharedResources> {

    public:
        explicit TauTauFinalStateFilter(const ParameterSet&);
        ~TauTauFinalStateFilter();
        static void fillDescriptions(ConfigurationDescriptions& descriptions);

    private:
        virtual void beginJob() override;
        virtual void endJob() override;
        virtual bool filter(Event&, const EventSetup&) override;

        EDGetTokenT<vector<string>> tauTauPairFlags_;
        int nSel_;
};

TauTauFinalStateFilter::TauTauFinalStateFilter(const ParameterSet& iConfig) {
    tauTauPairFlags_ = consumes<vector<string>>(iConfig.getParameter<InputTag>("tauTauPairFlags"));
    usesResource();
}


void TauTauFinalStateFilter::fillDescriptions(ConfigurationDescriptions& descriptions) {
    //The following says we do not know what parameters are allowed so do no validation
    // Please change this to state exactly what you do use, even if it is no parameters
    ParameterSetDescription desc;
    desc.setUnknown();
    descriptions.addDefault(desc);
}


bool TauTauFinalStateFilter::filter(Event& event, const EventSetup& setup) {
    Handle<vector<string>> tauTauPairFlags;

    event.getByToken(tauTauPairFlags_, tauTauPairFlags);

    if (tauTauPairFlags->size() == 0) {
        return false;
    }
    
    if (
        (tauTauPairFlags->at(0) == "et")
        || (tauTauPairFlags->at(0) == "mt")
        || (tauTauPairFlags->at(0) == "tt")
        || (tauTauPairFlags->at(0) == "em")
        || (tauTauPairFlags->at(0) == "ee")
        || (tauTauPairFlags->at(0) == "mm")
    ) {
        return true;
    }

    return false;
}


TauTauFinalStateFilter::~TauTauFinalStateFilter() {}


void TauTauFinalStateFilter::beginJob() {}


void TauTauFinalStateFilter::endJob() {}


//define this as a plug-in
DEFINE_FWK_MODULE(TauTauFinalStateFilter);
