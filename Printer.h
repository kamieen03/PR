#pragma once
#include <stdio.h>
#include <string>
#include <iostream>

class Printer{
    //Black        "\033[0;30m"
    //Red          "\033[0;31m"
    //Green        "\033[0;32m"
    //Brown/Orange "\033[0;33m"
    //Blue         "\033[0;34m"
    //Purple       "\033[0;35m"
    //Cyan         "\033[0;36m"
    //Light Gray   "\033[0;37m"

    static void print(std::string msg, int nr);
};

