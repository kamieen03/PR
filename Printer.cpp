#include "Printer.h"
void Printer::print(std::string msg, int nr){
    std::cout << "\033[0;3" << nr <<"m Process " << nr << ": " << msg << std::endl;
}

