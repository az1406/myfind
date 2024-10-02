using namespace std;

// Function to parse command-line arguments
int parseArguments(int argc, char *argv[], int &err, int &rec, int &caseIns);

// Gibt die gefundene Datei aus.
void printFile(pid_t pID, const string &fileName, const string &absPath);

// Gibt den absoluten Pfad einer Datei aus.
string getAbsPath(const string &path);

// Zwei Strings werden verglichen abhaengig von caseIns.
bool cmpFileNames(const string &str1, const string &str2, bool caseIns);

// oldPath und fileName werden kombiniert.
string buildNewPath(const string &oldPath, const string &fileName);

// Es wird nach einer Datei in dir gesucht. Wenn rec zugewiesen ist, dann recursiv bei den unteren Ordnern auch suchen.
void searchFile(const string &dir, const string &toSearch, bool rec, bool caseIns);

// In einem neuen Prozess einer Datei suchen.
pid_t forkSearch(const string &dir, const string &toSearch, bool rec, bool caseIns);
