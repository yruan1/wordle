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

enum LetterResult { Hit, Present, Miss };

string resultToString(LetterResult r) {
    if (r == Hit) return "O";     // correct position
    if (r == Present) return "?"; // wrong position but present
    return "_";                   // not present
}

vector<LetterResult> evaluateGuess(const string& guess, const string& answer) {
    vector<LetterResult> results(5, Miss);
    vector<bool> used(5, false);

    for (int i = 0; i < 5; i++) {
        if (guess[i] == answer[i]) {
            results[i] = Hit;
            used[i] = true;
        }
    }
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
    string res = s;
    for (size_t i = 0; i < res.size(); i++) res[i] = toupper(res[i]);
    return res;
}

bool isValidGuess(const string& guess) {
    if (guess.size() != 5) return false;
    for (size_t i = 0; i < guess.size(); i++) {
        if (!isalpha(guess[i])) return false;
    }
    return true;
}

int main() {
    const int PORT = 9090;
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

    int server_fd, client1, client2;
    struct sockaddr_in address;
    int opt = 1;
    socklen_t addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 2);

    cout << "Waiting for 2 players..." << endl;
    client1 = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    cout << "Player 1 connected." << endl;
    client2 = accept(server_fd, (struct sockaddr*)&address, &addrlen);
    cout << "Player 2 connected." << endl;

    // 🔹 Notify clients that game starts
    string startMsg = "Both players connected! Game starts now.\n";
    send(client1, startMsg.c_str(), startMsg.size(), 0);
    send(client2, startMsg.c_str(), startMsg.size(), 0);

    bool gameOver = false;

    for (int round = 1; round <= maxRounds && !gameOver; round++) {
        char buf1[100]; memset(buf1, 0, sizeof(buf1));
        char buf2[100]; memset(buf2, 0, sizeof(buf2));

        int r1 = read(client1, buf1, sizeof(buf1) - 1);
        int r2 = read(client2, buf2, sizeof(buf2) - 1);

        if (r1 <= 0 || r2 <= 0) { 
            cout << "A player disconnected.\n"; 
            break; 
        }

        string guess1(buf1);
        string guess2(buf2);
        guess1 = toUpper(guess1);
        guess2 = toUpper(guess2);

        if (!isValidGuess(guess1)) guess1 = "-----";
        if (!isValidGuess(guess2)) guess2 = "-----";

        vector<LetterResult> res1 = evaluateGuess(guess1, answer);
        vector<LetterResult> res2 = evaluateGuess(guess2, answer);

        // 🔹 Add round indicator
        string header = string("--- Round ") + to_string(round) + "/" + to_string(maxRounds) + " ---\n";

        string fb1 = "P1 " + guess1 + " ";
        for (int i = 0; i < 5; i++) fb1 += resultToString(res1[i]);
        fb1 += "\n";

        string fb2 = "P2 " + guess2 + " ";
        for (int i = 0; i < 5; i++) fb2 += resultToString(res2[i]);
        fb2 += "\n";

        string combined = header + fb1 + fb2;
        send(client1, combined.c_str(), combined.size(), 0);
        send(client2, combined.c_str(), combined.size(), 0);

        bool win1 = (guess1 == answer);
        bool win2 = (guess2 == answer);

        if (win1 && win2) {
            string msg = "TIE! The word was " + answer + "\n";
            send(client1, msg.c_str(), msg.size(), 0);
            send(client2, msg.c_str(), msg.size(), 0);
            gameOver = true;
        } else if (win1) {
            string msg1 = "You WIN! The word was " + answer + "\n";
            string msg2 = "You LOSE! The word was " + answer + "\n";
            send(client1, msg1.c_str(), msg1.size(), 0);
            send(client2, msg2.c_str(), msg2.size(), 0);
            gameOver = true;
        } else if (win2) {
            string msg1 = "You LOSE! The word was " + answer + "\n";
            string msg2 = "You WIN! The word was " + answer + "\n";
            send(client1, msg1.c_str(), msg1.size(), 0);
            send(client2, msg2.c_str(), msg2.size(), 0);
            gameOver = true;
        }
    }

    if (!gameOver) {
        string msg = "No one guessed it! The word was " + answer + "\n";
        send(client1, msg.c_str(), msg.size(), 0);
        send(client2, msg.c_str(), msg.size(), 0);
    }

    close(client1);
    close(client2);
    close(server_fd);
    return 0;
}