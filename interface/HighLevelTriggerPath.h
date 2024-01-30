#ifndef GUARD_HIGHLEVELTRIGGERPATH_H
#define GUARD_HIGHLEVELTRIGGERPATH_H

#include <regex>
#include <string>
#include <vector>

using namespace std;

class HighLevelTriggerPath {

public:
    HighLevelTriggerPath();
    HighLevelTriggerPath(const string&, const int&, const vector<string>&, const vector<string>&);
    const string& fullName();
    const string& name();
    const string& version();
    const int& index();
    const vector<string>& modules();
    const vector<string>& modulesSaveTags();
    const bool isInModulesSaveTags(const string&);
    const int moduleIndex(const string&);

private:
    const pair<string, string> splitName(const string&);

    string fullName_;
    string name_;
    string version_;
    int index_;
    vector<string> modules_;
    vector<string> modulesSaveTags_;

    const regex nameRegex_ = regex("^(\\.+)_(v\\d+)$");
};

#endif // GUARD_HIGHLEVELTRIGGERPATH_H