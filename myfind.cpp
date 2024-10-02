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
