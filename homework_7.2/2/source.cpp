#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <Windows.h>
#include <chrono>
#include <random>
#include <string>

using namespace std;

class consol_parameter
{
public:
    static void SetColor(int text, int background)
    {
        SetConsoleTextAttribute(hStdOut, (WORD)((background << 4) | text));
    }

    static std::pair<int, int> GetCurrentConsoleColor()
    {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        GetConsoleScreenBufferInfo(hStdOut, &csbi);
        WORD attributes = csbi.wAttributes;

        int textColor = attributes & 0x0F;
        int bgColor = (attributes >> 4) & 0x0F;

        return { textColor, bgColor };
    }

    static void ResetColor() {
        SetColor(0, 0);
    }

    static void SetPosition(int x, int y)
    {
        COORD point;
        point.X = x;
        point.Y = y;
        SetConsoleCursorPosition(hStdOut, point);
    }
private:
    static HANDLE hStdOut;
};
HANDLE consol_parameter::hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);


class Timer
{
private:
    using clock_t = std::chrono::high_resolution_clock;
    using second_t = std::chrono::duration<double, std::ratio<1> >;

    std::string m_name;
    std::chrono::time_point<clock_t> m_beg;
    double elapsed() const
    {
        return std::chrono::duration_cast<second_t>(clock_t::now() - m_beg).count();
    }

public:
    Timer() : m_beg(clock_t::now()) { }
    Timer(std::string name) : m_name(name), m_beg(clock_t::now()) { }

    void start(std::string name) {
        m_name = name;
        m_beg = clock_t::now();
    }
    void print() const {
        // std::cout << m_name << ":\t" << elapsed() * 1000 << " ms" << '\n';
        std::cout << elapsed() * 1000 << " ms";
    }
};


void printHeader() {
    consol_parameter::SetPosition(0, 0);
    cout << left << setw(5) << "#" << setw(5) << "id" << setw(30) << "Progress Bar" << "  " << "Time";
}


mutex mtx;
constexpr int PROGRESS_START_COL = 10;
constexpr int TIME_COL = PROGRESS_START_COL + 30 + 2;

void drawRowProgress(size_t threadNumber) {
    unique_lock<mutex> ul(mtx);
    consol_parameter::SetPosition(0, threadNumber);
    cout << left;
    cout << setw(5) << threadNumber;
    cout << setw(5) << this_thread::get_id();
    ul.unlock();

    thread_local mt19937 gen(random_device{}());
    uniform_int_distribution<> dist(1, 3);
    uniform_int_distribution<> errorDist(1, 100);

    Timer timer;
    timer.start("timer");

    for (int i = 0; i < 30;) {
        try {
            int step = dist(gen);
            if (i + step > 30) {
                step = 30 - i;
            }

            if (errorDist(gen) <= 10) {
                throw std::runtime_error("Error in thread " + std::to_string(threadNumber));
            }

            ul.lock();
            consol_parameter::SetPosition(PROGRESS_START_COL + i, threadNumber);

            for (int j = 0; j < step; j++) {
                cout << '|';
            }

            i += step;
            ul.unlock();
            this_thread::sleep_for(200ms);
        }
        catch (const std::exception& e) {
            ul.lock();
            consol_parameter::SetPosition(PROGRESS_START_COL + i, threadNumber);
            consol_parameter::SetColor(12, 0);
            cout << '!';
            consol_parameter::ResetColor();
            i++;
            ul.unlock();
            this_thread::sleep_for(200ms);
        }
    }

    ul.lock();
    consol_parameter::SetPosition(TIME_COL, threadNumber);
    timer.print();
}


int main() {
    system("clear");

    int threadQuant{ 5 };

    printHeader();
    consol_parameter::SetPosition(0, 1);

    vector<thread> threads(threadQuant);
    for (int i = 0; i < threadQuant; i++) {
        threads[i] = thread(drawRowProgress, i + 1);
    }

    for (size_t i = 0; i < threadQuant; i++) {
        threads[i].join();
    }

    return 0;
}
