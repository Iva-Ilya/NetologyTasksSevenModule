#include <iostream>
#include <future>

using namespace std;

void find_min_element(int* arr, int size, int start_index, promise<int>& prom) {
    int min_index = start_index;
    for (int j = start_index + 1; j < size; j++) {
        if (arr[j] < arr[min_index]) {
            min_index = j;
        }
    }
    prom.set_value(min_index);
}

void selection_sort(int* arr, int size) {
    for (int i = 0; i < size - 1; i++) {
        promise<int> prom;
        future<int> fut = prom.get_future();

        auto res = async(find_min_element, arr, size, i, ref(prom));

        int min_index = fut.get();
        if (min_index != i) {
            swap(arr[i], arr[min_index]);
        }
    }
}

constexpr int SIZE = 10;
int main() {
    int array[SIZE] = { 9, 23, 34, 1, 95, 65, 324, 8, 85, 753 };

    cout << "Before sort: ";
    for (int i = 0; i < SIZE; i++) {
        cout << array[i] << " ";
    }
    cout << endl;

    selection_sort(array, SIZE);

    cout << "After sort: ";
    for (int i = 0; i < SIZE; i++) {
        cout << array[i] << " ";
    }
    cout << endl;

    return 0;
}