#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <string>
#include "file_compare_utils.h"
#include "merge_utils.h"

using namespace std;
using namespace std::chrono;

vector<int> readInputFile(const string &filename)
{
    vector<int> data;
    ifstream infile(filename);
    int value;
    while (infile >> value)
    {
        data.push_back(value);
    }
    infile.close();
    return data;
}

void writeOutputFile(const string &filename, const vector<int> &data)
{
    ofstream outfile(filename);
    for (int num : data)
    {
        outfile << num << "\n";
    }
    outfile.close();
}

int main()
{
    const std::string inputFile = "datasets/unsorted_dataset.txt";
    const std::string outputFile = "datasets/output_serial.txt";
    const std::string referenceFile = "datasets/sorted_dataset.txt";

    std::vector<int> data = readInputFile(inputFile);

    std::cout << "Sorting " << data.size() << " elements...\n";

    auto start = std::chrono::high_resolution_clock::now();
    mergeSort(data, 0, data.size() - 1);
    auto end = std::chrono::high_resolution_clock::now();

    double duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() / 1000.0;
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
