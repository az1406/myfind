using namespace std;

// Function to parse command-line arguments
int parseArguments(int argc, char *argv[], int &err, int &rec, int &caseIns);

// Printing
void printUsage(const string &programName);

// Prints the found file.
void printFile(pid_t pID, const string &fileName, const string &absPath);

// Returns the absolute path of a file.
string getAbsPath(const string &path);

// Two strings are compared depending on caseIns.
bool cmpFileNames(const string &str1, const string &str2, bool caseIns);

// Combines oldPath and fileName.
string buildNewPath(const string &oldPath, const string &fileName);

// Searches for a file in dir. If rec is assigned, then also searches recursively in the subfolders.
void searchFile(const string &dir, const string &toSearch, bool rec, bool caseIns);

// Searches for a file in a new process.
pid_t forkSearch(const string &dir, const string &toSearch, bool rec, bool caseIns);

// Kill zombie processes
void kill();
