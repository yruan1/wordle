#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    int sock = 0;
    struct sockaddr_in serv_addr;
    const int PORT = 8080;
    char buffer[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);
    connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr));

    cout << "Connected to Wordle server!\n";

    while (true) {
        string guess;
        cout << "Enter your 5-letter guess: ";
        cin >> guess;

        send(sock, guess.c_str(), guess.size(), 0);

        memset(buffer, 0, sizeof(buffer));
        int valread = read(sock, buffer, 1024);
        if (valread <= 0) break;

        string response(buffer);

        if (response.find("INVALID") != string::npos) {
            cout << "Invalid guess. Must be exactly 5 letters A-Z.\n";
            continue;
        }
        if (response.find("WIN") != string::npos) {
            cout << "🎉 You win! The word was: " << response.substr(4) << "\n";
            break;
        }
        if (response.find("LOSE") != string::npos) {
            cout << "💀 Game over! The word was: " << response.substr(5) << "\n";
            break;
        }

        cout << "Feedback: " << response << "\n";
    }

    close(sock);
    return 0;
}