#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cctype>
using namespace std;

enum LetterResult { Hit, Present, Miss };

string resultToString(LetterResult r) {
    switch (r) {
        case Hit: return "Hit";
        case Present: return "Present";
        case Miss: return "Miss";
    }
    return "";
}

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
    // Second pass: present letters
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

string toUpper(const string& s) {
    string result = s;
    result = s; // copy
    for (size_t i = 0; i < result.size(); i++) {
        result[i] = toupper(result[i]);
    }
    return result;
}

bool isValidGuess(const string& guess) {
    if (guess.size() != 5) return false;
    for (size_t i = 0; i < guess.size(); i++) {
        if (!isalpha(guess[i])) return false;
    }
    return true;
}

int main() {
    const int maxRounds = 6;
    vector<string> wordList;
    wordList.push_back("APPLE");
    wordList.push_back("HOUSE");
    wordList.push_back("TRAIN");
    wordList.push_back("CRANE");
    wordList.push_back("PLANT");
    wordList.push_back("MONEY");
    wordList.push_back("WATER");
    wordList.push_back("LIGHT");

    srand((unsigned)time(NULL));
    string answer = wordList[rand() % wordList.size()];

    cout << "=== Multi-player Wordle (2 players) ===\n";
    cout << "Both players will try to guess the same 5-letter word.\n";
    cout << "Each round, both players make one guess.\n";
    cout << "First to guess correctly wins. If both guess in the same round, it's a tie.\n\n";

    bool p1Won = false;
    bool p2Won = false;

    for (int round = 1; round <= maxRounds; round++) {
        cout << "----- Round " << round << " -----\n";

        // Player 1's turn
        string guess1;
        cout << "[Player 1] Enter your guess: ";
        cin >> guess1;
        guess1 = toUpper(guess1);
        if (!isValidGuess(guess1)) {
            cout << "Invalid guess. Try again.\n";
            round--;
            continue;
        }
        vector<LetterResult> res1 = evaluateGuess(guess1, answer);
        cout << "Feedback P1: ";
        for (int i = 0; i < 5; i++) {
            cout << guess1[i] << "(" << resultToString(res1[i]) << ") ";
        }
        cout << "\n";
        if (guess1 == answer) p1Won = true;

        // Player 2's turn
        string guess2;
        cout << "[Player 2] Enter your guess: ";
        cin >> guess2;
        guess2 = toUpper(guess2);
        if (!isValidGuess(guess2)) {
            cout << "Invalid guess. Try again.\n";
            round--;
            continue;
        }
        vector<LetterResult> res2 = evaluateGuess(guess2, answer);
        cout << "Feedback P2: ";
        for (int i = 0; i < 5; i++) {
            cout << guess2[i] << "(" << resultToString(res2[i]) << ") ";
        }
        cout << "\n";
        if (guess2 == answer) p2Won = true;

        // Check outcomes
        if (p1Won && p2Won) {
            cout << "It's a tie! Both players guessed the word: " << answer << "\n";
            return 0;
        } else if (p1Won) {
            cout << "Player 1 wins! The word was: " << answer << "\n";
            return 0;
        } else if (p2Won) {
            cout << "Player 2 wins! The word was: " << answer << "\n";
            return 0;
        }
    }

    cout << "Game over! Neither player guessed the word. The correct word was: " << answer << "\n";
    return 0;
}