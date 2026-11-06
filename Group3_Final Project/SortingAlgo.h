#ifndef SORTING_ALGO_H
#define SORTING_ALGO_H

#include <iostream>
#include <vector>
#include <algorithm>

// Partition helper function for Quick Sort 
template <typename T, typename Comparator>
int partition(std::vector<T>& arr, int low, int high, Comparator comp) {
    T pivot = arr[high];
    int i = low - 1;

    for (int j = low; j < high; ++j) {
        if (comp(arr[j], pivot)) {
            i++;
            std::swap(arr[i], arr[j]);
        }
    }
    std::swap(arr[i + 1], arr[high]);
    return i + 1;
}

// Quick Sort recursive divide-and-conquer implementation 
template <typename T, typename Comparator>
void quickSort(std::vector<T>& arr, int low, int high, Comparator comp) {
    if (low < high) {
        int pi = partition(arr, low, high, comp);
        quickSort(arr, low, pi - 1, comp);
        quickSort(arr, pi + 1, high, comp);
    }
}

#endif