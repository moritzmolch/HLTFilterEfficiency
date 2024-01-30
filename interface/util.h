#ifndef GUARD_UTIL_H
#define GUARD_UTIL_H

#include "Math/Vector4D.h"

using namespace ROOT::Math;


namespace util {


const bool approxEqual(const double&, const double&);


const bool approxEqual(const double&, const double&, const double&);


const double getDeltaR(const LorentzVector<PxPyPzE4D<double>>&, const LorentzVector<PxPyPzE4D<double>>&);


}; // end namespace util

#endif // end GUARD_UTIL_H