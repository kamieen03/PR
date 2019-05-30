#pragma once
#include "types.h"
#include "Hunter.h"
#include <mpi.h>


class Receiver {
private:
    int *permissions;
    int P;
    int *weaponCount;
    weaponType *wType;
	State *state;
	Sender *sender;
	bool isRunning;

    template <class T> void receive(T* data, MPI_Status* status);
    void stopReceiving();

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
    Receiver(int N, int *permissions, weaponType *wType, int *weaponCount, State *state, Sender *sender);
	void run();
};
