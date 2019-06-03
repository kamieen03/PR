#pragma once
#include <stdio.h>
#include <string>
#include <iostream>
#include "types.h"

class Printer{
public:
    static void print(std::string msg, int nr);
    static std::string state2str(enum State);
};

