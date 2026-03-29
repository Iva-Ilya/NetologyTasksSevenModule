#include <iostream>
#include <thread>
#include <mutex>

using namespace std;

struct Data {
    int data;
    mutex mtx;
    Data(int value) : data(value) {}
};


void swap1(Data& data1, Data& data2) {
    lock(data1.mtx, data2.mtx);
    lock_guard<mutex> lg1{ data1.mtx, adopt_lock };
    lock_guard<mutex> lg2{ data2.mtx, adopt_lock };

    Data temp(0);
    temp.data = data1.data;
    data1.data = data2.data;
    data2.data = temp.data;
}

void swap2(Data& data1, Data& data2) {
    unique_lock<mutex> ul1{ data1.mtx, defer_lock };
    unique_lock<mutex> ul2{ data2.mtx, defer_lock };
    lock(ul1, ul2);

    Data temp(0);
    temp.data = data1.data;
    data1.data = data2.data;
    data2.data = temp.data;
}

void swap3(Data& data1, Data& data2) {
    std::scoped_lock sl{ data1.mtx, data2.mtx };

    Data temp(0);
    temp.data = data1.data;
    data1.data = data2.data;
    data2.data = temp.data;
}



int main() {
    Data data1(5);
    Data data2(13);

    cout << "Before swap" << endl;
    cout << "data1.data = " << data1.data << endl;
    cout << "data2.data = " << data2.data << endl;

    swap1(data1, data2);

    cout << "\nAfter swap1" << endl;
    cout << "data1.data = " << data1.data << endl;
    cout << "data2.data = " << data2.data << endl;

    swap2(data1, data2);

    cout << "\nAfter swap2" << endl;
    cout << "data1.data = " << data1.data << endl;
    cout << "data2.data = " << data2.data << endl;

    swap3(data1, data2);

    cout << "\nAfter swap3" << endl;
    cout << "data1.data = " << data1.data << endl;
    cout << "data2.data = " << data2.data << endl;


    return 0;
}