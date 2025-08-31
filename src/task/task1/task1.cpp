#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cctype> // for isalpha

using namespace std;

// Enum for feedback
enum LetterResult { Hit, Present, Miss };

// Convert feedback to string
string resultToString(LetterResult r) {
    switch (r) {
        case Hit: return "Hit";
        case Present: return "Present";
        case Miss: return "Miss";
    }
    return "";
}

// Evaluate guess against answer
vector<LetterResult> evaluateGuess(const string& guess, const string& answer) {
    vector<LetterResult> results(5, Miss);
    vector<bool> used(5, false);

    // First pass: exact matches
    for (int i = 0; i < 5; i++) {
        if (guess[i] == answer[i]) {
            results[i] = Hit;
            used[i] = true;
        }
    }

    // Second pass: present matches
    for (int i = 0; i < 5; i++) {
        if (results[i] == Hit) continue;
        for (int j = 0; j < 5; j++) {
            if (!used[j] && guess[i] == answer[j]) {
                results[i] = Present;
                used[j] = true;
                break;
            }
        }
    }
    return results;
}

// Convert string to uppercase
string toUpper(const string& s) {
    string result = s;
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = toupper(result[i]);
    }
    return result;
}

// Check if guess is only letters
bool isAlphaOnly(const string& guess) {
    if (guess.size() != 5) return false;
    for (size_t i = 0; i < guess.size(); i++) {
        if (!isalpha(guess[i])) return false;
    }
    return true;
}

// Check if guess is in dictionary
bool isInDictionary(const string& guess, const vector<string>& wordList) {
    for (size_t i = 0; i < wordList.size(); i++) {
        if (guess == wordList[i]) return true;
    }
    return false;
}

int main() {
    const int maxRounds = 6;  

    // Word list also acts as dictionary
    vector<string> wordList;
    wordList.push_back("APPLE");
    wordList.push_back("HOUSE");
    wordList.push_back("TRAIN");
    wordList.push_back("CRANE");
    wordList.push_back("PLANT");
    wordList.push_back("MONEY");
    wordList.push_back("WATER");
    wordList.push_back("LIGHT");

    // Pick random answer
    srand((unsigned)time(NULL));
    string answer = wordList[rand() % wordList.size()];

    cout << "Welcome to Wordle (C++ edition)!\n";
    cout << "You have " << maxRounds << " attempts to guess the 5-letter word.\n\n";

    for (int round = 1; round <= maxRounds; round++) {
        string guess;
        cout << "Round " << round << "/" << maxRounds << ". Enter your 5-letter guess: ";
        cin >> guess;
        guess = toUpper(guess);

        // 🔹 Check validity (letters + dictionary)
        if (!isAlphaOnly(guess)) {
            cout << "Invalid guess. Must be exactly 5 letters A-Z.\n";
            round--; 
            continue;
        }
        if (!isInDictionary(guess, wordList)) {
            cout << "Not in word list. Try another word.\n";
            round--;
            continue;
        }

        vector<LetterResult> results = evaluateGuess(guess, answer);

        // Print feedback
        cout << "Result: ";
        for (int i = 0; i < 5; i++) {
            cout << guess[i] << "(" << resultToString(results[i]) << ") ";
        }
        cout << "\n";

        // Win check
        if (guess == answer) {
            cout << "Congratulations! You guessed the word: " << answer << "\n";
            return 0;
        }
    }

    cout << "Game over! The correct word was: " << answer << "\n";
    return 0;
}