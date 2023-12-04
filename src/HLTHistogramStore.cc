/**
 * Store for cutflow-like histogram for filter sequence in a HLT path.
 *
 * Implementation of ``TauAnalysis/HLTFilterEfficiency/interface/HLTHistogramStore.h``.
 * 
 * \date  2023-12-04
 * \author Moritz Molch <moritz.molch@kit.edu>
 */

#include <map>
#include <stdexcept>
#include <string>
#include <vector>

#include "TH1.h"

#include "TauAnalysis/HLTFilterEfficiency/interface/HLTFilterPassHistogram.h"
#include "TauAnalysis/HLTFilterEfficiency/interface/HLTHistogramStore.h"


using namespace std;
using namespace ROOT;


namespace hltTools {


/**
 * Default constructor.
 */
HLTHistogramStore::HLTHistogramStore() {
    map<string, HLTFilterPassHistogram> contents_ = map<string, HLTFilterPassHistogram>();
}


/**
 * Add a new ``hltTools::HLTFilterPassHistogram`` to the store.
 * 
 * The object can only be added if the object does not exist yet. Otherwise, an exception is thrown.
 * 
 * @param name identifier of the stored object
 * @param hist the histogram that is added to the store
 */
HLTFilterPassHistogram& HLTHistogramStore::put(const string& name, HLTFilterPassHistogram& hist) {
    // check if the histogram is already part of the store
    if (contains(name)) {
        throw invalid_argument("object with same name is already part of the store");
    }

    // put the histogram into the store
    contents_[name] = hist;

    // also return a reference to the object
    return hist;
}


/**
 * Get an ``hltTools::HLTFilterPassHistogram`` to the store.
 * 
 * If an object with the given name does not exist, an exception is thrown.
 * 
 * @param name identifier of the stored object
 */
HLTFilterPassHistogram& HLTHistogramStore::get(const std::string& name) {
    // check if the histogram is part of the store
    if (! contains(name)) {
        throw out_of_range("object with name is not part of the store");
    }

    // get the object by the given identifier
    return contents_[name];
}



/**
 * Add a new ``hltTools::HLTFilterPassHistogram`` to the store.
 * 
 * The object can only be added if the object does not exist yet. Otherwise, an exception is thrown.
 * 
 * @param name identifier of the stored object
 */
const bool HLTHistogramStore::contains(const string& name) const {
    // iterate through the map of store contents
    for (const pair<string, HLTFilterPassHistogram>& item : contents_) {
        if (name == item.first) {
            // if the given name matches one key of the store, the element already exists
            return true;
        }
    }

    // if no match has been found, the element with the given name is not part of the store
    return false;
}


};
