#pragma once
#include <mpi.h>
#include <stdio.h>
#include <random>
#include <list>
#include "types.h"
#include <pthread.h>
#include "Printer.h"

class Sender {
private:
	int nr;		//numer procesu
	int N;		//liczba procesów w systemie
	int clock;
    double current_req_p;

	//listy na zigonorowane żądania (FIFO)
	std::list<std::pair<int, float>>* ignoredWeaponRequests;   //wektor na żądania o broń (nr procesu i 'K' lub 'M')
	std::list<std::pair<int, float>>* ignoredMedicRequests;                           //wektor na żądania o sanitariusza (nr procesu)
    std::list<std::pair<int, float>>* ignoredCenterRequests;                          //wektor na żądania o centrum (nr procesu)
    std::pair<int, float>** centerRequests;                         //tablica wskaźników na wszystkie aktualne żądania o centrum
                                                                    //(waga bandersnatcha i priorytet procesu)
    //mutexy na powyższe listy
    pthread_mutex_t iwrMutex;
    pthread_mutex_t imrMutex;
    pthread_mutex_t icrMutex;
    pthread_mutex_t crMutex;
    pthread_mutex_t clockMutex;
    pthread_mutex_t current_req_p_mutex;

    void broadcastWeaponPermission();
    void broadcastMedicPermission();
    void broadcastCenterPermission(int);


public:
    pthread_mutex_t mpi_mutex;
    bool waitingForPermissions;

	void sendWeaponPermission(int, float);		    //wysyłane do jednego procesu wraz z WeponRelease
	void sendMedicPermission(int, float);				        //wysyłane do jednego procesu wraz z MedicRelease
	void sendCenterPermission(int perm_weight, int nr, float);	    //wysyłane do jednego procesu wraz z CenterRelease
	int countCenterPermissions(float p);			//zliczanie zaraz prezd wysłaniem CenterRequest

	int getNr() { return this -> nr; }
	int getN() { return this -> N; }
	void incClock();
    void setClock(float p);
	int getClock();
	double getPriority();
    double getCurrentReqP();
    void lock_current_req_p(bool);

	Sender(int size, int nr);
	void broadcastWeaponRequest(weaponType w);
	void broadcastWeaponRelease(weaponType w);
	void broadcastMedicRequest();
	void broadcastMedicRelease();
	int broadcastCenterRequest(int w, int* permissions);
	void broadcastCenterRelease(int w);
	void broadcastDeathMsg(weaponType w);

    void ignoreWeaponRequest(std::pair<int, float> req);
    void ignoreMedicRequest(std::pair<int, float> req);
    void ignoreCenterRequest(std::pair<int, float> req);
    void setCenterRequest(int nr, std::pair<int, float> req);

    void removeCenterRequest(int nr);
};
