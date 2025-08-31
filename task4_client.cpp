#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    const int PORT = 9090;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024];

    sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        cout << "Socket creation failed\n";
        return -1;
    }

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
        cout << "Invalid address\n";
        return -1;
    }

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed\n";
        return -1;
    }

    cout << "Connected to Multi-player Wordle server.\n";

    // 🔹 Wait for game start message
    memset(buffer, 0, sizeof(buffer));
    int valread = read(sock, buffer, sizeof(buffer)-1);
    if (valread > 0) {
        cout << buffer;
    }

    while (true) {
        string guess;
        cout << "Enter your 5-letter guess: ";
        cin >> guess;

        // Force uppercase
        for (size_t i = 0; i < guess.size(); i++) guess[i] = toupper(guess[i]);

        send(sock, guess.c_str(), guess.size(), 0);

        memset(buffer, 0, sizeof(buffer));
        valread = read(sock, buffer, sizeof(buffer)-1);
        if (valread <= 0) break;

        string response(buffer);
        cout << response;

        if (response.find("WIN") != string::npos ||
            response.find("LOSE") != string::npos ||
            response.find("TIE") != string::npos ||
            response.find("No one guessed") != string::npos) {
            break;
        }
    }

    close(sock);
    return 0;
}