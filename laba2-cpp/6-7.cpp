#include <iostream>
#include <ctime>
#include <chrono>
#include <fstream>

//асимптотика O(NlogN): N раз проходим по массиву за шаг, logN раз уменьшим шаг по опр log

int swapping_for_step(int* arr, int n, int step) {
    int swaps = 0;
    for (int i = 0; i + step < n; ++i) {
        if (arr[i] > arr[i + step]) {
            std::swap(arr[i], arr[i + step]);
            swaps++;
        }
    }
    return swaps;
}

long long comb_sort(int* arr, int n) {
    int step = n;
    float shrink = 1.3f;
    //делить на 2 - слишком быстро прийти к шагу=1, 1.1 - слишком медленно
    //идеальное число это 1.3, близкое к 1/(1-e^-fi) =(aprox) 1.247
    long long total_swaps = 0;
    bool is_sorted = false;

    while (!is_sorted) {
        step = (int)(step / shrink);
        if (step <= 1) {
            step = 1;
            is_sorted = true;
        }

        int current_swaps = swapping_for_step(arr, n, step);
        total_swaps += current_swaps;

        if (step == 1 and current_swaps > 0) {
            is_sorted = false;
        }
    }
    return total_swaps;
}

int main() {
    int sizes[] = {5000, 10000, 50000, 100000, 250000, 500000, 750000, 1000000, 1750000, 2500000, 3250000, 5000000};
    int num_tests = sizeof(sizes) / sizeof(sizes[0]);
    
    unsigned int seed = 67; 
    std::ofstream outFile("output.txt");

    std::cout << "N\tTime(ms)\tSwaps" << std::endl;

    for (int i = 0; i < num_tests; ++i) {
        int n = sizes[i];
        int* data = new int[n];
        
        std::srand(seed); 
        for (int j = 0; j < n; ++j) {
            data[j] = std::rand() % 10000;
        }

        auto start = std::chrono::high_resolution_clock::now();
        long long s = comb_sort(data, n);
        auto end = std::chrono::high_resolution_clock::now();
        
        std::chrono::duration<double, std::milli> duration = end - start;
        std::cout << n << "\t" << duration.count() << "\t\t" << s << std::endl;

        outFile << n << " " << duration.count() << " " << s << std::endl;

        delete[] data;
    }
    outFile.close();
    return 0;
}
