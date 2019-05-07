#include <list>
#include <utility>
#include <random>

enum weaponType {NONE, KARABIN, MIECZ};
enum State {NEW, WAITING_WEAPON, HUNTING, INJURED, DEAD, HOSPITALIZED, WAITING_CENTER};
//std::discrete_distribution<int> HUNT_DISTRIBUTION {0, 0, 0, 15, 5, 0, 80};

class Hunter {
	//stałe systemowe
	const int K = 10;		//liczba karabinów
	const int M = 10;		//liczba mieczy
	const int S = 10;		//liczba sanitariuszy
	const int W = 10; 		//maksymalna waga bandersnatcha
	const int T = 100; 		//pojemnosc centrum
	const int N = 100;		//początkowa liczba procesów

	//zmienne procesu
	int nr;				//numer procesu
	int P;				//liczba żywych procesów
	int clock;			//zegar Lamporta
	enum weaponType weapon;		//aktualnie wybrany typ broni
	int permissions;		//liczba otrzymanych zgód
	enum State currentState;	//aktualny stan procesu
	
	//wektory na zigonorowane żądania (FIFO(?))
	std::list<std::pair<int, char>> ignoredWeaponRequests;	//wektor na żądania o broń (nr procesu i 'K' lub 'M')
	std::list<int> ignoredMedicRequests;			//wektor na żądania o sanitariusza (nr procesu)
	std::list<int> ignoredCenterRequests;			//wektor na żądania o centrum (nr procesu)

	std::pair<int, float>** centerRequests;			//tablica wskaźników na wszystkie aktualne żądania o centrum
								//	(waga bandersnatcha i priorytet procesu)
	std::default_random_engine randGenerator;		//prywatny geberator liczb pseudolowoych

	public:
		Hunter(int nr);					//nr procesu
		~Hunter(void);

		int getNr();

		void decreaseP();				//zmniejsz liczbę żywych procesów	        
		int getP();

		void incrementClock();				//zwiększ zegar procesu o 1
		int getClock();
		float getPriority();				//oblicz aktualny priorytet procesu

		weaponType drawNewWeaponType();			//wylosuj nowy typ broni
		weaponType getWeaponType();

		void incPermissions();				//zwiększ liczbę zgód o 1
		int getPermissions();

		void setState(State);				//ustaw stan
		State getState();

		void ignoreWeaponRequest(int, char);		//ignoredWeaponRequests
		std::pair<int, char> deignoreWeaponRequest(int, char);
		void ignoreMedicRequest(int);			//ignoredMedicRequests
		int deignoreMedicRequest(int);
		void ignoreCenterRequest(int);			//ignoredCenterRequests
		int deignoreCenterRequest(int);
		void recordCenterRequest(int, int, float);	//centerRequests
		void forgetCenterRequest(int);

		State hunt();					//poluj

};
