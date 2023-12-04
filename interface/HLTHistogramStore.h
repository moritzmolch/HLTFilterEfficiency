/**
 * Store for cutflow-like histogram for filter sequence in a HLT path.
 * 
 * \date  2023-12-04
 * \author Moritz Molch <moritz.molch@kit.edu>
 */

#ifndef HLTHistogramStore_h
#define HLTHistogramStore_h

#include <memory>
#include <string>
#include <unordered_map>
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
    void put(const string&, shared_ptr<HLTFilterPassHistogram>);

    // get an object from the store
    shared_ptr<HLTFilterPassHistogram> get(const string&);

    // check if name is already used as a key in the store
    const bool contains(const string& name);

    // iterator class for accessing elements of this store in a more convenient way
    class StoreIterator {

    public:
        StoreIterator(const unordered_map<string, shared_ptr<HLTFilterPassHistogram>>::iterator&);

        std::shared_ptr<HLTFilterPassHistogram>& operator*();
        std::shared_ptr<HLTFilterPassHistogram>* operator->();
        StoreIterator& operator++();
        bool operator==(const StoreIterator&) const;
        bool operator!=(const StoreIterator&) const;

    private:
        unordered_map<string, shared_ptr<HLTFilterPassHistogram>>::iterator iterator;
    };

    // iterator for begin of the store
    HLTHistogramStore::StoreIterator begin();

    // iterator for end of the store
    HLTHistogramStore::StoreIterator end();

private:

    // map containing the name-object pointer pairs in this store
    unordered_map<string, shared_ptr<HLTFilterPassHistogram>> contents_;

};


};


#endif