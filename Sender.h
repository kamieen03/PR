#pragma once
#include <mpi.h>
#include <stdio.h>
#include <random>
#include <list>
#include "types.h"

class Sender {
	int nr;		//numer procesu
	int N;		//liczba procesów w systemie
	int* clock;

	std::pair<int, float>** centerRequests;			//współdzileone z Hunter i Receiver


	void sendWeaponPermission(weaponType);		//wysyłane do jednego procesu wraz z WeponRelease
	void sendMedicPermission();				//wysyłane do jednego procesu wraz z MedicRelease
	void sendCenterPermission(int perm_weight);				//wysyłane do jednego procesu wraz z CenterRelease
	int countCenterPermissions(float p);			//zliczanie zaraz prezd wysłaniem CenterRequest

	int getNr() { return this -> nr; }
	int getN() { return this -> N; }
	void incClock();

	public:
		Sender(int size, int nr, std::list<std::pair<int, weaponType>>*, std::list<int>*, std::list<int>*,
				std::pair<int, float>**);
		void broadcastWeaponRequest(weaponType w, float p);
		void broadcastWeaponRelease(weaponType w);
		void broadcastMedicRequest(float p);
		void broadcastMedicRelease();
		int broadcastCenterRequest(int w, float p, int* permissions);
		void broadcastCenterRelease(int w);	
		void broadcastDeathMsg(weaponType w);

	std::list<std::pair<int, weaponType>>* ignoredWeaponRequests;	//współdzielone z Hunter i Receiver
	std::list<int>* ignoredMedicRequests;			//współdzielone z Hunter i Receiver
	std::list<int>* ignoredCenterRequests;			//współdzielone z Hunter i Receiver

};
