#pragma once
#include <random>

//deklaracje typów
enum weaponType {NONE, KARABIN, MIECZ};
enum State {NEW, WAITING_WEAPON, HUNTING, INJURED, DEAD, HOSPITALIZED, WAITING_CENTER, IN_CENTER};


//weapon
typedef struct WeaponRequest {
        weaponType w;   //rodzaj broni
        float p;        //priorytet
} WeaponRequest;

typedef struct WeaponRelease {
	weaponType w;
} WeaponRelease;


//medic
typedef struct MedicRequest {
        float p;        //priorytet
} MedicRequest;

typedef struct MedicRelease {
} MedicRelease;


//center
typedef struct CenterRequest {
        int w;          //waga bandersnatcha
        float p;        //priorytet
} CenterRequest;

typedef struct CenterRelease {
	int w;
} CenterRelease;


//death
typedef struct DeathMsg {
	weaponType w;
} DeathMsg;


//deklaracje kodowań wiadomości
const int W_REQ = 100;
const int W_REL = 101;
const int W_PER = 102;
const int M_REQ = 200;
const int M_REL = 201;
const int M_PER = 202;
const int C_REQ = 300;
const int C_REL = 301;
const int C_PER = 302;
const int DEATH = 404;



//stałe systemowe
const int K_MAX = 2;	//liczba karabinów
const int M_MAX = 2;	//liczba mieczy
const int S_MAX = 4;	//liczba sanitariuszy
const int W_MAX = 10;	//maksymalna waga bandersnatcha
const int T_MAX = 20;	//pojmeność centrum


//rozkład prawdopodobieństwa dla polowania


