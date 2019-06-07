#pragma once
#include "Sender.h"
#include <mpi.h>


class Receiver {
private:
    int *permissions;
    weaponType *wType;
	State *state;
	Sender *sender;
	bool isRunning;
	pthread_mutex_t *sleep_mutex;
	pthread_mutex_t STATE_MUTEX; 
	pthread_mutex_t WEAPON_MUTEX;
	pthread_mutex_t P_MUTEX;

    void receive(MSG* data, MPI_Status* status);

    void handleWeaponRequest(MSG msg, int sourceRank);
    void handleWeaponPermission(MSG);
    //void handleWeaponRelease(MSG, int sourceRank);

    void handleMedicRequest(MSG msg, int sourceRank);
    void handleMedicPermission(MSG);
    //void handleMedicRelease(MSG, int sourceRank);

    void handleCenterRequest(MSG msg, int sourceRank);
    void handleCenterPermission(MSG, int);
    void handleCenterRelease(MSG, int sourceRank);

    void handleDeath(MSG msg);

public:
    int P;

    Receiver(int N, int *permissions, weaponType *wType, State *state, Sender *sender, pthread_mutex_t *sleep_mutex);
	void* run(void*);
    void stopReceiving();

    void lock_state(bool);
    void lock_weapon(bool);
    void lockP(bool);
};
