#include "Printer.h"
static void print(std::string msg, int nr){
    std::cout << "\033[03" << nr <<"mProcess " << nr << ": " << msg << std::endl;
}
