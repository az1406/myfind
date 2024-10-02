#include <iostream>
#include <cstdlib>
#include <sys/wait.h>
#include <dirent.h>
#include <cassert>
#include <cerrno>
#include <cstring>
#include <cctype>
#include <unistd.h>
#include <sys/types.h>
#include <memory>
#include <vector>

#include "functions.h"

using namespace std;

void printFile(pid_t pID, const string &fileName, const string &absPath) {
    cout << pID << ": " << fileName << ": " << absPath << endl;
}

void printUsageErrors(int argc, char *argv[], int err, int opt, string programName, int recursive, int case_insensitive){
    if (err) {  // If an error occurred during argument parsing
        printUsage(programName, 1); 
    }

    // Check if no search path is provided
    if (opt >= argc) {
        printUsage(programName, 2); // No search path provided
    }

    string dirName = argv[opt]; // Get the directory name from arguments
    opt++;

    // Check if the provided search path is valid
    if (getPath(dirName).empty()) {
        printUsage(programName, 3); // Invalid search path
    }

    // Check if no filenames are provided for search
    if (argc < opt + 1) {
        printUsage(programName, 4); // No filenames provided for search
    }
}

void printUsage(const string &programName, int errorCode) {
    
    switch (errorCode) {
        case 1:
            cerr << "Error: Invalid option for arguments." << endl;
            break;
        case 2:
            cerr << "Error: No search path provided!" << endl;
            break;
        case 3:
            cerr << "Error: Invalid search path." << endl;
            break;
        case 4:
            cerr << "Error: No filenames provided for search." << endl;
            break;
        default:
            cerr << "An unknown error occurred." << endl;
            break;
    }

    cerr << "Usage: " << programName << " [-R] [-i] searchpath filename 1 [filename2] ... [filename n]" << endl;

    exit(EXIT_FAILURE);
}


int parseArguments(int argc, char *argv[], int &err, int &recursive, int &case_insensitive) {
    int c;
    
    while ((c = getopt(argc, argv, "Ri")) != -1) {
        switch (c) {
            case 'R':
                if (recursive) {
                    err = 1;
                } else {
                    recursive = 1;
                }
                break;
            case 'i':
                if (case_insensitive) {
                    err = 1;
                } else {
                    case_insensitive = 1;
                }
                break;
            case '?':
                err = 1;
                break;
            default:
                assert(false);
        }
    }

    return optind; // Return the index of the first non-option argument
}

string getPath(const string &path) {
    char *realPath = realpath(path.c_str(), nullptr);
    string result = realPath ? string(realPath) : string();
    free(realPath);
    return result;
}

bool compareFiles(const string &str1, const string &str2, bool case_insensitive) {
    if (str1.size() != str2.size()) {
        return false;
    }

    size_t i = 0;
    while (i < str1.size()) {
        char c1 = case_insensitive ? tolower(str1[i]) : str1[i];
        char c2 = case_insensitive ? tolower(str2[i]) : str2[i];

        if (c1 != c2) {
            return false;
        }

        i++;
    }

    return true;
}

void searchFile(const string &dir, const string &toSearch, bool recursive, bool case_insensitive, bool &fileFound) {
    DIR *directory;
    struct dirent *entry;

    if (!(directory = opendir(dir.c_str()))) {
        perror(dir.c_str());
        return;
    }

    while ((entry = readdir(directory)) != nullptr) {
        string fileName = entry->d_name;

        switch (entry->d_type) {
            case DT_DIR: {
                if (recursive) {
                    if (fileName != "." && fileName != "..") {
                        string newPath =  (dir + "/" + fileName);
                        searchFile(newPath, toSearch, recursive, case_insensitive, fileFound);
                    }
                }
                break;
            }
            case DT_REG: {
                if (compareFiles(fileName, toSearch, case_insensitive)) {
                    string combinedName = (dir + "/" + fileName);
                    string absPath = getPath(combinedName);
                    printFile(getpid(), toSearch, absPath);
                    fileFound = true;
                }
                break;
            }
            default:
                break;
        }
    }
    while ((closedir(directory) == -1 && errno == EINTR));
}

pid_t forkSearch(const string &dir, const string &toSearch, bool recursive, bool case_insensitive) {
    pid_t pID = fork();
    bool fileFound = false; 

    if (pID == 0) {  // Child process
        searchFile(dir, toSearch, recursive, case_insensitive, fileFound);

        // If file not found after the search
        if (!fileFound) {
            cerr << "\033[31m" << getpid() << ": File '" << toSearch << "' not found.\033[0m" << endl;
        }

        exit(EXIT_SUCCESS);
    } else if (pID < 0) {
        cerr << "Failure while starting child process" << endl;
        exit(EXIT_FAILURE);
    }

    return pID;
}

void kill() {
    int status = 0, count = 0;
    pid_t wPID;

    while ((wPID = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            count++;
        } else {
            cout << "Child " << wPID << " not terminated correctly." << endl;
        }
    }
    cout << count << " processes completed successfully " << endl;
}
