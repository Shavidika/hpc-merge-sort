#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>

using namespace std;
using namespace std::chrono;

void merge(vector<int>& arr, int left, int mid, int right) {
    vector<int> leftArr(arr.begin() + left, arr.begin() + mid + 1);
    vector<int> rightArr(arr.begin() + mid + 1, arr.begin() + right + 1);

    int i = 0, j = 0, k = left;

    while (i < (int)leftArr.size() && j < (int)rightArr.size()) {
        if (leftArr[i] <= rightArr[j]) {
            arr[k++] = leftArr[i++];
        } else {
            arr[k++] = rightArr[j++];
        }
    }

    while (i < (int)leftArr.size()) arr[k++] = leftArr[i++];
    while (j < (int)rightArr.size()) arr[k++] = rightArr[j++];
}

void mergeSort(vector<int>& arr, int left, int right) {
    if (left >= right) return;
    int mid = left + (right - left) / 2;

    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
    merge(arr, left, mid, right);
}

vector<int> readInputFile(const string& filename) {
    vector<int> data;
    ifstream infile(filename);
    int value;
    while (infile >> value) {
        data.push_back(value);
    }
    infile.close();
    return data;
}

void writeOutputFile(const string& filename, const vector<int>& data) {
    ofstream outfile(filename);
    for (int num : data) {
        outfile << num << "\n";
    }
    outfile.close();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    string inputFile = argv[1];
    vector<int> data = readInputFile(inputFile);

    cout << "Sorting " << data.size() << " elements...\n";

    auto start = high_resolution_clock::now();
    mergeSort(data, 0, data.size() - 1);
    auto end = high_resolution_clock::now();

    double duration = duration_cast<milliseconds>(end - start).count() / 1000.0;
    cout << "Time taken: " << duration << " seconds\n";

    writeOutputFile("output_serial.txt", data);

    return 0;
}
