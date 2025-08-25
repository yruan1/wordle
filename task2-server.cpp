#include <iostream>
#include <string>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

int main() {
    const int PORT = 8080;
    int sock = 0;
    struct sockaddr_in serv_addr;
    char buffer[1024] = {0};

    sock = socket(AF_INET, SOCK_STREAM, 0);

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
        cout << "Connection Failed\n";
        return -1;
    }

    cout << "Connected to Wordle server.\n";

    while (true) {
        string guess;
        cout << "Enter your 5-letter guess: ";
        cin >> guess;

        send(sock, guess.c_str(), guess.size(), 0);
        int valread = read(sock, buffer, 1024);
        if (valread <= 0) break;

        string response(buffer, valread);
        cout << response;

        if (response.find("WIN") != string::npos ||
            response.find("LOSE") != string::npos) {
            break;
        }
        memset(buffer, 0, sizeof(buffer));
    }

    close(sock);
    return 0;
}