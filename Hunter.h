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
	//stałe systemowe
	const int K = 10;		//liczba karabinów
	const int M = 10;		//liczba mieczy
	const int S = 10;		//liczba sanitariuszy
	const int W = 10; 		//maksymalna waga bandersnatcha
	const int T = 100; 		//pojemnosc centrum
	int N;				//początkowa liczba procesów

	//zmienne procesu
	int nr;				//numer procesu
	int P;				//liczba żywych procesów
	int clock;			//zegar Lamporta
	enum weaponType weapon;		//aktualnie wybrany typ broni
	int permissions;		//liczba otrzymanych zgód
	enum State currentState;	//aktualny stan procesu
	
	//listy na zigonorowane żądania (FIFO)
	std::list<std::pair<int, char>> ignoredWeaponRequests;	//wektor na żądania o broń (nr procesu i 'K' lub 'M')
	std::list<int> ignoredMedicRequests;			//wektor na żądania o sanitariusza (nr procesu)
	std::list<int> ignoredCenterRequests;			//wektor na żądania o centrum (nr procesu)
	
	std::pair<int, float>** centerRequests;			//tablica wskaźników na wszystkie aktualne żądania o centrum
								//	(waga bandersnatcha i priorytet procesu)
	std::default_random_engine randGenerator;		//prywatny geberator liczb pseudolowoych

	public:
		Sender* sender;		//klasa do sysyłania wiadomości prze wątek główny
		Receiver* receiver;	//wątek komunikacyjny procesu

		//metody zarządzajace polami danych obieku
		//nie powinny zmieniać pola currentState (wyjątek - konstruktor)
		Hunter(int N, int nr);					//nr procesu
		~Hunter(void);

		int getNr();

		void decreaseP();				//zmniejsz liczbę żywych procesów	        
		int getP();

		void incrementClock();				//zwiększ zegar procesu o 1
		int getClock();
		float getPriority();				//oblicz aktualny priorytet procesu

		weaponType drawNewWeaponType();			//wylosuj nowy typ broni
		void setWeapon(weaponType);
		weaponType getWeaponType();

		void incPermissions();				//zwiększ liczbę zgód o 1
		void resetPermissions();			//wyzeruj liczbę zgód
		int getPermissions();

		void ignoreWeaponRequest(int, char);		//ignoredWeaponRequests
		std::pair<int, char> deignoreWeaponRequest(int, char);
		void ignoreMedicRequest(int);			//ignoredMedicRequests
		int deignoreMedicRequest(int);
		void ignoreCenterRequest(int);			//ignoredCenterRequests
		int deignoreCenterRequest(int);
		void recordCenterRequest(int, int, float);	//centerRequests
		void forgetCenterRequest(int);

		//metody komunikacyjne i stanu
		//mogą zmieniać currentState
		void setState(State);				//ustaw stan
		State getState();				//pobierz aktualny stan
		void start();					//wykonaj przejście NEW -> WAITING_WEAPON,
	       							//	rozpocznij wątek komunikacyjny
		void requestWeapon();				//wyślij request o broń i czekaj na zgody
		State hunt();					//poluj
		void die();					//die
		void requestMedic();				//wyślij request o sanitariusza i czekaj na zgody
		void getHospitalized();				//sleep
		void requestCenter();				//wyślij request o centrum i czekaj na zgody
		
		//główna pętla procesu
		void mainLoop();
		//sleep na rand() czasu
		void randSleep();
		int randomWeight();	

};
