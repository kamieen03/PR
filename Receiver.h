#pragma once
#include "Sender.h"
#include <mpi.h>


class Receiver {
private:
    int *permissions;
    int P;
    weaponType *wType;
	State *state;
	Sender *sender;
	bool isRunning;
	pthread_mutex_t *sleep_mutex;

    template <class T> void receive(T* data, MPI_Status* status);

    void handleWeaponRequest(WeaponRequest msg, int sourceRank);
    void handleWeaponPermission();
    void handleWeaponRelease(int sourceRank);

    void handleMedicRequest(MedicRequest msg, int sourceRank);
    void handleMedicPermission();
    void handleMedicRelease(int sourceRank);

    void handleCenterRequest(CenterRequest msg, int sourceRank);
    void handleCenterPermission(int weight);
    void handleCenterRelease(int sourceRank);

    void handleDeath(DeathMsg msg);

public:
    Receiver(int N, int *permissions, weaponType *wType, State *state, Sender *sender, pthread_mutex_t *sleep_mutex);
	void* run(void*);
    void stopReceiving();
};
