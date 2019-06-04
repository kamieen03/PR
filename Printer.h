#pragma once
#include <stdarg.h>
#include <stdio.h>
#include <string>
#include <iostream>
#include <list>
#include "types.h"
#include <list>

class Printer{
public:
    static void print(const std::list<std::string> args, int nr);
    static std::string state2str(enum State);
    static std::string weapon2str(enum weaponType);
};

