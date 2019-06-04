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
    void handleWeaponPermission(WeaponPermission);
    void handleWeaponRelease(WeaponRelease, int sourceRank);

    void handleMedicRequest(MedicRequest msg, int sourceRank);
    void handleMedicPermission(MedicPermission);
    void handleMedicRelease(MedicRelease, int sourceRank);

    void handleCenterRequest(CenterRequest msg, int sourceRank);
    void handleCenterPermission(CenterPermission);
    void handleCenterRelease(CenterRelease, int sourceRank);

    void handleDeath(DeathMsg msg);

public:
    Receiver(int N, int *permissions, weaponType *wType, State *state, Sender *sender, pthread_mutex_t *sleep_mutex);
	void* run(void*);
    void stopReceiving();
};
