#include <iostream>
#include <fstream>
#include <random>
#include <vector>
#include <algorithm>

int main()
{
    const std::string sorted_filename = "datasets/sorted_dataset.txt";
    const std::string unsorted_filename = "datasets/unsorted_dataset.txt";
    const size_t num_elements = 10000000;

    std::vector<int> data(num_elements);
    for (size_t i = 0; i < num_elements; ++i)
    {
        data[i] = static_cast<int>(i + 1);
    }

    std::ofstream sorted_out(sorted_filename);
    if (!sorted_out.is_open())
    {
        std::cerr << "Error: Unable to open file for writing: " << sorted_filename << std::endl;
        return 1;
    }
    for (size_t i = 0; i < num_elements; ++i)
    {
        sorted_out << data[i] << "\n";
    }
    sorted_out.close();

    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(data.begin(), data.end(), gen);

    std::ofstream unsorted_out(unsorted_filename);
    if (!unsorted_out.is_open())
    {
        std::cerr << "Error: Unable to open file for writing: " << unsorted_filename << std::endl;
        return 1;
    }
    for (size_t i = 0; i < num_elements; ++i)
    {
        unsorted_out << data[i] << "\n";
    }
    unsorted_out.close();

    std::cout << "Datasets generated successfully: " << sorted_filename << " and " << unsorted_filename << std::endl;
    return 0;
}
