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

    template <class T> void receive(T* data, MPI_Status* status);
    void handleWeaponRequest(WeaponRequest msg, int sourceRank);
    void handleWeaponRelease(WeaponRelease msg);
    void handleMedicRequest(MedicRequest msg, int sourceRank);
    void handleMedicRelease(MedicRelease msg);
    void handleCentrumRequest(CenterRequest msg, int sourceRank);
    void handleCentrumRelease(CenterRequest msg);
    void handleDeath(DeathMsg msg);

public:
    Receiver(int N, int *permissions, weaponType *wType, int *weaponCount, State *state, Sender *sender);
	void run();
};
