#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <cctype>
#include <netinet/in.h>
#include <unistd.h>
#include <cstring>

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

bool isValidGuess(const string& guess) {
    if (guess.size() != 5) return false;
    for (char c : guess) {
        if (!isalpha(c)) return false;
    }
    return true;
}

string toUpper(const string& s) {
    string result = s;
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

int main() {
    // Word list
    vector<string> wordList = {"APPLE","HOUSE","TRAIN","CRANE","PLANT","MONEY","WATER","LIGHT"};

    srand((unsigned)time(NULL));
    string answer = wordList[rand() % wordList.size()];

    // Setup server socket
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    const int PORT = 8080;

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd, 3) < 0) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    cout << "Server is running on port " << PORT << "...\n";

    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    cout << "Client connected!\n";

    const int maxRounds = 6;
    int round = 1;
    char buffer[1024] = {0};

    while (round <= maxRounds) {
        memset(buffer, 0, sizeof(buffer));
        int valread = read(new_socket, buffer, 1024);
        if (valread <= 0) break;

        string guess(buffer);
        guess = toUpper(guess);

        // Validate
        if (!isValidGuess(guess)) {
            string msg = "INVALID";
            send(new_socket, msg.c_str(), msg.size(), 0);
            continue; // don’t count invalid
        }

        // Evaluate
        vector<LetterResult> results = evaluateGuess(guess, answer);

        // Check win
        if (guess == answer) {
            string msg = "WIN " + answer;
            send(new_socket, msg.c_str(), msg.size(), 0);
            break;
        }

        // Not win, send feedback
        string feedback = "ROUND " + to_string(round) + " ";
        for (int i = 0; i < 5; i++) {
            feedback += guess[i] + string("(") + resultToString(results[i]) + ") ";
        }
        send(new_socket, feedback.c_str(), feedback.size(), 0);

        round++;
    }

    if (round > maxRounds) {
        string msg = "LOSE " + answer;
        send(new_socket, msg.c_str(), msg.size(), 0);
    }

    close(new_socket);
    close(server_fd);
    return 0;
}
