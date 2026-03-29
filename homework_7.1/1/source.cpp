#include <iostream>
#include <thread>

using namespace std;


void worker(int& counter) {
    // cout << "Worker thread number = " << this_thread::get_id() << endl;
    while (counter) {
        counter--;
        cout << "Worker. Counter = " << counter << endl;
        this_thread::sleep_for(2s);
    }
}

class Client {
public:
    Client(int number) : clientsNumber(number) {}

    void call(int& counter) {
        // cout << "Client thread number = " << this_thread::get_id() << endl;
        while (clientsNumber) {
            counter++;
            clientsNumber--;
            cout << "Client call. Counter = " << counter << endl;
            this_thread::sleep_for(1s);
        }
    }

private:
    int clientsNumber;
};

int main() {
    // cout << thread::hardware_concurrency() << endl;

    int counter{};
    Client client(10);

    thread t1(&Client::call, &client, ref(counter));
    thread t2(worker, ref(counter));

    t1.join();
    t2.join();

    return 0;
}