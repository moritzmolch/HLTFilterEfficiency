/**
 * Cutflow-like histogram for filter sequence in a HLT path.
 * 
 * \date  2023-12-04
 * \author Moritz Molch <moritz.molch@kit.edu>
 */

#ifndef HLTFilterPassHistogram_h
#define HLTFilterPassHistogram_h

#include <map>
#include <string>
#include <vector>

#include "TH1.h"


using namespace std;
using namespace ROOT;


namespace hltTools {


class HLTFilterPassHistogram {

public:

    // default constructor
    HLTFilterPassHistogram();

    // constructor with HLT path name and list of filters
    HLTFilterPassHistogram(const string&, const vector<string>&);

    // getter method for the name
    const std::string& name();

    // set filters that have saveTags = True
    void SetFilterNamesSaveTags(const vector<string>&);

    // fill the filter cutflow histogram with one event.
    void fill(const std::string& filterNameLastRun, const bool& hasPassedFilterLastRun);

    // get a copy of the underlying TH1D object
    const TH1D getTH1D();

    // get a copy of the underlying TH1D object with selected bins
    const TH1D getTH1D(const vector<string>&, const std::string&);

    // get a copy of the underlying TH1D object with selected bins
    const TH1D getTH1DSaveTags();



private:

    // name of this histogram, which is equal to the full name of the HLT path
    string name_;

    // list of all filter names
    vector<string> filterNames_;

    // list of all filter names with saveTags = True
    vector<string> filterNamesSaveTags_;

    // map of the filter names to the bin index in the histogram
    map<string, int> filterBinIndices_;

    // the actual histogram
    TH1D hist_;

    // create the map of filters to the bin index in the internal ``ROOT::TH1`` histogram
    map<string, int> createFilterBinIndices(const vector<string>&);

    // create the internally used ``ROOT::TH1D`` histogram
    TH1D createHist(const string&, const vector<string>&);
};

};

#endif