#pragma once

//deklaracje typów
enum weaponType {NONE, KARABIN, MIECZ};
enum State {NEW, WAITING_WEAPON, HUNTING, INJURED, DEAD, HOSPITALIZED, WAITING_CENTER};


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
} DeathMsg;


//deklaracje kodowań wiadomości
const int W_REQ = 100;
const int W_REL = 101;
const int M_REQ = 200;
const int M_REL = 201;
const int C_REQ = 300;
const int C_REL = 301;
const int DEATH = 404;

//rozkład prawdopodobieństwa dla polowania
//(kolejne stany typu State)
std::discrete_distribution<int> HUNT_DISTRIBUTION {0, 0, 0, 15, 5, 0, 80};


