/**
 * Store for cutflow-like histogram for filter sequence in a HLT path.
 * 
 * \date  2023-12-04
 * \author Moritz Molch <moritz.molch@kit.edu>
 */

#ifndef HLTHistogramStore_h
#define HLTHistogramStore_h

#include <map>
#include <string>
#include <vector>

#include "TH1.h"


using namespace std;
using namespace ROOT;


namespace hltTools {


class HLTHistogramStore {

public:

    // default constructor
    HLTHistogramStore();

    // add a new object to the store
    HLTFilterPassHistogram& put(const string&, HLTFilterPassHistogram&);

    // get an object from the store
    HLTFilterPassHistogram& get(const string& name);

    // check if name is already used as a key in the store
    const bool contains(const string& name) const;


private:

    // map containing the name-object pairs in this store
    map<string, HLTFilterPassHistogram> contents_;

};

};

#endif