#include "Printer.h"
void Printer::print(const std::list<std::string> &args, int nr){
    std::cout << "\033[1;3" << nr <<"mProcess " << nr << ": ";
    for(auto &msg: args)
        std::cout << msg << " ";
    std::cout << std::endl;
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

std::string Printer::weapon2str(enum weaponType val){
    std::string s[] = {"NONE",
    "KARABIN",
    "MIECZ"
    };
    return s[val];
}
