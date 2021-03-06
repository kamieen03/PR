#pragma once
#include <stdio.h>
#include <list>
#include <utility>
#include <time.h>
#include <mpi.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include "Receiver.h"


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
    std::discrete_distribution<int> HUNT_DISTRIBUTION {0, 0, 0, 15, 10, 0, 75};
    pthread_mutex_t sleep_mutex;

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
	int requestCenter();				//wyślij request o centrum i czekaj na zgody
    void visitCenter(int w);            //idź na jakiś czas do centrum

	void randSleep();                   //sleep na rand() czasu
	int randomWeight();
	

public:
    Hunter(int N, int nr);				//początkowa liczba procesów, nr procesu
    void mainLoop();                    //główna pętla procesu
};
