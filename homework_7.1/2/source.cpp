#include <iostream>
#include <Windows.h>
#include <iomanip>
#include <vector>
#include <thread>
#include <chrono>
#include <random>

using namespace std;


struct Variants {
    const size_t VECTOR_SIZE;
    vector<double> vecA;
    vector<double> vecB;
    vector<double> result;

    Variants(int size) :
        VECTOR_SIZE(size),
        vecA(VECTOR_SIZE),
        vecB(VECTOR_SIZE),
        result(VECTOR_SIZE)
    {}
};

void partial_sum(const vector<double>& a, const vector<double>& b, vector<double>& result, size_t start, size_t end) {
    for (size_t i = start; i < end; i++) {
        result[i] = a[i] + b[i];
    }
}

void parallel_vector_sum(const vector<double>& a, const vector<double>& b, vector<double>& result, int threads_num) {
    vector<thread> threads;
    size_t size = a.size();
    size_t chunk_size = (size + threads_num - 1) / threads_num;

    for (int i = 0; i < threads_num; i++) {
        size_t start = i * chunk_size;
        size_t end = min((i + 1) * chunk_size, size);

        if (start < size) {
            threads.emplace_back(
                static_cast<void(*)(const vector<double>&,
                    const vector<double>&,
                    vector<double>&,
                    size_t, size_t)>(partial_sum),
                ref(a), ref(b), ref(result), start, end);
        }
    }

    for (auto& t : threads) {
        t.join();
    }
}

void print_main_info(const vector<Variants>& variants) {
    cout << "Количество аппаратных ядер - " << thread::hardware_concurrency() << endl << endl;

    cout << std::setw(20);
    for (const Variants& variant : variants) {
        cout << variant.VECTOR_SIZE << "\t\t";
    }
    cout << endl;
}


int main() {

    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    vector<Variants> variants = { 1000, 10000, 100000, 1000000 };

    mt19937 gen;
    uniform_real_distribution<double> dis(0.0, 100.0);

    for (Variants& variant : variants) {
        for (size_t i = 0; i < variant.VECTOR_SIZE; i++) {
            variant.vecA[i] = dis(gen);
            variant.vecB[i] = dis(gen);
        }
    }

    vector<int> thread_counts = { 1, 2, 4, 8, 16 };

    print_main_info(variants);

    for (int threads : thread_counts) {

        cout << threads << " потоков: \t";
        for (Variants variant : variants) {
            auto start = chrono::high_resolution_clock::now();

            parallel_vector_sum(variant.vecA, variant.vecB, variant.result, threads);

            auto end = chrono::high_resolution_clock::now();
            auto duration = chrono::duration_cast<chrono::duration<double, milli>>(end - start);

            cout << fixed << setprecision(3);
            cout << duration.count() << " мс\t";
        }
        cout << endl;
    }

    return 0;
}