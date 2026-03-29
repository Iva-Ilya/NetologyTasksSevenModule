
#include <iostream>
#include <thread>
#include <vector>
#include <chrono>
#include <mutex>
#include <Windows.h>
#include <iomanip>﻿

const int progress_bar_width = 50; // ширина прогресс-бара

std::mutex cout_mutex; // мьютекс для синхронного вывода

struct ThreadInfo
{
    int thread_number;
    std::thread thread;
    double duration_seconds;
    int progress; // текущий прогресс (0..progress_bar_width)
    bool finished;
};

void worker(int thread_number, int total_steps, ThreadInfo& info)
{
    auto start_time = std::chrono::steady_clock::now();

    for (int i = 0; i <= total_steps; ++i)
    {
        // Обновляем прогресс
        info.progress = (i * progress_bar_width) / total_steps;

        // Имитация работы
        std::this_thread::sleep_for(std::chrono::milliseconds(10));

        // Обновляем статус завершения
        if (i == total_steps)
        {
            info.finished = true;
        }
    }

    auto end_time = std::chrono::steady_clock::now();
    info.duration_seconds = std::chrono::duration<double>(end_time - start_time).count();
}

void print_progress(const std::vector<ThreadInfo>& threads)
{

    std::lock_guard<std::mutex> lock(cout_mutex);

    // Перебираем все потоки и выводим их прогресс
    for (const auto& t : threads)
    {
        // Номер потока и идентификатор
        std::cout << "Поток #" << t.thread_number << " (ID: " << t.thread.get_id() << ")" << std::endl;

        // Прогресс-бар
        std::cout << "[";
        for (int i = 0; i < progress_bar_width; ++i)
        {
            if (i < t.progress)
                std::cout << "=";
            else
                std::cout << " ";
        }
        std::cout << "]";

        // Если поток завершен, выводим время
        if (t.finished)
        {
            std::cout << " Время: " << std::fixed << std::setprecision(4) << t.duration_seconds << " сек";
        }
        std::cout << std::endl;
    }
}

int main()
{
    SetConsoleOutputCP(1251);
    SetConsoleCP(1251);

    int num_threads = 4;    // Количество потоков
    int total_steps = 1000; // Длина расчёта (количество итераций)


    std::vector<ThreadInfo> threads_info;
    threads_info.reserve(num_threads);

    // Создаем потоки
    for (int i = 0; i < num_threads; ++i)
    {
        threads_info.push_back({ i + 1, std::thread(), 0.0, 0, false });
    }

    // Запускаем потоки
    for (int i = 0; i < num_threads; ++i)
    {
        threads_info[i].thread = std::thread(worker, i + 1, total_steps, std::ref(threads_info[i]));
    }

    // Обновляем прогресс-бар до завершения
    bool all_finished = false;
    while (!all_finished)
    {
        {
            // Очищаем экран (зависит от ОС)
#ifdef _WIN32
            system("cls");
#else
            system("clear");
#endif
        }

        print_progress(threads_info);

        // Проверяем, все ли завершились
        all_finished = true;

        for (const auto& t : threads_info)
        {
            if (!t.finished)
            {
                all_finished = false;
                break;
            }
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    // Ждем завершения всех потоков
    for (auto& t : threads_info)
    {
        if (t.thread.joinable())
        {
            t.thread.join();
        }
    }

    // Финальный вывод
    {
        std::lock_guard<std::mutex> lock(cout_mutex);

        std::cout << "\nВсе потоки завершены." << std::endl;

        for (const auto& t : threads_info)
        {
            std::cout << "Поток #" << t.thread_number << " (ID: " << t.thread.get_id()
                << ") завершился за " << std::fixed << std::setprecision(4) << t.duration_seconds << " сек."
                << std::endl;
        }
    }

    return 0;
}