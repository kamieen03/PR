#include <vector>

enum weaponType {KARABIN, MIECZ};
enum state {NEW, WAITING_WEAPON, HUNTING, INJURED, DEAD, HOSPITALIZED, WAITING_CENTER};

class Hunter {
	const int K = 10;		//liczba karabinów
	const int M = 10;		//liczba mieczy
	const int S = 10;		//liczba sanitariuszy
	const int W = 10; 		//maksymalna waga bandersnatcha
	const int T = 100; 		//pojemnosc centrum

	int P;				//liczba żywych procesów
	int clock;			//zegar Lamporta
	enum weaponType weapon;		//aktualnie wybrany typ broni
	int permissions;		//liczba otrzymanych zgód
	enum state currentState;	//aktualny stan procesu

	std::vector<> 			//TODO: wektor na żądania o broń
					//TODO: wektor na żądania o sanitariusza
					//TODO: wektor na żądania o centrum	

	public:
		Hunter (int P);					//P - początkowa liczba procesów
		void decreaseLiving();				//zmniejsz liczbę żywych procesów	        
		void incrementClock();				//zwiększ zegar procesu o 1
		float getPriority();				//oblicz aktualny priorytet procesu
		enum weaponType drawNewWeaponType();		//wylosuj nowy typ broni
		void incPermissions();				//zwiększ liczbę zgód o 1
		void setState(enum state);				//ustaw stan
		void hunt();					//poluj

}
