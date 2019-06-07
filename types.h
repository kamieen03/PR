#pragma once
#include <random>

//deklaracje typów
enum weaponType {NONE, KARABIN, MIECZ};
enum State {NEW, WAITING_WEAPON, HUNTING, INJURED, DEAD, HOSPITALIZED, WAITING_CENTER, IN_CENTER};


typedef struct MSG {
    weaponType w;   //broń
    float p;        //priorytet
    int weight;     //waga bandersnatcha
    int permission_weight;  //waga pozwoloenia dla CenterPermission
    float req_p;              //wykorzystywane przy wiadomości zwrotnej - priorytet oryginalnej wiadomości
} MSG;

const int MSG_SIZE = sizeof(MSG);


//deklaracje kodowań wiadomości
const int W_REQ = 100;
const int W_PER = 101;
const int M_REQ = 200;
const int M_PER = 201;
const int C_REQ = 300;
const int C_PER = 301;
const int C_REL = 302;
const int DEATH = 404;



//stałe systemowe
const int K_MAX = 3;	//liczba karabinów
const int M_MAX = 2;	//liczba mieczy
const int S_MAX = 4;	//liczba sanitariuszy
const int W_MAX = 11;	//maksymalna waga bandersnatcha
const int T_MAX = 20;	//pojemność centrum

