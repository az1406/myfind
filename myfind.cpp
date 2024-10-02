#include <iostream>
#include <getopt.h>
#include <cstdlib>
#include <cassert>
#include <sys/wait.h>
#include <cstring>
#include "functions.h"

using namespace std;

int main(int argc, char *argv[]) {
    int err = 0, recursive = 0, case_insensitive = 0;
    string programName = argv[0];

    int opt = parseArguments(argc, argv, err, recursive, case_insensitive);
    printUsageErrors(argc, argv, err, opt, programName, recursive, case_insensitive);
        
    string dirName = argv[opt]; // Get the directory name from arguments
    opt++;

    pid_t *pIDs = new pid_t[argc - opt];    // Create an array of process IDs
    int i = 0;
    while (opt < argc) {
        pIDs[i++] = forkSearch(dirName, argv[opt++], recursive, case_insensitive);
    }

    kill();     // Wait for child processes to complete and handle zombies
    delete[] pIDs; // Clean up dynamically allocated array
    exit(EXIT_SUCCESS);
}
