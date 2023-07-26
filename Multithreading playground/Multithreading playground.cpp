#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <string>
#include <shared_mutex>
#include <future>
using namespace std;
random_device randint;
mt19937 engine(randint());
uniform_int_distribution<int> binary_distr(0, 1);


void produce(const string& message, promise<string>& produce_string) {
    string produced;
    int size = message.size();
    for (int i = 0; i < size; i++) {
        uniform_int_distribution<int> distr(0, size-1);
        int index = distr(engine);
        char c;
        if (message[index] == ' ') {
            c = ' ';
        }
        else {
            int choise = binary_distr(engine);
            c = (choise == 0) ? toupper(message[index]) : tolower(message[index]);
        }
        produced.push_back(c);
        cout << "/";
        this_thread::sleep_for(120ms);
    }
    cout << endl;
    cout << "Finished mutilating string!" << endl;
    produce_string.set_value(produced);
}
void consume(future<string>& consume_string) {
    string output = consume_string.get();
    for (auto c : output) {
            this_thread::sleep_for(120ms);
            cout << c;
    }
}

int main()
{
    string input;
    getline(cin, input);
    promise<string> produce_string;
    future<string> consume_string = produce_string.get_future();
    thread produce_thread(produce, ref(input), ref(produce_string));
    thread consume_thread(consume, ref(consume_string));
    produce_thread.join();
    consume_thread.join();
    return 0;
}

