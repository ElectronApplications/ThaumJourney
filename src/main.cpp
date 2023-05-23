#include <iostream>
#include <fstream>
#include <thread>

#include "io.h"
#include "location.h"

using namespace std;

int main(int argc, char **argv) {
    srand(time(nullptr));
    
    ifstream about_file("resources/about.txt");
    string texta((istreambuf_iterator<char>(about_file)), (istreambuf_iterator<char>()));
    about_text = texta;

    ifstream reference_file("resources/reference.txt");
    string textr((istreambuf_iterator<char>(reference_file)), (istreambuf_iterator<char>()));
    reference_text = textr;

    thread io(io_thread); // Создаем и запускаем основной поток игры
    io.join();

    return 0;
}