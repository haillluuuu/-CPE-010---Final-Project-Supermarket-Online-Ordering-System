#ifndef SEARCHING_ALGO_H
#define SEARCHING_ALGO_H

#include <iostream>
#include <vector>
#include <string>

// Binary search implementation 
template <typename T, typename KeyType, typename Extractor>
int binarySearchVector(const std::vector<T>& vec, KeyType targetKey, Extractor extract) {
    int low = 0;
    int high = static_cast<int>(vec.size()) - 1;

    while (low <= high) {
        int mid = low + (high - low) / 2;
        KeyType midVal = extract(vec[mid]);

        if (midVal == targetKey) {
            return mid;
        }
        if (midVal < targetKey) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return -1;
}

// Linear search implementation using standard arrays 
template <typename T>
void linearSearchArr(T data[], int arrSize, T dataFind) {
    int i = 0;
    while (i < arrSize) {
        if (dataFind == data[i]) {
            std::cout << "The search is successful\n";
            return;
        }
        i++;
    }
    std::cout << "The element is not found\n";
}

#endif