#include <mpi.h>
#include <stdio.h>
#include <random>
#include "types.h"

class Sender {
	int nr;		//numer procesu
	int size;	//liczba procesów w systemie

	public:
		Sender(int size, int nr);
		void broadcastWeaponRequest(weaponType w, float p);
		void broadcastWeaponRelease(weaponType w);
		void broadcastMedicRequest(float p);
		void broadcastMedicRelease();
		void broadcastCenterRequest(int w, float p);
		void broadcastCenterRelease(int w);	
		void broadcastDeathMsg();

		int getNr() { return this -> nr; }
		int getSize() { return this -> size; }
};
