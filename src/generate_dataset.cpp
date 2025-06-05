#include <iostream>
#include <fstream>
#include <random>

int main() {
    const std::string filename = "datasets/input_large.txt";
    const size_t num_elements = 10000000; // 10 million
    const int min_value = 0;
    const int max_value = 1000000;

    std::ofstream outfile(filename);
    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open file for writing: " << filename << std::endl;
        return 1;
    }

    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> dis(min_value, max_value);

    for (size_t i = 0; i < num_elements; ++i) {
        outfile << dis(gen) << "\n";
    }

    outfile.close();
    std::cout << "Dataset generated successfully: " << filename << std::endl;
    return 0;
}
