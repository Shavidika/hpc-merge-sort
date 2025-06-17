#ifndef MERGE_UTILS_H
#define MERGE_UTILS_H

#include <vector>

inline void merge(std::vector<int> &arr, int left, int mid, int right)
{
    std::vector<int> leftArr(arr.begin() + left, arr.begin() + mid + 1);
    std::vector<int> rightArr(arr.begin() + mid + 1, arr.begin() + right + 1);
    int i = 0, j = 0, k = left;
    while (i < (int)leftArr.size() && j < (int)rightArr.size())
    {
        if (leftArr[i] <= rightArr[j])
            arr[k++] = leftArr[i++];
        else
            arr[k++] = rightArr[j++];
    }
    while (i < (int)leftArr.size())
        arr[k++] = leftArr[i++];
    while (j < (int)rightArr.size())
        arr[k++] = rightArr[j++];
}

inline void mergeSort(std::vector<int> &arr, int left, int right)
{
    if (left >= right)
        return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

#endif // MERGE_UTILS_H
