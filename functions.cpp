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

void printUsageErrors(int argc, char *argv[], int err, int opt, string programName, int rec, int caseIns){
    if (err) {  // If an error occurred during argument parsing
        printUsage(programName, 1); 
    }

    // Check if too many options (-R and -i) were given
    int optionCount = (rec ? 1 : 0) + (caseIns ? 1 : 0); // Count the options
    if (optionCount > 2) {
        printUsage(programName, 2); // Error: Too many options
    }

    // Check if no search path is provided
    if (opt >= argc) {
        printUsage(programName, 3); // No search path provided
    }

    string dirName = argv[opt]; // Get the directory name from arguments
    opt++;

    // Check if the provided search path is valid
    if (getAbsPath(dirName).empty()) {
        printUsage(programName, 4); // Invalid search path
    }

    // Check if no filenames are provided for search
    if (argc < opt + 1) {
        printUsage(programName, 5); // No filenames provided for search
    }
}

void printUsage(const string &programName, int errorCode) {
    
    switch (errorCode) {
        case 1:
            cerr << "Error: Invalid option for arguments." << endl;
            break;
        case 2:
            cerr << "Error: Too many arguments given." << endl;
            break;
        case 3:
            cerr << "Error: No search path provided!" << endl;
            break;
        case 4:
            cerr << "Error: Invalid search path." << endl;
            break;
        case 5:
            cerr << "Error: No filenames provided for search." << endl;
            break;
        default:
            cerr << "An unknown error occurred." << endl;
            break;
    }

    cerr << "Usage: " << programName << " [-R] [-i] searchpath filename 1 [filename2] ... [filename n]" << endl;

    exit(EXIT_FAILURE);
}


int parseArguments(int argc, char *argv[], int &err, int &rec, int &caseIns) {
    int c;
    
    while ((c = getopt(argc, argv, "Ri")) != -1) {
        switch (c) {
            case 'R':
                if (rec) {
                    err = 1;
                } else {
                    rec = 1;
                }
                break;
            case 'i':
                if (caseIns) {
                    err = 1;
                } else {
                    caseIns = 1;
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

string getAbsPath(const string &path) {
    char *realPath = realpath(path.c_str(), nullptr);
    string result = realPath ? string(realPath) : string();
    free(realPath);
    return result;
}

bool cmpFileNames(const string &str1, const string &str2, bool caseIns) {
    if (str1.size() != str2.size()) {
        return false;
    }

    for (size_t i = 0; i < str1.size(); ++i) {
        char c1 = caseIns ? tolower(str1[i]) : str1[i];
        char c2 = caseIns ? tolower(str2[i]) : str2[i];

        if (c1 != c2) {
            return false;
        }
    }

    return true;
}

string buildNewPath(const string &oldPath, const string &fileName) {
    return oldPath + "/" + fileName;
}

void searchFile(const string &dir, const string &toSearch, bool rec, bool caseIns, bool &fileFound) {
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
                if (rec) {
                    if (fileName != "." && fileName != "..") {
                        string newPath = buildNewPath(dir, fileName);
                        searchFile(newPath, toSearch, rec, caseIns, fileFound);
                    }
                }
                break;
            }
            case DT_REG: {
                if (cmpFileNames(fileName, toSearch, caseIns)) {
                    string combinedName = buildNewPath(dir, fileName);
                    string absPath = getAbsPath(combinedName);
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

pid_t forkSearch(const string &dir, const string &toSearch, bool rec, bool caseIns) {
    pid_t pID = fork();
    bool fileFound = false; 

    if (pID == 0) {  // Child process
        searchFile(dir, toSearch, rec, caseIns, fileFound);

        // If file not found after the search, print the error message
        if (!fileFound) {
            cerr << "Error: File '" << toSearch << "' not found." << endl;
        }

        exit(EXIT_SUCCESS);
    } else if (pID == -1) {
        cerr << "Unable to start child" << endl;
        exit(EXIT_FAILURE);
    }

    return pID;
}

void kill() {
    int status = 0;
    pid_t wPID;
    int count = 0;

    while ((wPID = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            count++;
        } else {
            cout << "Child " << wPID << " not terminated correctly." << endl;
        }
    }
    cout << count << " processes completed successfully " << endl;
}
