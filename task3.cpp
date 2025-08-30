#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <map>
#include <sstream>

using namespace std;

// Enum for feedback
enum LetterResult { Hit, Present, Miss };

string resultToString(LetterResult r) {
    switch (r) {
        case Hit: return "Hit";
        case Present: return "Present";
        case Miss: return "Miss";
    }
    return "";
}

// Evaluate guess against an "answer"
vector<LetterResult> evaluateGuess(const string& guess, const string& answer) {
    vector<LetterResult> results(5, Miss);
    vector<bool> used(5,false);

    // Hits
    for (int i=0;i<5;i++) {
        if (guess[i]==answer[i]) {
            results[i]=Hit;
            used[i]=true;
        }
    }
    // Presents
    for (int i=0;i<5;i++) {
        if (results[i]==Hit) continue;
        for (int j=0;j<5;j++) {
            if (!used[j] && guess[i]==answer[j]) {
                results[i]=Present;
                used[j]=true;
                break;
            }
        }
    }
    return results;
}

// Uppercase
string toUpper(const string& s) {
    string r=s;
    for (size_t i=0;i<r.size();i++) r[i]=toupper(r[i]);
    return r;
}

// Validate
bool isValidGuess(const string& g) {
    if (g.size()!=5) return false;
    for (size_t i=0;i<g.size();i++) if (!isalpha(g[i])) return false;
    return true;
}

// Score = (hits, presents)
pair<int,int> scoreResult(const vector<LetterResult>& res) {
    int h=0,p=0;
    for (size_t i=0;i<res.size();i++) {
        if (res[i]==Hit) h++;
        else if (res[i]==Present) p++;
    }
    return make_pair(h,p);
}

// Compare scores: more hits better, then more presents better
bool betterScore(const pair<int,int>& a, const pair<int,int>& b) {
    if (a.first!=b.first) return a.first>b.first;
    return a.second>b.second;
}

// Helper: int -> string (for C++98)
string intToString(int x) {
    stringstream ss; ss<<x; return ss.str();
}

int main() {
    // Word list (dictionary)
    vector<string> wordList;
    wordList.push_back("APPLE");
    wordList.push_back("HOUSE");
    wordList.push_back("TRAIN");
    wordList.push_back("CRANE");
    wordList.push_back("PLANT");
    wordList.push_back("MONEY");
    wordList.push_back("WATER");
    wordList.push_back("LIGHT");

    vector<string> candidates=wordList;

    const int maxRounds=6;
    cout<<"Welcome to Wordle (Cheating Host edition)!\n";
    cout<<"You have "<<maxRounds<<" attempts.\n\n";

    for (int round=1; round<=maxRounds; round++) {
        string guess;
        cout<<"Round "<<round<<"/"<<maxRounds<<". Enter guess: ";
        cin>>guess;
        guess=toUpper(guess);

        if (!isValidGuess(guess)) {
            cout<<"Invalid guess. Must be 5 letters A-Z.\n";
            round--;
            continue;
        }

        // Group candidates by feedback
        map< vector<LetterResult>, vector<string> > groups;
        for (size_t i=0;i<candidates.size();i++) {
            vector<LetterResult> res=evaluateGuess(guess,candidates[i]);
            groups[res].push_back(candidates[i]);
        }

        // Select group with "lowest score"
        vector<LetterResult> chosenRes;
        vector<string> chosenGroup;
        bool first=true;
        pair<int,int> bestScore=make_pair(999,999);

        map< vector<LetterResult>, vector<string> >::iterator it;
        for (it=groups.begin(); it!=groups.end(); ++it) {
            pair<int,int> sc=scoreResult(it->first);
            if (first || betterScore(bestScore, sc)) {
                chosenRes=it->first;
                chosenGroup=it->second;
                bestScore=sc;
                first=false;
            }
        }

        candidates=chosenGroup;

        // Print feedback
        cout<<"Result: ";
        for (int i=0;i<5;i++) {
            cout<<guess[i]<<"("<<resultToString(chosenRes[i])<<") ";
        }
        cout<<"\n";

        // Debug: show remaining candidates
        cout<<"Remaining candidates: ";
        for (size_t i=0;i<candidates.size();i++) {
            cout<<candidates[i]<<" ";
        }
        cout<<"\n";

        // Win condition
        if (candidates.size()==1 && guess==candidates[0]) {
            cout<<"Congratulations! You guessed the word: "<<candidates[0]<<"\n";
            return 0;
        }
    }

    if (!candidates.empty()) {
        cout<<"Game over! The word was: "<<candidates[0]<<"\n";
    } else {
        cout<<"Game over! No candidates left.\n";
    }
    return 0;
}