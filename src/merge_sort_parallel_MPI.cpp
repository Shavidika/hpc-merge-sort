#include <mpi.h>
#include <iostream>
#include <vector>
#include <fstream>
#include <chrono>
#include <string>
#include <algorithm> // for std::remove_if, std::isspace
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
