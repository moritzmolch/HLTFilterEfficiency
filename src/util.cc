#include <cmath>

#include "Math/LorentzVector.h"
#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/util.h"

using namespace ROOT::Math;
using namespace std;


namespace util {


const bool approxEqual(const double& value1, const double& value2) {
    return approxEqual(value1, value2, 1.0e-5);
}


const bool approxEqual(const double& value1, const double& value2, const double& maxDelta) {
    if (value1 == value2) {
        return true;
    } else {
        double delta = abs(value1 - value2);
        if ((value1 + value2) != 0) {
            delta *= (2.0 / abs(value1 + value2));
        }
        return (delta < maxDelta);
    }
}


const double getDeltaR(const LorentzVector<PxPyPzE4D<double>>& p1, const LorentzVector<PxPyPzE4D<double>>& p2) {
    return sqrt(pow(p1.eta() - p2.eta(), 2) + pow(p1.phi() - p2.phi(), 2));
}


}; // end namespace util