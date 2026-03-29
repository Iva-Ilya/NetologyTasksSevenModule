#include <iostream>
#include <future>
#include <algorithm>
#include <vector>
#include <mutex>

using namespace std;

template <typename Iterator, typename Function>
void user_for_each(Iterator begin, Iterator end, Function& f) {
    int size = distance(begin, end);
    if (size <= 2) {
        for_each(begin, end, f);
    }
    else {
        Iterator middle = begin;
        advance(middle, size / 2);
        auto res1 = async(launch::async, user_for_each<Iterator, Function>, begin, middle, ref(f));
        user_for_each(middle, end, f);
        res1.get();
    }
}

mutex mtx;
class Class1 {
public:
    void operator() (int a) {
        std::lock_guard<mutex> lock(mtx);
        if (!(a % 3)) {
            cout << a << " ";
        }
    }
};

constexpr int SIZE = 10;

int main() {
    int arr[SIZE] = { 12, 4, 2, 65, 84, 23, 6, 123, 5, 90 };
    Class1 ob1;

    cout << "Before function: ";
    for (int i = 0; i < SIZE; i++) {
        cout << arr[i] << " ";
    }
    cout << endl;

    cout << "Multiple of 3 are: ";
    user_for_each(arr, arr + 10, ob1);

    return 0;
}