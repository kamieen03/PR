#include "Receiver.h"


Receiver::Receiver(Hunter * hunter){
    this->hunter = hunter;
}

void Receiver::run(){
    MPI_Status status;
    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    switch(status.MPI_TAG) {
        case W_REQ:
            WeaponRequest wReq;
            this -> receive(&wReq, &status);
            break;
        case W_REL:
            WeaponRelease wRel;
            this -> receive(&wRel, &status);
            break;
        case W_PER:
            //TODO
            break;
        case M_REQ:
            MedicRequest mReq;
            this -> receive(&mReq, &status);
            break;
        case M_REL:
            MedicRelease mRel;
            this -> receive(&mRel, &status);
            break;
        case M_PER:
            //TODO
            break;
        case C_REQ:
            CenterRequest cReq;
            this -> receive(&cReq, &status);
            break;
        case C_REL:
            CenterRelease cRel;
            this -> receive(&cRel, &status);
            break;
        case C_PER:
            //TODO
            break;
        case DEATH:
            DeathMsg dMsg;
            this -> receive(&dMsg, &status);
            break;
    }
}

template <class T>void Receiver::receive(T* data, MPI_Status* status) {
    MPI_Recv(data, sizeof *data, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
}

void Receiver::handleWeaponRequest(WeaponRequest *msg) {

}

void Receiver::handleWeaponRelease(WeaponRelease *msg){

}

void Receiver::handleMedicRequest(MedicRequest *msg) {

}

void Receiver::handleMedicRelease(MedicRelease *msg) {

}

void Receiver::handleCentrumRequest(CenterRequest *msg) {

}

void Receiver::handleCentrumRelease(CenterRequest *msg) {

}

void Receiver::handelDeath(DeathMsg *msg) {

}








