#include "Receiver.h"


Receiver::Receiver(int N, int *permissions, weaponType *wType, int *weaponCount, State *state, Sender *sender){
    this->P = N;
    this->permissions = permissions;
    this->wType = wType;
    this->weaponCount = weaponCount;
    this->state = state;
    this->sender = sender;
}

void Receiver::run(){
    MPI_Status status;
    MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

    switch(status.MPI_TAG) {
        case W_REQ:
            WeaponRequest wReq;
            this -> receive(&wReq, &status);
            handleWeaponRequest(wReq, status.MPI_SOURCE);
            break;
        case W_REL:
            WeaponRelease wRel;
            this -> receive(&wRel, &status);
            break;
        case W_PER:
            int wPer;
            this -> receive(&wPer, &status);
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
            int mPer;
            this -> receive(&mPer, &status);
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
            int cPer;
            this -> receive(&cPer, &status)
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

void Receiver::handleWeaponRequest(WeaponRequest msg, int sourceRank) {
    double priority = this->sender->getPriority();

    if(((*(this->state)) == WAITING_WEAPON and msg.p > priority ) or *(this->state) != WAITING_WEAPON){
        this->sender->sendWeaponPermission(msg.w, sourceRank);
    }
    else if((*(this->state) == WAITING_WEAPON and msg.p < priority)) {
        this->sender->ignoredWeaponRequests->push_back(std::make_pair(sourceRank, msg.w));
    }
}

void Receiver::handleWeaponRelease(WeaponRelease msg) {
    (*permissions)++;

    if(*permissions == this->P - (*this->weaponCount)) {
        //TODO
        *permissions = 0;
    }
}

void Receiver::handleMedicRequest(MedicRequest msg, int sourceRank) {
    double priority = this->sender->getPriority();

    if(((*(this->state)) == INJURED and msg.p > priority ) or *(this->state) != INJURED) {
        this->sender->sendMedicPermission(sourceRank);
    }else if((*(this->state) == INJURED and msg.p < priority)) {
        this->sender->ignoredMedicRequests->push_back(sourceRank);
    }
}

void Receiver::handleMedicRelease(MedicRelease msg) {
    (*permissions)++;

    if(*permissions == this->P - S_MAX) {
        //TODO
        *permissions = 0;
    }
}

void Receiver::handleCentrumRequest(CenterRequest msg, int sourceRank) {
    double priority = this->sender->getPriority();

    //TODO Nie wszystkie przypadki
    if(((*(this->state)) == WAITING_CENTER and msg.p > priority ) or *(this->state) != WAITING_CENTER) {
        this->sender->sendCenterPermission(msg.w, sourceRank);
    }
    else if((*(this->state) == WAITING_CENTER and msg.p < priority)){
        this->sender->ignoredCenterRequests->push_back(sourceRank);
    }
}

void Receiver::handleCentrumRelease(CenterRequest msg) {
    (*permissions)++;

    if(*permissions == this->P * W_MAX - T_MAX) {
        //TODO
        *permissions = 0;
    }
}

void Receiver::handleDeath(DeathMsg msg) {
    this->P -= 1;

    if(*(this->wType) == msg.w)
        (*weaponCount)++;
}









