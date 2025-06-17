#ifndef FILE_COMPARE_UTILS_H
#define FILE_COMPARE_UTILS_H

#include <fstream>
#include <iostream>
#include <string>
#include <algorithm> // for std::isspace

// Trims leading and trailing whitespace from a string
inline std::string trim(const std::string &s)
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

// Compares two files line by line, ignoring leading/trailing whitespace
inline bool compareFiles(const std::string &file1, const std::string &file2)
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

// Prints up to max_diffs differences between two files
inline void printFileDifferences(const std::string &file1, const std::string &file2, int max_diffs = 10)
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

#endif // FILE_COMPARE_UTILS_H
