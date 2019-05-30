#pragma once
#include <stdio.h>
#include <list>
#include <utility>
#include <random>
#include <time.h>
#include <mpi.h>
#include <pthread.h>
#include <math.h>
#include <unistd.h>
#include "Sender.h"
#include "Receiver.h"
#include "types.h"


class Hunter {
	int N;				        //początkowa liczba procesów
	int nr;				        //numer procesu
	int* clock;			        //wkaźnik na zegar Lamporta
	enum weaponType weapon;		//aktualnie wybrany typ broni
	int* permissions;		    //wskaźnik na liczbę otrzymanych zgód
	enum State currentState;	//aktualny stan procesu

    Sender* sender;		        //klasa do wysyłania wiadomości
    Receiver* receiver;	        //wątek komunikacyjny procesu

    std::default_random_engine randGenerator;		//prywatny geberator liczb pseudolowoych

	weaponType drawNewWeaponType();			        //wylosuj nowy typ broni
	void setWeapon(weaponType);

	//metody komunikacyjne i stanu
	//mogą zmieniać currentState
	void setState(State);				//ustaw stan
	void start();					    //wykonaj przejście NEW -> WAITING_WEAPON i rozpocznij wątek komunikacyjny

	void requestWeapon();				//wyślij request o broń i czekaj na zgody
	State hunt();					    //poluj
	void die();					        //die
	void requestMedic();				//wyślij request o sanitariusza i czekaj na zgody
	void getHospitalized();				//sleep
	void requestCenter();				//wyślij request o centrum i czekaj na zgody

	void randSleep();                   //sleep na rand() czasu
	int randomWeight();
	

public:
    Hunter(int N, int nr);				//początkowa liczba procesów, nr procesu
    void mainLoop();                    //główna pętla procesu
};
