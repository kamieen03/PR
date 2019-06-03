#include "Printer.h"
void Printer::print(std::string msg, int nr){
    std::cout << "\033[1;3" << nr <<"m Process " << nr << ": " << msg << std::endl;
}

std::string Printer::state2str(enum State val){
    std::string s[] = {"NEW",
    "WAITING_WEAPON",
    "HUNTING",
    "INJURED",
    "DEAD",
    "HOSPITALIZED",
    "WAITING_CENTER",
    "IN_CENTER"
    };
    return s[val];
}
