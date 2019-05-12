#pragma once
#include "types.h"
#include "Hunter.h"
#include <mpi.h>


class Receiver {
private:
	Hunter * hunter;

    template <class T> void receive(T* data, MPI_Status* status);
    void handleWeaponRequest(WeaponRequest *msg);
    void handleWeaponRelease(WeaponRelease *msg);
    void handleMedicRequest(MedicRequest *msg);
    void handleMedicRelease(MedicRelease *msg);
    void handleCentrumRequest(CenterRequest *msg);
    void handleCentrumRelease(CenterRequest *msg);
    void handelDeath(DeathMsg *msg);

public:
    explicit Receiver(Hunter *hunter);
	void run();
};
