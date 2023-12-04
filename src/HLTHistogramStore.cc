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
    unordered_map<string, shared_ptr<HLTFilterPassHistogram>> contents_ = unordered_map<string, shared_ptr<HLTFilterPassHistogram>>();
}


/**
 * Add a new ``hltTools::HLTFilterPassHistogram`` to the store.
 * 
 * The object can only be added if the object does not exist yet. Otherwise, an exception is thrown.
 * 
 * @param name identifier of the stored object
 * @param hist the histogram that is added to the store
 */
void HLTHistogramStore::put(const string& name, shared_ptr<HLTFilterPassHistogram> hist) {
    // check if the histogram is already part of the store
    if (contains(name)) {
        throw invalid_argument("object with given name is already part of the store");
    }

    // put the histogram into the store
    contents_[name] = hist;
}


/**
 * Get an ``hltTools::HLTFilterPassHistogram`` to the store.
 * 
 * If an object with the given name does not exist, an exception is thrown.
 * 
 * @param name identifier of the stored object
 */
shared_ptr<HLTFilterPassHistogram> HLTHistogramStore::get(const string& name) {
    // check if the histogram is part of the store
    if (! contains(name)) {
        throw out_of_range("object with given name is not part of the store");
    }

    // get the object pointer by the given identifier and return the moved pointer
    return contents_[name];
}


/**
 * Add a new ``hltTools::HLTFilterPassHistogram`` to the store.
 * 
 * The object can only be added if the object does not exist yet. Otherwise, an exception is thrown.
 * 
 * @param name identifier of the stored object
 */
const bool HLTHistogramStore::contains(const string& name) {
    // iterate through the map of store contents
    unordered_map<string, shared_ptr<HLTFilterPassHistogram>>::iterator it = contents_.find(name);
    if (it != contents_.end()) {
        // if the key has been found, the object is part of the store
        return true;
    }

    // if no match has been found, the element with the given name is not part of the store
    return false;
}


/**
 * Returns iterator for the begin of an iteration.
 */
HLTHistogramStore::StoreIterator HLTHistogramStore::begin() {
    return HLTHistogramStore::StoreIterator(contents_.begin());
}


/**
 * Returns iterator for the end of an iteration.
 */
HLTHistogramStore::StoreIterator HLTHistogramStore::end() {
    return HLTHistogramStore::StoreIterator(contents_.end());
}


/**
 * Implementation of the StoreIterator methods
 */


HLTHistogramStore::StoreIterator::StoreIterator(const unordered_map<string, shared_ptr<HLTFilterPassHistogram>>::iterator& iter) : iterator(iter) {

}


std::shared_ptr<HLTFilterPassHistogram>& HLTHistogramStore::StoreIterator::operator*() {
    return iterator->second;
}


std::shared_ptr<HLTFilterPassHistogram>* HLTHistogramStore::StoreIterator::operator->() {
    return &iterator->second;
}


HLTHistogramStore::StoreIterator& HLTHistogramStore::StoreIterator::operator++() {
    ++iterator;
    return *this;   
}


bool HLTHistogramStore::StoreIterator::operator==(const HLTHistogramStore::StoreIterator& other) const {
    return iterator == other.iterator;
}


bool HLTHistogramStore::StoreIterator::operator!=(const HLTHistogramStore::StoreIterator& other) const {
    return iterator != other.iterator;
}


};
