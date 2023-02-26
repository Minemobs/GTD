#include <iostream>
#include <string>
#include <algorithm>
#include <string_view>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>
#include <filesystem>

#ifdef WIN32
#include <direct.h>
#define cwd _getcwd
#define cd _chdir
#define NUL "nul"
#define PATH_SEP "\\"
#define GRADLEW "gradlew.bat"
#else
#include "unistd.h"
#define cwd getcwd
#define cd chdir
#define NUL "/dev/null"
#define PATH_SEP "/"
#define GRADLEW "gradlew"
#endif

using std::cout, std::endl, std::cin, std::string, nlohmann::json;

const char* ws = " \t\n\r\f\v";

bool isGitIsInstalled();
bool iequals(const std::string& a, const std::string& b);
bool contains(const json &j, const string &str);
string getRepoUrl(const json &j, const string &str);
void willExecuteScripts(const string &repoPath);

void split(const string &str, const char& delimiter, std::vector<string> &out) {
    size_t start;
    size_t end = 0;
    while((start = str.find_first_not_of(delimiter, end)) != std::string::npos) {
        end = str.find(delimiter, start);
        out.push_back(str.substr(start, end - start));
    }
}

int main() {
    if(!isGitIsInstalled()) {
        std::cerr << "Git is not installed.\n";
        return 1;
    }
    cpr::Response r = cpr::Get(cpr::Url("https://raw.githubusercontent.com/Minemobs/GTD/main/gradle-template.json"));
    json j = json::parse(r.text);
    string repo;
    do {
        cout << "Write the name of the repo you want to clone" << endl;
        std::getline(cin, repo);
    } while(!contains(j, repo));
    string projectName;
    cout << "Write the new name of this repository. [Default to " << repo << "]" << endl;
    /*std::cin.clear();
    std::cin.sync();*/
    std::getline(cin, projectName);
    if(projectName.empty()) projectName = repo;
    cout << "Getting the url of the repository" << endl;
    string repoURL = getRepoUrl(j, repo);
    if(repoURL.empty()) {
        cout << "The url of the repository could not be find." << endl;
        return 2;
    }
    cout << "Cloning" << endl;
    int cmdStatus = system(string("git clone ").append(repoURL).append(" 2> gitLog.txt ").append(1, '"').append(projectName).append(1, '"').data());
    if(cmdStatus != 0) {
        cout << "An error occurred while cloning the repo." << endl;
        std::ifstream o("gitLog.txt");
        std::string logs;
        while(std::getline(o, logs)) {
            std::cout << logs << std::endl;
        }
        o.close();
        return 3;
    }
    string _cwd = std::filesystem::current_path();
    willExecuteScripts(projectName);
    cd(_cwd.c_str());
    return 0;
}

void willExecuteScripts(const string &repoPath) {
    cd(repoPath.c_str());
    std::ifstream scriptJsonReader("gtd.json");
    if(!scriptJsonReader) {
        return;
    }
    string executeScriptStr;
    do {
        cout << "Do you want to execute scripts ?" << endl << "It's generally a bad idea. [Y/N]" << endl;
        cin >> executeScriptStr;
    } while(!iequals(executeScriptStr, "y") && !iequals(executeScriptStr, "n"));
    if(iequals(executeScriptStr, "false")) return;
    json j;
    scriptJsonReader >> j;
    for(const auto &i : j["before_building"]) {
        system(i.get<string>().c_str());
    }
    system((string(".") + PATH_SEP + GRADLEW + " assemble").c_str());
    for(const auto &i : j["after_building"]) {
        system(i.get<string>().c_str());
    }
}

bool contains(const json& j, const string& str) {
    for (const auto & i : j) {
        if(iequals(i["name"], str)) return true;
    }
    return false;
}

string getRepoUrl(const json &j, const string &str) {
    for (const auto &i : j) {
        if(iequals(i["name"], str)) return i["repo_path"];
    }
    //Should never be empty because every repo contains a repoPath key.
    return "";
}

bool isGitIsInstalled() {
    return system(string("git --version > ").append(NUL).append(" 2> ").append(NUL).c_str()) == 0;
}

bool iequals(const std::string& a, const std::string& b){
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}