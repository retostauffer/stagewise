
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

void fn(std::string file, int a, int b) {

    std::cout << "file: " << file << "\n";

}

int main() {
    std::string file = "test.txt";
    fn(file, 3, 10);

    //double i = 3.5343;
    int i = 123456789;
    int nchar = file.size();
    int nbyte = sizeof(file);


    std::cout << "\n\n ----- writing ---------- \n";
    std::ofstream out("_test.bin", ios::out | ios::binary);

    std::cout << "nchar:   " << nchar << "\n";
    out.write(reinterpret_cast<char*>(&nchar), 4);

    std::cout << "nbyte:   " << nbyte << "\n";
    out.write(reinterpret_cast<char*>(&nbyte), 4);

    std::cout << "file:    " << file << " with " << file.size() + 1 << "bytes\n";
    out.write(file.c_str(), nbyte);

    std::cout << "i:       " << i << "\n";
    out.write(reinterpret_cast<char*>(&i), 4);
    out.close();


    int i_nchar, i_nbyte;
    //double i_i;
    int i_i;

    std::cout << "\n ------ reading ------ \n";
    std:ifstream in("_test.bin", ios::in | ios::binary);

    in.read(reinterpret_cast<char*>(&i_nchar), 4);
    std::cout << "i_nchar: " << i_nchar << "\n";

    in.read(reinterpret_cast<char*>(&i_nbyte), 4);
    std::cout << "i_nbyte: " << i_nbyte << "\n";

    // xxx
    char bffr[i_nchar + 1];
    in.read(bffr, i_nbyte);
    bffr[i_nchar] = '\0'; // Adding zero bit;
    auto i_file = std::string();
    i_file = string(bffr);

    std::cout << "i_file:  `" << i_file << "`\n";

    in.read(reinterpret_cast<char*>(&i_i), 4);
    std::cout << "i_i:     " << i_i << " (originally " << i << ")\n";

    in.close();
}

