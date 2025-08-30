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
    vector<string> wordList;
    wordList.push_back("APPLE");
    wordList.push_back("HOUSE");
    wordList.push_back("TRAIN");
    wordList.push_back("CRANE");
    wordList.push_back("PLANT");
    wordList.push_back("MONEY");
    wordList.push_back("WATER");
    wordList.push_back("LIGHT");

    // Randomly pick an answer
    srand((unsigned)time(NULL));
    string answer = wordList[rand() % wordList.size()];

    cout << "Welcome to Wordle (C++ edition)!\n";
    cout << "You have " << maxRounds << " attempts to guess the 5-letter word.\n\n";

    for (int round = 1; round <= maxRounds; round++) {
        string guess;
        cout << "Round " << round << "/" << maxRounds << ". Enter your 5-letter guess: ";
        cin >> guess;
        guess = toUpper(guess);

        // Check validity
        if (!isValidGuess(guess)) {
            cout << "Invalid guess. Must be exactly 5 letters A-Z.\n";
            round--; // don’t count invalid guesses as attempts
            continue;
        }

        vector<LetterResult> results = evaluateGuess(guess, answer);

        // Print feedback
        cout << "Result: ";
        for (int i = 0; i < 5; i++) {
            cout << guess[i] << "(" << resultToString(results[i]) << ") ";
        }
        cout << "\n";

        // Check win condition
        if (guess == answer) {
            cout << "Congratulations! You guessed the word: " << answer << "\n";
            return 0;
        }
    }

    cout << "Game over! The correct word was: " << answer << "\n";
    return 0;
}