#include <iostream>
#include <fstream>
#include <cmath>
#include <chrono>

long long shell_sort(int* arr, int n, int* gaps, int gap_count) {
    long long swaps = 0;

    for (int g = 0; g < gap_count; ++g) { 
        int gap = gaps[g]; // по факту этот блок это for gap in gaps:

        for (int i = gap; i < n; ++i) {
            int temp = arr[i]; //берем эл-т массива сразу скипая gap штук, их все равно не с чем слева будет сравнивать
            int j = i; //чтоб i случайно не поменять

            while (j >= gap and arr[j - gap] > temp) { //пока смотрим на элемент, слева от которого через gap стоит больший его...
                arr[j] = arr[j - gap]; 
                j -= gap; //...заменяем тот, на которого смотрим, на больший и идем влево на gap еще раз, сравнивая теперь уже его
                swaps++; 
            }
            arr[j] = temp; //теперь заменяем максимального из gap-слева (по-любому на нем и остановимся, см. условие while) на того, на которого смотрели
        }
    }
    return swaps;
}
// ПРИМЕР РАБОТЫ SHELL SORT: [..., 7, -gap-, 52, -gap-, 34, -gap-, 2, ...]: 2->34->52. 52->2. Остановка на 52, т.к. 7<52
//                              j-3gap     j-2gap     j-gap        j

int main() {
    int sizes[] = {5000, 10000, 50000, 100000, 250000, 500000, 750000, 1000000, 1750000, 2500000, 3250000, 5000000};
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    unsigned int seed = 67;
    std::ofstream outFile("output.txt");

    for (int i = 0; i < num_sizes; ++i) {
        int n = sizes[i];
        
        for (int type = 1; type <= 3; ++type) {
            int* data = new int[n];
            std::srand(seed);
            for (int j = 0; j < n; ++j) data[j] = std::rand() % 100000;

            int gaps[100]; //уже на 50 элементах будут невообразимо гигантские значения для n, 100 точно хватит
            int gap_count = 0;

            if (type == 1) { // В идеале Shell это примерно N^1.5
                for (int d = n / 2; d > 0; d /= 2) gaps[gap_count++] = d;
            } 
            else if (type == 2) { // В идеале Hibbard это примерно N^1.3
                int k = 1;
                while ((1 << k) - 1 < n) {k++;} //a << b - побитовый сдвиг битов a влево на b позиций, который является изи методом возведения в степень 2: a << b = a * 2^b
                for (int j = k - 1; j >= 1; --j) gaps[gap_count++] = (1 << j) - 1;
            } 
            else if (type == 3) { //В идеале Fibonacci это примерно N^1.25-1.3
                int a = 1, b = 2; // начинаем с 2 и 3 чисел Фиб: это неважно, тк шаг =н1 будет последним
                int fib[100];
                int f_idx = 0;
                while (a < n) {
                    fib[f_idx++] = a;
                    int next = a + b;
                    a = b;
                    b = next;
                }
                for (int j = f_idx - 1; j >= 0; --j) gaps[gap_count++] = fib[j];
            }

            auto start = std::chrono::high_resolution_clock::now();
            long long s = shell_sort(data, n, gaps, gap_count);
            auto end = std::chrono::high_resolution_clock::now();
            
            std::chrono::duration<double, std::milli> duration = end - start;
            outFile << type << " " << n << " " << duration.count() << " " << s << std::endl;
            
            delete[] data;
        }
    }
    outFile.close();
    return 0;
}
