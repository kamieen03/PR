#include "Receiver.h"


Receiver::Receiver(int N, int *permissions, weaponType *wType, State *state, Sender *sender, pthread_mutex_t *sleep_mutex){
    this->P = N;
    this->permissions = permissions;
    this->wType = wType;
    this->state = state;
    this->sleep_mutex = sleep_mutex;

    this->sender = sender;
    this->isRunning = false;
}

void* Receiver::run(void* args){
    MPI_Status status;
    isRunning = true;

    while(isRunning) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &status);

        switch(status.MPI_TAG) {
            case W_REQ:
                WeaponRequest wReq;
                this -> receive(&wReq, &status);
                this -> handleWeaponRequest(wReq, status.MPI_SOURCE);
                break;
            case W_REL:
                WeaponRelease wRel;
                this -> receive(&wRel, &status);
                this -> handleWeaponRelease(status.MPI_SOURCE);
                break;
            case W_PER:
                int wPer;
                this -> receive(&wPer, &status);
                this -> handleWeaponPermission();
                break;
            case M_REQ:
                MedicRequest mReq;
                this -> receive(&mReq, &status);
                this -> handleMedicRequest(mReq, status.MPI_SOURCE);
                break;
            case M_REL:
                MedicRelease mRel;
                this -> receive(&mRel, &status);
                this -> handleMedicRelease(status.MPI_SOURCE);
                break;
            case M_PER:
                int mPer;
                this -> receive(&mPer, &status);
                this -> handleMedicPermission();
                break;
            case C_REQ:
                CenterRequest cReq;
                this -> receive(&cReq, &status);
                this -> handleCenterRequest(cReq, status.MPI_SOURCE);
                break;
            case C_REL:
                CenterRelease cRel;
                this -> receive(&cRel, &status);
                this -> handleCenterRelease(status.MPI_SOURCE);
                break;
            case C_PER:
                int cPer;
                this -> receive(&cPer, &status);
                this -> handleCenterPermission(cPer);
                break;
            case DEATH:
                DeathMsg dMsg;
                this -> receive(&dMsg, &status);
                this -> handleDeath(dMsg);
                break;
        }

    }


}

template <class T>void Receiver::receive(T* data, MPI_Status* status) {
    MPI_Recv(data, sizeof data, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
}

void Receiver::stopReceiving() {
    this->isRunning = false;
}


// WEAPON
void Receiver::handleWeaponRequest(WeaponRequest msg, int sourceRank) {
    double priority = this->sender->getPriority();

    if(((*(this->state)) == WAITING_WEAPON and msg.p > priority ) or *(this->state) != WAITING_WEAPON){
        this->sender->sendWeaponPermission(msg.w, sourceRank);
    }else if((*(this->state) == WAITING_WEAPON and msg.p < priority)) {
        this->sender->ignoreWeaponRequest(std::make_pair(sourceRank, msg.w));
    }
}

void Receiver::handleWeaponPermission() {
    int WEAPON_NUMBER = (*wType == KARABIN) ? K_MAX : M_MAX;

    (*permissions)++;

    if(*permissions >= this->P - WEAPON_NUMBER) {
        *permissions = 0;
    }
}

void Receiver::handleWeaponRelease(int sourceRank) {
    this->sender->removeIgnoredWeaponRequest(sourceRank);
}


// MEDIC
void Receiver::handleMedicRequest(MedicRequest msg, int sourceRank) {
    double priority = this->sender->getPriority();

    if(((*(this->state)) == INJURED and msg.p > priority ) or *(this->state) != INJURED) {
        this->sender->sendMedicPermission(sourceRank);
    }else if((*(this->state) == INJURED and msg.p < priority)) {
        this->sender->ignoreMedicRequest(sourceRank);
    }
}

void Receiver::handleMedicPermission() {
    (*permissions)++;

    if(*permissions >= this->P - S_MAX) {
        pthread_mutex_unlock(sleep_mutex);
        *permissions = 0;
    }
}

void Receiver::handleMedicRelease(int sourceRank) {
    this->sender->removeIgnoredMedicRequest(sourceRank);
}


// CENTER
void Receiver::handleCenterRequest(CenterRequest msg, int sourceRank) {
    double priority = this->sender->getPriority();

    if(((*(this->state)) == WAITING_CENTER and msg.p > priority ) or *(this->state) != WAITING_CENTER) {
        this->sender->sendCenterPermission(msg.w, sourceRank);
    }
    else if((*(this->state) == WAITING_CENTER and msg.p < priority) or *(this->state) == IN_CENTER){
        this->sender->ignoreCenterRequest(sourceRank);
    }
}

void Receiver::handleCenterPermission(int weight){
    *permissions += weight;

    if(*permissions >= this->P * W_MAX - T_MAX) {
        pthread_mutex_unlock(sleep_mutex);
        *permissions = 0;
    }
}

void Receiver::handleCenterRelease(int sourceRank) {
    this->sender->removeIgnoredCenterRequest(sourceRank);
}


// DEATH
void Receiver::handleDeath(DeathMsg msg) {
    (this->P)--;
}
