#include <regex>
#include <string>
#include <vector>

#include "TauAnalysis/TauTriggerNtuples/interface/HighLevelTriggerPath.h"

using namespace std;

HighLevelTriggerPath::HighLevelTriggerPath() {
    fullName_ = "";
    name_ = "";
    version_ = "";
    index_ = -1;
    modules_ = vector<string>();
    modulesSaveTags_ = vector<string>();
}

HighLevelTriggerPath::HighLevelTriggerPath(const string& fullName, const int& index, const vector<string>& modules, const vector<string>& modulesSaveTags) {
    fullName_ = fullName;
    index_ = index;
    modules_ = modules;
    modulesSaveTags_ = modulesSaveTags;
    const pair<string, string> nameSplit = splitName(fullName);
    name_ = nameSplit.first;
    version_ = nameSplit.second;
}

const string& HighLevelTriggerPath::fullName() {
    return fullName_;
}

const string& HighLevelTriggerPath::name() {
    return name_;
}

const string& HighLevelTriggerPath::version() {
    return version_;
}

const int& HighLevelTriggerPath::index() {
    return index_;
}

const vector<string>& HighLevelTriggerPath::modules() {
    return modules_;
}

const vector<string>& HighLevelTriggerPath::modulesSaveTags() {
    return modulesSaveTags_;
}

const bool HighLevelTriggerPath::isInModulesSaveTags(const string& module) {
    return find(modulesSaveTags_.begin(), modulesSaveTags_.end(), module) != modulesSaveTags_.end();
}

const int HighLevelTriggerPath::moduleIndex(const string& module) {
    for (size_t i = 0; i < modules_.size(); ++i) {
        if (module == modules_.at(i)) {
            return i;
        }
    }
    return -1;
}

const pair<string, string> HighLevelTriggerPath::splitName(const string& fullName) {
    string name = "";
    string version = "";
    smatch nameMatch;
    if (regex_match(fullName, nameMatch, nameRegex_)) {
        if (nameMatch.size() != 3) {
            throw out_of_range("high level trigger path has not the expected format");
        }
        name = nameMatch[1].str();
        version = nameMatch[2].str();
    }
    return pair<string, string>({name, version});
}