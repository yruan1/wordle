#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <cstring>
#include <netinet/in.h>
#include <unistd.h>

using namespace std;

// Enum for feedback (old-style)
enum LetterResult { Hit, Present, Miss };

// Function to convert LetterResult to string
string resultToString(LetterResult r) {
    switch (r) {
        case Hit: return "Hit";
        case Present: return "Present";
        case Miss: return "Miss";
    }
    return "";
}

// Evaluate a guess against the answer
vector<LetterResult> evaluateGuess(const string& guess, const string& answer) {
    vector<LetterResult> results(5, Miss);
    vector<bool> used(5, false);

    // First pass: check for exact matches (Hit)
    for (int i = 0; i < 5; i++) {
        if (guess[i] == answer[i]) {
            results[i] = Hit;
            used[i] = true;
        }
    }

    // Second pass: check for Present letters
    for (int i = 0; i < 5; i++) {
        if (results[i] == Hit) continue;
        for (int j = 0; j < 5; j++) {
            if (!used[j] && guess[i] == answer[j]) {
                results[i] = Present;
                used[j] = true; // fix: mark as used
                break;
            }
        }
    }
    return results;
}

// Convert string to uppercase for consistency
string toUpper(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Validate guess: must be 5 letters only
bool isValidGuess(const string& guess) {
    if (guess.size() != 5) return false;
    for (char c : guess) {
        if (!isalpha(c)) return false; // reject numbers or symbols
    }
    return true;
}

int main() {
    // Configuration
    const int maxRounds = 6;  // configurable
    vector<string> wordList = {"APPLE","HOUSE","TRAIN","CRANE","PLANT","MONEY","WATER","LIGHT"};

    srand((unsigned)time(NULL));
    string answer = wordList[rand() % wordList.size()];

    // Setup server socket
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    const int PORT = 8080;
}