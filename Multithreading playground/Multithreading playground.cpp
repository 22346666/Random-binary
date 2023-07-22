#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <string>
#include <shared_mutex>
using namespace std;
random_device randint;
mt19937 engine(randint());
uniform_int_distribution<int> distr(0, 1);
uniform_int_distribution<int> ind(0, 4);

shared_mutex task_mutex;

void add_binary(vector<int>& binary) {
    lock_guard<shared_mutex> lock(task_mutex);
    int value = distr(engine);
    binary.push_back(value);
    cout << "Wrote number " <<  value << " to the end" << endl;
}
void read_binary(const vector<int>& binary) {
    shared_lock<shared_mutex> lock(task_mutex);
    if (binary.empty()) {
        cout << "Empty" << endl;
        return;
    }
    for (auto i : binary) {
        cout << i;
    }
    cout << endl;

}


int main()
{
    
    int num;
    cin >> num;
    cin.ignore();
    vector<int> data;
    vector <thread> threads;
    for (int i{}; i < num; i++) {
        int choise = distr(engine);
        switch (choise) {
        case 0:
            threads.push_back(thread(add_binary, ref(data)));
            threads[i].join();
            break;
        case 1:
            threads.push_back(thread(read_binary, ref(data)));
            threads[i].join();
            break;
        default:
            continue;
            break;
        }
        
    }
    return 0;
}

