#include <iostream>
#include <algorithm>
#include <cassert>

//асимптотика O(N^2) - всего операций в ср N-1 + N-2 + ... = N(N-1)/2 = O(N^2)

void forward_step(unsigned arr[], unsigned const begin_idx, unsigned const end_idx) {
    for (unsigned i = begin_idx; i < end_idx; ++i) {
        if (arr[i] > arr[i + 1]) std::swap(arr[i], arr[i + 1]);
    }
}

void backward_step(unsigned arr[], unsigned const begin_idx, unsigned const end_idx) {
    for (unsigned i = end_idx; i > begin_idx; --i) {
        if (arr[i] < arr[i - 1]) std::swap(arr[i], arr[i - 1]);
    }
}

void shaker_sort(unsigned arr[], unsigned const begin_idx, unsigned const end_idx) {
    unsigned left = begin_idx;
    unsigned right = end_idx;
    while (left < right) {
        forward_step(arr, left, right);
        right--;
        backward_step(arr, left, right);
        left++;
    }
}


void test_forward() {
    unsigned arr[] = {1000, 95, 8, 343, 1};
    forward_step(arr, 0, 4);
    assert(arr[4] == 1000);
    std::cout << "Forward test passed" << std::endl;
}

void test_backward() {
    unsigned arr[] = {78, 0, 456, 1, 1};
    backward_step(arr, 0, 4);
    assert(arr[0] == 0);
    std::cout << "Backward test passed" << std::endl;
}

void test_shaker() {
    unsigned arr[] = {5, 2, 96, 1, 5, 67};
    shaker_sort(arr, 0, 5);
    for (int i = 0; i < 5; ++i) {
        assert(arr[i] <= arr[i + 1]);
    }
    std::cout << "Shaker test passed" << std::endl;
}

int main() {
    test_forward();
    test_backward();
    test_shaker();
    return 0;
}
