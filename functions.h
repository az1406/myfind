// Function to parse command-line arguments
int parseArguments(int argc, char *argv[], int &err, int &rec, int &caseIns);

// Print usage information
void printUsage(const string &programName);

// Outputs the found file.
void printFile(pid_t pID, const string &fileName, const string &absPath);

// Outputs the absolute path of a file.
string getAbsPath(const string &path);

// Compares two strings based on caseIns.
bool cmpFileNames(const string &str1, const string &str2, bool caseIns);

// Combines oldPath and fileName.
string buildNewPath(const string &oldPath, const string &fileName);

// Searches for a file in dir. If rec is assigned, also searches recursively in the lower folders.
void searchFile(const string &dir, const string &toSearch, bool rec, bool caseIns);

// Search for a file in a new process.
pid_t forkSearch(const string &dir, const string &toSearch, bool rec, bool caseIns);
