#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <string>
#include <shared_mutex>
#include <condition_variable>
using namespace std;

struct Conditions {
    condition_variable completed;
    bool is_completed = false;;
    condition_variable progress;
    bool has_progress = false;
};
Conditions conditions;
mutex data_mutex;
mutex completion_mutex;

vector<char> final_message;




void download(string message) {
    size_t size = message.size();
    if (size == 0) {
        conditions.completed.notify_all();
        return;
    }
    conditions.is_completed = false;
    conditions.has_progress = false;
    for (size_t i{}; i < size; i++) {
        this_thread::sleep_for(100ms);
        lock_guard<mutex> lock(data_mutex);
        final_message.push_back(message[i]);
        conditions.has_progress = true;
        conditions.progress.notify_all();
    }
    this_thread::sleep_for(100ms);
    conditions.is_completed = true;
    conditions.completed.notify_all();
}
void progress_bar() {
    while (true) {
        unique_lock<mutex> data_lock(data_mutex);
        if (conditions.is_completed) {
            return;
        }
        conditions.progress.wait(data_lock, [] { return conditions.has_progress; });
        cout << "#";
        conditions.has_progress = false;
    }
}
void completion() {
    unique_lock<mutex> completed_lock(completion_mutex);
    conditions.completed.wait(completed_lock, [] {return conditions.is_completed; });
    cout << " 100%\nDownload is completed" << endl;
    lock_guard<mutex> lock(data_mutex);
    if (final_message.size() == 0) {
        cout << "Empty file" << endl;
        return;
    }
    for (auto c : final_message) {
        cout << c;
    }
    cout << endl;
}

int main()
{
    string message;
    getline(cin, message);
    thread downloading_thread(download, message);
    thread progress_bar_thread(progress_bar);
    thread completion_thread(completion);
    downloading_thread.join();
    progress_bar_thread.join();
    completion_thread.join();
    return 0;
}

