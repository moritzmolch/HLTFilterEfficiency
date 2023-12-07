/**
 * Cutflow-like histogram for filter sequence in a HLT path.
 *
 * Implementation of ``TauAnalysis/HLTFilterEfficiencyStudies/interface/HLTFilterPassHistogram.h``.
 * 
 * \date  2023-12-04
 * \author Moritz Molch <moritz.molch@kit.edu>
 */

#include <map>
#include <string>
#include <vector>

#include "TH1.h"

#include "TauAnalysis/HLTFilterEfficiencyStudies/interface/HLTFilterPassHistogram.h"


using namespace std;
using namespace ROOT;


namespace hltTools {


/**
 * Default constructor, which does not contain any initialization.
 */
HLTFilterPassHistogram::HLTFilterPassHistogram() {}


/**
 * Constructor of a filter cutflow histogram.
 * 
 * @param name full name of the HLT path 
 * @param filterNames ordered vector of filter names that the HLT path consists of
 */
HLTFilterPassHistogram::HLTFilterPassHistogram(const string& name, const vector<string>& filterNames) {
    // create trivial class members that are just copied
    name_ = std::string(name);
    filterNames_ = vector<string>();

    filterNames_.push_back("nEvents");
    for (const string& filter : filterNames) {
        filterNames_.push_back(filter);
    }
    filterNamesSaveTags_ = vector<string>();

    // create the internally used ``ROOT::TH1D`` histogram
    hist_ = createHist(name_, filterNames_);

    // create the map of filters to the bin index in the internal ``ROOT::TH1`` histogram
    filterBinIndices_ = createFilterBinIndices(filterNames_);
}


/**
 * Get the name of this histogram, which corresponds to the name of the full HLT path.
 */
const std::string& HLTFilterPassHistogram::name() {
    return name_;
}


/*
 * Set list with name of filters that have saveTags = True.
 *
 * @param filterNamesSaveTags list of filter names that have saveTags = True
 */
void HLTFilterPassHistogram::SetFilterNamesSaveTags(const vector<string>& filterNamesSaveTags) {
    filterNamesSaveTags_ = vector<string>();
    filterNamesSaveTags_.push_back("nEvents");
    for (const string& filter : filterNamesSaveTags) {
        filterNamesSaveTags_.push_back(filter);
    }
}


/**
 * Fill the filter cutflow histogram with one event.
 * 
 * The method receives the name of the last filter that has been run in the HLT path as well as information about the state of the last filter, i.e. if the event has passed the filter or not. This information is used to increase the bin count of all bins up to the last passed filter by 1.
 * 
 * @param filterNameLastRun name of the last filter that has been run in the HLT path
 * @param hasPassedFilterLastRun boolean value for the state of the last filter, which is ``true`` if the filter has been run successfully (corresponds to state ``edm::hlt::Pass``)
 */
void HLTFilterPassHistogram::fill(const string& filterNameLastRun, const bool& hasPassedFilterLastRun) {
    // get the index of the bin that corresponds to the last filter that the event has passed
    int lastBinIndex;
    if (hasPassedFilterLastRun) {
        lastBinIndex = filterBinIndices_.at(filterNameLastRun);
    } else {
        lastBinIndex = filterBinIndices_.at(filterNameLastRun) - 1;
    }

    // fill all bins up to the last passed filter
    for (int i = 1; i <= lastBinIndex; ++i) {
        hist_.AddBinContent(i);
    }
}


/**
 * Get a copy of the underlying ``ROOT::TH1D`` object.
 */
const TH1D HLTFilterPassHistogram::getTH1D() {
    return TH1D(hist_);
}


/**
 * Get a copy of the underlying ``ROOT::TH1D`` object for selected bins.
 * 
 * @param binLabels labels of bins that appear in the returned histogram
 */
const TH1D HLTFilterPassHistogram::getTH1D(const vector<string>& selectedFilterNames, const std::string& nameSuffix) {
    int nBins = static_cast<int>(selectedFilterNames.size());
    char buffer[1024];
    snprintf(buffer, 1024, "%s_%s", hist_.GetName(), nameSuffix.c_str());
    const char* name = buffer;
    TH1D histSlim = TH1D(name, hist_.GetTitle(), nBins, 0, nBins);
    for (int i = 1; i <= histSlim.GetNbinsX(); ++i) {
        int fullHistBinIndex = filterBinIndices_.at(selectedFilterNames.at(i - 1));
        double fullHistBinContent = hist_.GetBinContent(fullHistBinIndex);
        histSlim.GetXaxis()->SetBinLabel(i, selectedFilterNames.at(i - 1).c_str());
        histSlim.SetBinContent(i, fullHistBinContent);
    }
    return histSlim;
}


const TH1D HLTFilterPassHistogram::getTH1DSaveTags() {
    return getTH1D(filterNamesSaveTags_, "saveTags");
}


/**
 * Create map that associates each filter to the corresponding bin index in the internal ``ROOT::TH1D``histogram.
 * 
 * The following convention is used:
 * - Bin 1 corresponds to the total number of events
 * - Bin 2, ..., ``filterNames.size() + 1`` corresponds to the number of events that have passed the filter with name ``filterNames[i - 2]``
 * 
 * @param filterNames ordered vector of filter names that the HLT path consists of
 */
map<string, int> HLTFilterPassHistogram::createFilterBinIndices(const vector<string>& filterNames) {
    // create the map
    map<string, int> filterBinIndices = map<string, int>();

    // fill the map
    for (size_t i = 0; i < filterNames.size(); ++i) {
        filterBinIndices.insert(pair<string, int>(filterNames.at(i), static_cast<int>(i + 1)));
    }

    return filterBinIndices;
}


/**
 * Create the histogram.
 * 
 * The bin contents have to be interpreted in the following way:
 * - Bin 1 corresponds to the total number of events
 * - Bin 2, ..., ``filterNames.size() + 1`` corresponds to the number of events that have passed the filter with name ``filterNames[i - 2]``
 * 
 * @param name full name of the HLT path
 * @param filterNames ordered vector of filter names that the HLT path consists of
 */
TH1D HLTFilterPassHistogram::createHist(const std::string& name, const vector<string>& filterNames) {
    // create the histogram
    int nBins = static_cast<int>(filterNames.size());
    TH1D hist = TH1D(name.c_str(), name.c_str(), nBins, 0, nBins);

    // set the bin indices according to the filter names (or to nEvents in the case of the first bin)
    for (int i = 1; i <= hist.GetNbinsX(); ++i) {
        hist.GetXaxis()->SetBinLabel(i, filterNames.at(i - 1).c_str());
    }

    return hist;
}


};
