#include <iostream>
#include <thread>
#include <mutex>
#include <random>
#include <string>
#include <shared_mutex>
using namespace std;


mutex data_mutex;
mutex completed_mutex;
mutex output_mutex;

vector<char> final_message;

bool completed = false;
bool progress = false;


void download(string message) {
    
    size_t size = message.size();
    if (size == 0) {
        lock_guard<mutex> completed_lock(completed_mutex);
        bool completed = true;
        return;
    }
    
    for (size_t i{}; i < size; i++) {
        this_thread::sleep_for(100ms);
        lock_guard<mutex> lock(data_mutex);
        progress = true;
        final_message.push_back(message[i]);
        
    }
    lock_guard<mutex> completed_lock(completed_mutex);
    completed = true;
}
void progress_bar() {
    while (true) {
        unique_lock<mutex> completed_lock(completed_mutex);
        if (completed) {
            lock_guard<mutex> output_lock(output_mutex);
            cout << " 100%\nDownload is completed" << endl;
            return;
        }
        completed_lock.unlock();
        unique_lock<mutex> data_lock(data_mutex);
        while (!progress) {
            data_lock.unlock();
            this_thread::sleep_for(100ms);
            data_lock.lock();
        }
        progress = false;
        lock_guard<mutex> output_lock(output_mutex);
        cout << "#";
        
    }
}
void completion() {
    unique_lock<mutex> completed_lock(completed_mutex);
    while (!completed) {
        completed_lock.unlock();
        this_thread::sleep_for(100ms);
        completed_lock.lock();
    }
    completed_lock.unlock();
    lock_guard<mutex> lock(data_mutex);
    unique_lock<mutex> output_lock(output_mutex, defer_lock);
    if (final_message.size() == 0) {
        output_lock.lock();
        cout << "Empty file" << endl;
        return;
    }
    output_lock.lock();
    for (auto c : final_message) {
        cout << c;
    }
    cout << endl;
}

int main()
{
    string message;
    getline(cin, message);
    cin.ignore();
    thread downloading_thread(download, message);
    thread progress_bar_thread(progress_bar);
    thread completion_thread(completion);
    downloading_thread.join();
    progress_bar_thread.join();
    completion_thread.join();
    return 0;
}

