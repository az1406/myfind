#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <cassert>
#include <sys/wait.h>
#include <cstring>
#include "functions.h"

using namespace std;

int main(int argc, char *argv[]) {
    int err = 0, rec = 0, caseIns = 0;
    string programName = argv[0];

    // Parse command-line arguments
    int opt = parseArguments(argc, argv, err, rec, caseIns);

    printUsageErrors(argc, argv, err, opt, programName, rec, caseIns);
        
    string dirName = argv[opt]; // Get the directory name from arguments
    opt++;

    // Create an array of process IDs
    pid_t *pIDs = new pid_t[argc - opt];
    int i = 0;

    while (opt < argc) {
        pIDs[i++] = forkSearch(dirName, argv[opt++], rec, caseIns);
    }

    // Wait for child processes to complete and handle zombies
    kill();

    delete[] pIDs; // Clean up dynamically allocated array
    exit(EXIT_SUCCESS); // Exit the program successfully
}
