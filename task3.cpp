#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cctype>
using namespace std;

enum LetterResult { Hit, Present, Miss };

struct Feedback {
    vector<LetterResult> results;
    int hits;
    int presents;
};

// Evaluate feedback for guess vs answer
Feedback evaluateGuess(const string& guess, const string& answer) {
    Feedback fb;
    fb.results.assign(5, Miss);
    fb.hits = 0;
    fb.presents = 0;
    vector<bool> used(5, false);

    for (int i = 0; i < 5; i++) {
        if (guess[i] == answer[i]) {
            fb.results[i] = Hit;
            fb.hits++;
            used[i] = true;
        }
    }
    for (int i = 0; i < 5; i++) {
        if (fb.results[i] == Hit) continue;
        for (int j = 0; j < 5; j++) {
            if (!used[j] && guess[i] == answer[j]) {
                fb.results[i] = Present;
                fb.presents++;
                used[j] = true;
                break;
            }
        }
    }
    return fb;
}

string resultToString(LetterResult r) {
    if (r == Hit) return "O";
    if (r == Present) return "?";
    return "_";
}

// Check if candidate word is consistent with all past rounds
bool consistentWithHistory(const string& candidate,
                           const vector<pair<string, Feedback> >& history) {
    for (size_t i = 0; i < history.size(); i++) {
        Feedback fb = evaluateGuess(history[i].first, candidate);
        if (fb.results != history[i].second.results) return false;
    }
    return true;
}

// Choose lowest-score feedback for this guess
Feedback chooseFeedback(const string& guess,
                        const vector<string>& candidates,
                        const vector<pair<string, Feedback> >& history) {
    bool first = true;
    Feedback best;
    for (size_t i = 0; i < candidates.size(); i++) {
        Feedback fb = evaluateGuess(guess, candidates[i]);
        if (!consistentWithHistory(candidates[i], history)) continue;

        if (first) { best = fb; first = false; }
        else {
            if (fb.hits < best.hits ||
               (fb.hits == best.hits && fb.presents < best.presents)) {
                best = fb;
            }
        }
    }
    return best;
}

int main() {
    vector<string> wordList;
    wordList.push_back("HELLO");
    wordList.push_back("WORLD");
    wordList.push_back("QUITE");
    wordList.push_back("FANCY");
    wordList.push_back("FRESH");
    wordList.push_back("PANIC");
    wordList.push_back("CRAZY");
    wordList.push_back("BUGGY");
    wordList.push_back("SCARE");

    vector<string> candidates = wordList;
    vector<pair<string, Feedback> > history;

    const int maxRounds = 6;
    cout << "Welcome to Cheating Wordle!\n";
    for (int round = 1; round <= maxRounds; round++) {
        string guess;
        cout << "Round " << round << ": Enter guess: ";
        cin >> guess;
        for (size_t i = 0; i < guess.size(); i++) guess[i] = toupper(guess[i]);

        // If only one candidate left, act like normal Wordle
        Feedback fb;
        if (candidates.size() == 1) {
            fb = evaluateGuess(guess, candidates[0]);
        } else {
            fb = chooseFeedback(guess, candidates, history);
        }

        // Filter candidates to those consistent with new feedback
        vector<string> newList;
        for (size_t i = 0; i < candidates.size(); i++) {
            if (consistentWithHistory(candidates[i],
                    history)) {
                Feedback candFb = evaluateGuess(guess, candidates[i]);
                if (candFb.results == fb.results) {
                    newList.push_back(candidates[i]);
                }
            }
        }
        candidates = newList;
        history.push_back(make_pair(guess, fb));

        // Print feedback
        for (int i = 0; i < 5; i++) {
            cout << resultToString(fb.results[i]);
        }
        cout << endl;

        if (fb.hits == 5) {
            cout << "You win! Answer: " << guess << endl;
            return 0;
        }
    }
    // Game over
    if (!candidates.empty()) {
        cout << "Game over! Answer was: " << candidates[0] << endl;
    } else {
        cout << "Game over! (no candidate left)" << endl;
    }
    return 0;
}