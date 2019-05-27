#pragma once
#include <mpi.h>
#include <stdio.h>
#include <random>
#include <list>
#include "types.h"

class Sender {
	int nr;		//numer procesu
	int N;		//liczba procesów w systemie
	int clock;


        //listy na zigonorowane żądania (FIFO)
        std::list<std::pair<int, weaponType>>* ignoredWeaponRequests;   //wektor na żądania o broń (nr procesu i 'K' lub 'M')
        std::list<int>* ignoredMedicRequests;                           //wektor na żądania o sanitariusza (nr procesu)
        std::list<int>* ignoredCenterRequests;                          //wektor na żądania o centrum (nr procesu)

        std::pair<int, float>** centerRequests;                         //tablica wskaźników na wszystkie aktualne żądania o centrum
                                                                        //      (waga bandersnatcha i priorytet procesu)

        pthread_mutex_t* iwrMutex;                              //mutexy na powyższe listy
        pthread_mutex_t* imrMutex;                              //
        pthread_mutex_t* icrMutex;                              //
        pthread_mutex_t* crMutex;                               //


	public:

	void sendWeaponPermission(weaponType);		//wysyłane do jednego procesu wraz z WeponRelease
	void sendMedicPermission();				//wysyłane do jednego procesu wraz z MedicRelease
	void sendCenterPermission(int perm_weight);				//wysyłane do jednego procesu wraz z CenterRelease
	int countCenterPermissions(float p);			//zliczanie zaraz prezd wysłaniem CenterRequest

	int getNr() { return this -> nr; }
	int getN() { return this -> N; }
	void incClock();
	void getClock();
	double gerPriority();

		Sender(int size, int nr);
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
