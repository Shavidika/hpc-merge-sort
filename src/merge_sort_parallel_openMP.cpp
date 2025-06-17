#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <string>
#include <omp.h>

using namespace std;
using namespace std::chrono;

void merge(vector<int> &arr, int left, int mid, int right)
{
    vector<int> leftArr(arr.begin() + left, arr.begin() + mid + 1);
    vector<int> rightArr(arr.begin() + mid + 1, arr.begin() + right + 1);
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

void mergeSort(vector<int> &arr, int left, int right, int depth = 0)
{
    if (left >= right)
        return;
    int mid = left + (right - left) / 2;
    if (depth <= 2) // limit parallelism depth to avoid oversubscription
    {
#pragma omp parallel sections
        {
#pragma omp section
            mergeSort(arr, left, mid, depth + 1);
#pragma omp section
            mergeSort(arr, mid + 1, right, depth + 1);
        }
    }
    else
    {
        mergeSort(arr, left, mid, depth + 1);
        mergeSort(arr, mid + 1, right, depth + 1);
    }
    merge(arr, left, mid, right);
}

vector<int> readInputFile(const string &filename)
{
    vector<int> data;
    ifstream infile(filename);
    int value;
    while (infile >> value)
        data.push_back(value);
    infile.close();
    return data;
}

void writeOutputFile(const string &filename, const vector<int> &data)
{
    ofstream outfile(filename);
    for (int num : data)
        outfile << num << "\n";
    outfile.close();
}

bool compareFiles(const std::string &file1, const std::string &file2)
{
    std::ifstream f1(file1), f2(file2);
    std::string line1, line2;
    bool f1_has_line, f2_has_line;
    while (true)
    {
        f1_has_line = static_cast<bool>(std::getline(f1, line1));
        f2_has_line = static_cast<bool>(std::getline(f2, line2));
        if (!f1_has_line && !f2_has_line)
            break;
        if (f1_has_line != f2_has_line)
            return false;
        if (line1 != line2)
            return false;
    }
    return true;
}

void printFileDifferences(const std::string &file1, const std::string &file2, int max_diffs = 10)
{
    std::ifstream f1(file1), f2(file2);
    std::string line1, line2;
    int line_num = 1, diff_count = 0;
    bool f1_has_line, f2_has_line;
    while ((f1_has_line = static_cast<bool>(std::getline(f1, line1))) | (f2_has_line = static_cast<bool>(std::getline(f2, line2))))
    {
        if (!f1_has_line)
        {
            std::cout << "Extra line in " << file2 << " at line " << line_num << ": " << line2 << "\n";
            diff_count++;
        }
        else if (!f2_has_line)
        {
            std::cout << "Extra line in " << file1 << " at line " << line_num << ": " << line1 << "\n";
            diff_count++;
        }
        else if (line1 != line2)
        {
            std::cout << "Difference at line " << line_num << ":\n";
            std::cout << "  " << file1 << ": " << line1 << "\n";
            std::cout << "  " << file2 << ": " << line2 << "\n";
            diff_count++;
        }
        if (diff_count >= max_diffs)
        {
            std::cout << "More differences exist...\n";
            break;
        }
        line_num++;
    }
    if (diff_count == 0)
        std::cout << "No differences found.\n";
}

int main()
{
    const std::string inputFile = "datasets/unsorted_dataset.txt";
    const std::string outputFile = "datasets/output_openMP.txt";
    const std::string referenceFile = "datasets/sorted_dataset.txt";

    std::vector<int> data = readInputFile(inputFile);

    std::cout << "Sorting " << data.size() << " elements in parallel using OpenMP...\n";

    auto start = high_resolution_clock::now();
#pragma omp parallel
    {
#pragma omp single
        mergeSort(data, 0, data.size() - 1);
    }
    auto end = high_resolution_clock::now();

    double duration = duration_cast<milliseconds>(end - start).count() / 1000.0;
    std::cout << "Time taken: " << duration << " seconds\n";

    writeOutputFile(outputFile, data);

    // Check correctness
    if (compareFiles(outputFile, referenceFile))
    {
        std::cout << "Output is correct: output matches sorted_dataset.txt\n";
    }
    else
    {
        std::cout << "Output is incorrect: output does not match sorted_dataset.txt\n";
        printFileDifferences(outputFile, referenceFile, 10);
    }

    return 0;
}
