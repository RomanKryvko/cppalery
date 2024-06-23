#include <iostream>
#include "form.h"

int main (int argc, char** argv) {
    setlocale(LC_ALL, "");

    if (argc < 2) {
        printf("Argument not provided. Provide a directory path.\n");
        return 1;
    }

    fs::path path;

    try {
        path = fs::canonical(argv[1]);
    }
    catch (const std::exception &ex) {
        std::cerr << ex.what() << "\n";
    }

    if (!fs::exists(path)) {
        printf("Provided path not valid.\n");
        return 1;
    }

    Form form = Form(path.string());
    form.loopOptions();

    return 0;
}