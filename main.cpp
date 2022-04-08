#include <iostream>
#include <string>
#include <string_view>
#include <cpr/cpr.h>
#include <nlohmann/json.hpp>

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

using std::cout, std::endl, std::string, nlohmann::json;

bool isGitIsInstalled();

bool iequals(const std::string& a, const std::string& b);

bool contains(const json &j, const string &str);

string getRepoUrl(const json &j, const string &str);

void willExecuteScripts(const string &repoPath);

string GetCurrentWorkingDirectory() {
    const int BUFSIZE = 4096;
    char buf[BUFSIZE];
    memset(buf , 0 , BUFSIZE);
    cwd(buf , BUFSIZE - 1);
    return buf;
}

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
        std::cout << "Git is not installed." << std::endl;
        return 1;
    }
    cpr::Response r = cpr::Get(cpr::Url("https://gist.githubusercontent.com/Minemobs/c473c12c00ad6cfb4f67e6b1a56ea089/raw/f6f9de2902189b474632033c13cbf088a410c73b/gradle-template.json"));
    json j = json::parse(r.text);
    string repo;
    do {
        cout << "Write the name of the repo you want to clone" << endl;
        std::cin >> repo;
    } while(!contains(j, repo));
    cout << "Getting the url of the repository" << endl;
    string repoURL = getRepoUrl(j, repo);
    if(repoURL.empty()) {
        cout << "The url of the repository could not be find." << endl;
        return 2;
    }
    cout << "Cloning" << endl;
    int cmdStatus = system(string("git clone ").append(repoURL).append(" 2> gitLog.txt").c_str());
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
    string _cwd = GetCurrentWorkingDirectory();
    willExecuteScripts(repo);
    cd(_cwd.c_str());
    return 0;
}

bool isABoolean(const string &str) {
    return iequals(str, "true") || iequals(str, "false");
}

void willExecuteScripts(const string &repoPath) {
    cd(repoPath.c_str());
    std::ifstream scriptJsonReader("gtd.json");
    if(!scriptJsonReader) {
        return;
    }
    string executeScriptStr;
    do {
        cout << "Do you want to execute scripts ?" << endl << "It's generally a bad idea." << endl;
        std::cin >> executeScriptStr;
    } while(!isABoolean(executeScriptStr));
    if(iequals(executeScriptStr, "false")) {
        return;
    }
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
        if(iequals(i["name"], str)) return i["repoPath"];
    }
    //Should never be empty because every repo contains a repoPath key.
    return "";
}

bool isGitIsInstalled() {
    //TODO: migrate to exec(3)
    return system(string("git --version > ").append(NUL).append(" 2> ").append(NUL).c_str()) == 0;
}

bool iequals(const std::string& a, const std::string& b){
    return std::equal(a.begin(), a.end(),
                      b.begin(), b.end(),
                      [](char a, char b) {
                          return tolower(a) == tolower(b);
                      });
}