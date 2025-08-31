# wordle

1.predefine answer[5 words] -> list 
condition: English alphabet and case-insensitive

2. 3 statuts :hit, present,miss


Solution Expect:
maximum number of round
Only can input 5-letter words

guess the words -> win
guess the words after maximumround->lose

one word can only match onetimes apple->ppppp should only two words have hit or present

We assume our wordlist only have words: "apple house train crane plant money water light" to prevents the player input invaild english words.

# task1
Step1
g++ task1.cpp -o task1

Step2
./task1



# task2
Step1
g++ task2_server.cpp -o task2_server
g++ task2_client.cpp -o task2_client


Step2
split terminal

Start the server in one terminal
./task2_client

Start the server in another terminal
./task2_server



#task4
g++ task4_server.cpp -o task4_server
g++ task4_client.cpp -o task4_client

Step2
split terminal

Start the server in one terminal
./task4_server

Start the server in another terminal
./task4_client
