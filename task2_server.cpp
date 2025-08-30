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
    transform(result.begin(), result.end(), result.begin(), ::toupper);
    return result;
}

// Validate guess
bool isValidGuess(const string& guess) {
    if (guess.size() != 5) return false;
    for (char c : guess) {
        if (!isalpha(c)) return false;
    }
    return true;
}

int main() {
    // Configuration
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

    // Setup server socket
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);
    const int PORT = 8080;

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind(server_fd, (struct sockaddr*)&address, sizeof(address));
    listen(server_fd, 3);
    cout << "Server started on port " << PORT << "...\n";

    new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen);
    cout << "Client connected!\n";

    char buffer[1024] = {0};
    int round = 1;

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
            continue;
        }

        // Evaluate
        vector<LetterResult> results = evaluateGuess(guess, answer);

        // Win condition
        if (guess == answer) {
            string msg = "WIN " + answer;
            send(new_socket, msg.c_str(), msg.size(), 0);
            break;
        }

        // Send feedback
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