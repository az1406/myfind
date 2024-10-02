#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <cassert>
#include <sys/wait.h>
#include <cstring>
#include "functions.h"

using namespace std;

int main(int argc, char *argv[]) {
    int err = 0;
    int rec = 0;
    int caseIns = 0;
    string programName = argv[0];

    // Parse command-line arguments
    int opt = parseArguments(argc, argv, err, rec, caseIns);
    if (err) {
        printUsage(programName);
    }

    // If an incorrect number of arguments is entered.
    if (argc < opt + 2) {
        cerr << "False number of args" << endl;
        printUsage(programName);
    }

    string dirName = argv[opt];
    opt++;

    // Create an array of process IDs
    pid_t *pIDs = new pid_t[argc - opt];
    int i = 0;
    while (opt < argc) {
        pIDs[i++] = forkSearch(dirName, argv[opt++], rec, caseIns);
    }

    int status = 0;
    pid_t wPID;

    // Kill zombie processes.
    while ((wPID = wait(&status)) > 0) {
        if (WIFEXITED(status)) {
            cout << "Child " << wPID << " successfully exited with status " << WEXITSTATUS(status) << "." << endl;
        } else {
            cout << "Child " << wPID << " not terminated correctly." << endl;
        }
    }

    delete[] pIDs;
    exit(EXIT_SUCCESS);
}
