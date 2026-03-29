#include <iostream>
#include <thread>
#include <atomic>
#include <mutex>

using namespace std;

mutex mtx1;

void worker(atomic<int>& counter) {
    // cout << "Worker thread number = " << this_thread::get_id() << endl;
    while (counter) {
        counter.store(--counter);
        unique_lock<mutex> ul1(mtx1);
        cout << "Worker. Counter = " << counter << endl;
        ul1.unlock();
        this_thread::sleep_for(2s);
    }
}

class Client {
public:
    Client(int number) : clientsNumber(number) {}

    void call(atomic<int>& counter) {
        // cout << "Client thread number = " << this_thread::get_id() << endl;
        while (clientsNumber) {
            counter.store(++counter);
            clientsNumber--;
            unique_lock<mutex> ul1(mtx1);
            cout << "Client call. Counter = " << counter << endl;
            ul1.unlock();
            this_thread::sleep_for(1s);
        }
    }

private:
    int clientsNumber;
};

int main() {
    // cout << thread::hardware_concurrency() << endl;

    atomic<int> counter;
    counter.store(0);
    Client client(10);

    thread t1(&Client::call, &client, ref(counter));
    thread t2(worker, ref(counter));

    t1.join();
    t2.join();

    return 0;
}