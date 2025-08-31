#include <iostream>
#include <list>
#include <string>
#include <vector>
#include <ctime>
#include <cstdlib>
#include <algorithm>
using namespace std;

// 判斷猜測結果
vector<string> scoreGuess(const string& guess, const string& answer) {
    vector<string> result(5, "Miss");
    vector<bool> used(5, false);

    // 先處理 Hit
    for (int i = 0; i < 5; i++) {
        if (guess[i] == answer[i]) {
            result[i] = "Hit";
            used[i] = true;
        }
    }

    // 再處理 Present
    for (int i = 0; i < 5; i++) {
        if (result[i] == "Hit") continue;
        for (int j = 0; j < 5; j++) {
            if (!used[j] && guess[i] == answer[j]) {
                result[i] = "Present";
                used[j] = true;
                break;
            }
        }
    }

    return result;
}

int main() {
    // 遊戲設定
    int maxRounds = 6;
    list<string> words = {
        "HELLO", "WORLD", "QUITE", "FANCY", "FRESH", 
        "PANIC", "CRAZY", "BUGGY", "SCARE"
    };

    // 隨機選答案
    srand(time(0));
    int idx = rand() % words.size();
    auto it = words.begin();
    advance(it, idx);
    string answer = *it;

    cout << "=== Wordle Clone ===" << endl;
    cout << "Guess the 5-letter word! You have " << maxRounds << " rounds." << endl;

    // 遊戲循環
    for (int round = 1; round <= maxRounds; round++) {
        string guess;
        cout << "Round " << round << "/" << maxRounds << " - Enter your guess: ";
        cin >> guess;

        // 轉大寫
        transform(guess.begin(), guess.end(), guess.begin(), ::toupper);

        if (guess.size() != 5) {
            cout << "Invalid guess! Must be 5 letters." << endl;
            round--; // 不算回合
            continue;
        }

        // 判斷
        auto result = scoreGuess(guess, answer);

        // 印結果
        cout << "Result: ";
        for (int i = 0; i < 5; i++) {
            cout << guess[i] << "(" << result[i] << ") ";
        }
        cout << endl;

        // 判斷勝利
        if (guess == answer) {
            cout << "🎉 You win! The answer was " << answer << endl;
            return 0;
        }
    }

    cout << "💀 You lose! The answer was " << answer << endl;
    return 0;
}