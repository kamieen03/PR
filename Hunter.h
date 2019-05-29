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

//enum State {NEW, WAITING_WEAPON, HUNTING, INJURED, DEAD, HOSPITALIZED, WAITING_CENTER};
//std::discrete_distribution<int> HUNT_DISTRIBUTION {0, 0, 0, 15, 5, 0, 80};

class Hunter {
	int N;				//początkowa liczba procesów

	//zmienne procesu
	int nr;				//numer procesu
	int P;				//liczba żywych procesów
	int* clock;			//wkaźnik na zegar Lamporta
	enum weaponType weapon;		//aktualnie wybrany typ broni
	int* permissions;		//wskaźnik na liczbę otrzymanych zgód
	enum State currentState;	//aktualny stan procesu
	
	//listy na zigonorowane żądania (FIFO)
	std::list<std::pair<int, weaponType>>* ignoredWeaponRequests;	//wektor na żądania o broń (nr procesu i 'K' lub 'M')
	std::list<int>* ignoredMedicRequests;			//wektor na żądania o sanitariusza (nr procesu)
	std::list<int>* ignoredCenterRequests;			//wektor na żądania o centrum (nr procesu)
	
	std::pair<int, float>** centerRequests;			//tablica wskaźników na wszystkie aktualne żądania o centrum
								//	(waga bandersnatcha i priorytet procesu)
	std::default_random_engine randGenerator;		//prywatny geberator liczb pseudolowoych

	Sender* sender;		//klasa do sysyłania wiadomości prze wątek główny
	Receiver* receiver;	//wątek komunikacyjny procesu

	//metody zarządzajace polami danych obieku
	//nie powinny zmieniać pola currentState

	int getNr();

	void decreaseP();				//zmniejsz liczbę żywych procesów	        
	int getP();

	void incrementClock();				//zwiększ zegar procesu o 1
	int getClock();

	weaponType drawNewWeaponType();			//wylosuj nowy typ broni
	void setWeapon(weaponType);


	void resetPermissions();			//wyzeruj liczbę zgód

	//TODO: te funkcje powinny zostać zaimplementowane w klasie Receiver
	//void ignoreWeaponRequest(int, char);		//ignoredWeaponRequests
	//void ignoreMedicRequest(int);			//ignoredMedicRequests
	//void ignoreCenterRequest(int);			//ignoredCenterRequests
	//void recordCenterRequest(int, int, float);	//centerRequests
	//void forgetCenterRequest(int);


	//metody komunikacyjne i stanu
	//mogą zmieniać currentState
	void setState(State);				//ustaw stan
	void start();					//wykonaj przejście NEW -> WAITING_WEAPON,
							//	rozpocznij wątek komunikacyjny
	void requestWeapon();				//wyślij request o broń i czekaj na zgody
	State hunt();					//poluj
	void die();					//die
	void requestMedic();				//wyślij request o sanitariusza i czekaj na zgody
	void getHospitalized();				//sleep
	void requestCenter();				//wyślij request o centrum i czekaj na zgody
	
	//sleep na rand() czasu
	void randSleep();
	int randomWeight();
	

public:
    Hunter(int N, int nr);				//początkowa liczba procesów, nr procesu
    //główna pętla procesu
    void mainLoop();
    weaponType getWeaponType();
    float getPriority();                //oblicz aktualny priorytet procesu
    State getState();				//pobierz aktualny stan
    Sender* getSender();
    std::list<std::pair<int, weaponType>>* getIgnoredWeaponReq;
};
