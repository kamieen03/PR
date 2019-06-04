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
    int flag;

    while(isRunning) {
        pthread_mutex_lock(&(this -> sender -> mpi_mutex));
        MPI_Iprobe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, &flag, &status);
        pthread_mutex_unlock(&(this -> sender -> mpi_mutex));
        if( !flag) continue;

        Printer::print({std::to_string(status.MPI_TAG), std::to_string(status.MPI_SOURCE),
            std::to_string(this->sender->getClock())}, this -> sender -> getNr());

        switch(status.MPI_TAG) {
            case W_REQ:
                WeaponRequest wReq;
                this -> receive(&wReq, &status);
                this -> handleWeaponRequest(wReq, status.MPI_SOURCE);
                //Printer::print({Printer::state2str(*state), "- WEAPON REQUEST na ", Printer::weapon2str(*(this->wType))}, status.MPI_SOURCE);
                break;
            case W_REL:
                WeaponRelease wRel;
                this -> receive(&wRel, &status);
                this -> handleWeaponRelease(wRel, status.MPI_SOURCE);
                //Printer::print({Printer::state2str(*state), "- WEAPON RELEASE na ", Printer::weapon2str(*wType)}, status.MPI_SOURCE);
                break;
            case W_PER:
                WeaponPermission wPer;
                this -> receive(&wPer, &status);
                this -> handleWeaponPermission(wPer);
                //Printer::print({Printer::state2str(*state), "- WEAPON PERMISSION na ", Printer::weapon2str(*wType)}, status.MPI_SOURCE);
                break;
            case M_REQ:
                MedicRequest mReq;
                this -> receive(&mReq, &status);
                this -> handleMedicRequest(mReq, status.MPI_SOURCE);
                break;
            case M_REL:
                MedicRelease mRel;
                this -> receive(&mRel, &status);
                this -> handleMedicRelease(mRel, status.MPI_SOURCE);
                break;
            case M_PER:
                MedicPermission mPer;
                this -> receive(&mPer, &status);
                this -> handleMedicPermission(mPer);
                break;
            case C_REQ:
                CenterRequest cReq;
                this -> receive(&cReq, &status);
                this -> handleCenterRequest(cReq, status.MPI_SOURCE);
                break;
            case C_REL:
                CenterRelease cRel;
                this -> receive(&cRel, &status);
                this -> handleCenterRelease(cRel, status.MPI_SOURCE);
                break;
            case C_PER:
                CenterPermission cPer;
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
    pthread_mutex_lock(&(this -> sender -> mpi_mutex));
    MPI_Recv(data, sizeof data, MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
    pthread_mutex_unlock(&(this -> sender -> mpi_mutex));
}

void Receiver::stopReceiving() {
    this->isRunning = false;
}


// WEAPON
void Receiver::handleWeaponRequest(WeaponRequest msg, int sourceRank) {
    double priority = this->sender->getCurrentReqP();
    this -> sender -> setClock(msg.p);
    //nie mozna porównywac ze swoim obecnym priorytettem tylko z priorytettem requetu, który sie wysłało
    bool a = ((*(this->state) == WAITING_WEAPON) and (msg.p < priority) and (msg.w == *(this->wType)));
    bool b = !((*(this->state) == WAITING_WEAPON) or (*(this->state) == HUNTING));
    bool c = (msg.w != *(this->wType));
        //Printer::print({"handle wreq", std::to_string(sourceRank), std::to_string(priority), std::to_string(msg.p), 
        //std::to_string(a), std::to_string(b), std::to_string(c), std::to_string(this->sender->getClock())}, this -> sender ->       getNr());

    if(((*(this->state) == WAITING_WEAPON) and (msg.p < priority) and (msg.w == *(this->wType))) or
        !((*(this->state) == WAITING_WEAPON) or (*(this->state) == HUNTING)) or
        (msg.w != *(this->wType))){
        this->sender->sendWeaponPermission(msg.w, sourceRank);
    }else {
        this->sender->ignoreWeaponRequest(std::make_pair(sourceRank, msg.w));
    }
}

void Receiver::handleWeaponPermission(WeaponPermission msg) {
    int WEAPON_NUMBER = (*(this -> wType) == KARABIN) ? K_MAX : M_MAX;
    this -> sender -> setClock(msg.p);

    (*permissions)++;

        //Printer::print({"handle wper", std::to_string(*permissions)}, this -> sender -> getNr());
    if(*permissions >= this->P - WEAPON_NUMBER) {
        *permissions = 0;
        pthread_mutex_unlock(sleep_mutex);
    }


}

void Receiver::handleWeaponRelease(WeaponRelease msg, int sourceRank) {
    this -> sender -> setClock(msg.p);
    this->sender->removeIgnoredWeaponRequest(sourceRank);
}


// MEDIC
void Receiver::handleMedicRequest(MedicRequest msg, int sourceRank) {
    this -> sender -> setClock(msg.p);
    double priority = this->sender->getCurrentReqP();

    if(((*(this->state)) == INJURED and msg.p < priority ) or 
        !(*(this->state) == INJURED or *(this->state) == HOSPITALIZED)) {
        this->sender->sendMedicPermission(sourceRank);
    }else {
        this->sender->ignoreMedicRequest(sourceRank);
    }
}

void Receiver::handleMedicPermission(MedicPermission msg) {
    this -> sender -> setClock(msg.p);
    (*permissions)++;

    if(*permissions >= this->P - S_MAX) {
        *permissions = 0;
        pthread_mutex_unlock(sleep_mutex);
    }
}

void Receiver::handleMedicRelease(MedicRelease msg, int sourceRank) {
    this -> sender -> setClock(msg.p);
    this->sender->removeIgnoredMedicRequest(sourceRank);
}


// CENTER
void Receiver::handleCenterRequest(CenterRequest msg, int sourceRank) {
    this -> sender -> setClock(msg.p);
    double priority = this->sender->getCurrentReqP();
    
    this -> sender -> setCenterRequest(sourceRank, std::make_pair(msg.w, msg.p));
    if((*(this->state) == WAITING_CENTER and msg.p < priority ) or
        !(*(this->state) == WAITING_CENTER or *(this->state) == IN_CENTER)) {
        this->sender->sendCenterPermission(W_MAX, sourceRank);
    }else{
        this->sender->ignoreCenterRequest(sourceRank);
    }
}

void Receiver::handleCenterPermission(CenterPermission msg){
    this -> sender -> setClock(msg.p);
    *permissions += msg.w;

    if(*permissions >= this->P * W_MAX - T_MAX) {
        *permissions = 0;
        pthread_mutex_unlock(sleep_mutex);
    }
}

void Receiver::handleCenterRelease(CenterRelease msg, int sourceRank) {
    this -> sender -> setClock(msg.p);
    this->sender->removeIgnoredCenterRequest(sourceRank);
}


// DEATH
void Receiver::handleDeath(DeathMsg msg) {
    this -> sender -> setClock(msg.p);
    (this->P)--;
}
