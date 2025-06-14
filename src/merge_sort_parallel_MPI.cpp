#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <string>
#include <algorithm> // for std::remove_if, std::isspace

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

void mergeSort(vector<int> &arr, int left, int right)
{
    if (left >= right)
        return;
    int mid = left + (right - left) / 2;
    mergeSort(arr, left, mid);
    mergeSort(arr, mid + 1, right);
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

std::string trim(const std::string &s)
{
    auto start = s.begin();
    while (start != s.end() && std::isspace(*start))
        start++;
    auto end = s.end();
    do
    {
        end--;
    } while (std::distance(start, end) > 0 && std::isspace(*end));
    return std::string(start, end + 1);
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
        if (trim(line1) != trim(line2))
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
            std::cout << "Extra line in " << file2 << " at line " << line_num << ": " << trim(line2) << "\n";
            diff_count++;
        }
        else if (!f2_has_line)
        {
            std::cout << "Extra line in " << file1 << " at line " << line_num << ": " << trim(line1) << "\n";
            diff_count++;
        }
        else if (trim(line1) != trim(line2))
        {
            std::cout << "Difference at line " << line_num << ":\n";
            std::cout << "  " << file1 << ": " << trim(line1) << "\n";
            std::cout << "  " << file2 << ": " << trim(line2) << "\n";
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

int main(int argc, char *argv[])
{
    MPI_Init(&argc, &argv);
    int rank, size;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    const std::string inputFile = "datasets/unsorted_dataset.txt";
    const std::string outputFile = "datasets/output_MPI.txt";
    const std::string referenceFile = "datasets/sorted_dataset.txt";

    std::vector<int> data;
    int n = 0;

    if (rank == 0)
    {
        data = readInputFile(inputFile);
        n = data.size();
        std::cout << "Sorting " << n << " elements in parallel using " << size << " processes...\n";
    }

    // Broadcast the size to all processes
    MPI_Bcast(&n, 1, MPI_INT, 0, MPI_COMM_WORLD);
    int local_n = n / size;
    int remainder = n % size;
    std::vector<int> local_data(local_n + (rank < remainder ? 1 : 0));

    // Calculate send counts and displacements
    std::vector<int> sendcounts(size), displs(size);
    int sum = 0;
    for (int i = 0; i < size; ++i)
    {
        sendcounts[i] = n / size + (i < remainder ? 1 : 0);
        displs[i] = sum;
        sum += sendcounts[i];
    }

    // Scatter the data
    MPI_Scatterv(data.data(), sendcounts.data(), displs.data(), MPI_INT,
                 local_data.data(), local_data.size(), MPI_INT, 0, MPI_COMM_WORLD);

    auto start = high_resolution_clock::now();
    // Each process sorts its local data
    mergeSort(local_data, 0, local_data.size() - 1);

    // Gather the sorted subarrays
    MPI_Gatherv(local_data.data(), local_data.size(), MPI_INT,
                data.data(), sendcounts.data(), displs.data(), MPI_INT, 0, MPI_COMM_WORLD);
    auto end = high_resolution_clock::now();

    if (rank == 0)
    {
        // Merge the sorted subarrays
        int curr_size = sendcounts[0];
        for (int i = 1; i < size; ++i)
        {
            merge(data, 0, curr_size - 1, curr_size + sendcounts[i] - 1);
            curr_size += sendcounts[i];
        }
        double duration = duration_cast<milliseconds>(end - start).count() / 1000.0;
        std::cout << "Time taken: " << duration << " seconds\n";
        writeOutputFile(outputFile, data);
        // Check correctness
        if (compareFiles(outputFile, referenceFile))
            std::cout << "Output is correct: output matches sorted_dataset.txt\n";
        else
        {
            std::cout << "Output is incorrect: output does not match sorted_dataset.txt\n";
            printFileDifferences(outputFile, referenceFile, 10);
        }
    }
    MPI_Finalize();
    return 0;
}
