#pragma once
#include <mpi.h>
#include <stdio.h>
#include <random>
#include <list>
#include "types.h"

class Sender {
private:
	int nr;		//numer procesu
	int N;		//liczba procesów w systemie
	int clock;

	//listy na zigonorowane żądania (FIFO)
	std::list<std::pair<int, weaponType>>* ignoredWeaponRequests;   //wektor na żądania o broń (nr procesu i 'K' lub 'M')
	std::list<int>* ignoredMedicRequests;                           //wektor na żądania o sanitariusza (nr procesu)
    std::list<int>* ignoredCenterRequests;                          //wektor na żądania o centrum (nr procesu)
    std::pair<int, float>** centerRequests;                         //tablica wskaźników na wszystkie aktualne żądania o centrum
                                                                    //(waga bandersnatcha i priorytet procesu)
    //mutexy na powyższe listy
    pthread_mutex_t iwrMutex;
    pthread_mutex_t imrMutex;
    pthread_mutex_t icrMutex;
    pthread_mutex_t crMutex;
    pthread_mutex_t clockMutex;


public:
	void sendWeaponPermission(weaponType, int nr);		    //wysyłane do jednego procesu wraz z WeponRelease
	void sendMedicPermission(int nr);				        //wysyłane do jednego procesu wraz z MedicRelease
	void sendCenterPermission(int perm_weight, int nr);		//wysyłane do jednego procesu wraz z CenterRelease
	int countCenterPermissions(float p);			//zliczanie zaraz prezd wysłaniem CenterRequest

	int getNr() { return this -> nr; }
	int getN() { return this -> N; }
    void setCenterRequest(int nr, std::pair<int, float> req);
	void incClock();
	void getClock();
	double getPriority();

	Sender(int size, int nr);
	void broadcastWeaponRequest(weaponType w, float p);
	void broadcastWeaponRelease(weaponType w);
	void broadcastMedicRequest(float p);
	void broadcastMedicRelease();
	int broadcastCenterRequest(int w, float p, int* permissions);
	void broadcastCenterRelease(int w);
	void broadcastDeathMsg(weaponType w);

    void ignoreWeaponRequest(std::pair<int, weaponType> req);
    void ignoreMedicRequest(int nr);
    void ignoreCenterRequest(int nr);
    void setCenterRequest(int nr, std::pair<int, float> req);

    void removeIgnoredWeaponRequest(int nr);
    void removeIgnoredMedicRequest(int nr);
    void removeIgnoredCenterRequest(int nr);
};
