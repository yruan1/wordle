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

// Evaluate a guess against the answer
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
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

bool isValidGuess(const string& guess) {
    if (guess.size() != 5) return false;
    for (int i = 0; i < 5; i++) {
        if (!isalpha(guess[i])) return false;
    }
    return true;
}

int main() {
    const int PORT = 8080;
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

    int server_fd, client_fd;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 1);

    cout << "Server running. Waiting for client..." << endl;
    client_fd = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    cout << "Client connected." << endl;

    for (int round = 1; round <= maxRounds; round++) {
        char buffer[1024] = {0};
        int valread = read(client_fd, buffer, 1024);
        string guess(buffer);
        guess = toUpper(guess);

        if (!isValidGuess(guess)) {
            string msg = "Invalid\n";
            send(client_fd, msg.c_str(), msg.size(), 0);
            round--; // invalid input doesn't count
            continue;
        }

        vector<LetterResult> results = evaluateGuess(guess, answer);
        string feedback = "Result ";
        for (int i = 0; i < 5; i++) {
            feedback += guess[i];
            feedback += "(" + resultToString(results[i]) + ") ";
        }
        feedback += "\n";
        send(client_fd, feedback.c_str(), feedback.size(), 0);

        if (guess == answer) {
            string winMsg = "WIN " + answer + "\n";
            send(client_fd, winMsg.c_str(), winMsg.size(), 0);
            close(client_fd);
            close(server_fd);
            return 0;
        }
    }

    string loseMsg = "LOSE " + answer + "\n";
    send(client_fd, loseMsg.c_str(), loseMsg.size(), 0);
    close(client_fd);
    close(server_fd);

    return 0;
}