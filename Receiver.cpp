#include "Receiver.h"


Receiver::Receiver(int N, int *permissions, weaponType *wType, State *state, Sender *sender, pthread_mutex_t *sleep_mutex){
    this->P = N;
    this->permissions = permissions;
    this->wType = wType;
    this->state = state;
    this->sleep_mutex = sleep_mutex;

    this->sender = sender;
    this->isRunning = false;

    this->STATE_MUTEX = PTHREAD_MUTEX_INITIALIZER;
    this->WEAPON_MUTEX = PTHREAD_MUTEX_INITIALIZER;
}

//zakładamy, że poścmierci Huntera, Receiver wciąż operuje (by móc w końcu wykryć zakończenie)
//gdyby REceiver nie operował po ścierci Huntera, należałoby zmienić N na P w funkcjach
//handlePermission

void* Receiver::run(void* args){
    MPI_Status status;
    isRunning = true;
    int flag;
    MSG* msg = new MSG;

    while(isRunning) {
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,  &status);
        this -> receive(msg, &status);
        switch(status.MPI_TAG) {
            case W_REQ:
                this -> handleWeaponRequest(*msg, status.MPI_SOURCE);
                break;
            case W_PER:
                this -> handleWeaponPermission(*msg);
                break;
            case M_REQ:
                this -> handleMedicRequest(*msg, status.MPI_SOURCE);
                break;
            case M_PER:
                this -> handleMedicPermission(*msg);
                break;
            case C_REQ:
                this -> handleCenterRequest(*msg, status.MPI_SOURCE);
                break;
            case C_PER:
                this -> handleCenterPermission(*msg);
                break;
            case DEATH:
                this -> handleDeath(*msg);
                break;
        }

    }


}

void Receiver::receive(MSG* data, MPI_Status* status) {
    pthread_mutex_lock(&(this -> sender -> mpi_mutex));
    MPI_Recv(data, sizeof(MSG), MPI_BYTE, MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD, status);
    pthread_mutex_unlock(&(this -> sender -> mpi_mutex));
}

void Receiver::stopReceiving() {
    this->isRunning = false;
}


// WEAPON
void Receiver::handleWeaponRequest(MSG msg, int sourceRank) {
    this -> sender -> setClock(msg.p);
    this -> sender -> lock_current_req_p(true);
    this -> lock_state(true);
    this -> lock_weapon(true);
    double priority = this->sender->getCurrentReqP();
    bool a = (*(this->state) == WAITING_WEAPON) and (msg.p < priority) and (msg.w == *(this->wType));
    bool b = !((*(this->state) == WAITING_WEAPON) or (*(this->state) == HUNTING));
    bool c = msg.w != *(this->wType);
    this -> sender -> lock_current_req_p(false);
    this -> lock_state(false);
    this -> lock_weapon(false);
        //Printer::print({"handle wreq", std::to_string(sourceRank), std::to_string(priority), std::to_string(msg.p), 
            //std::to_string(a), std::to_string(b), std::to_string(c), std::to_string(this->sender->getClock())}, this -> sender ->       getNr());

    //send permission to request of priority p
    if(a or b or c){
        this->sender->sendWeaponPermission(sourceRank, msg.p);
    }else {
        this->sender->ignoreWeaponRequest(std::make_pair(sourceRank, msg.p));
    }
}

void Receiver::handleWeaponPermission(MSG msg) {
    this -> sender -> setClock(msg.p);
    if(msg.req_p != this -> sender -> getCurrentReqP() or
        !(this -> sender-> waitingForPermissions)) return;
    int WEAPON_NUMBER = (*(this -> wType) == KARABIN) ? K_MAX : M_MAX;

    (*permissions)++;

        //Printer::print({"handle wper", std::to_string(*permissions), std::to_string(this->N - WEAPON_NUMBER),
          //  std::to_string(msg.req_p), std::to_string(this->sender->getCurrentReqP())}, this -> sender -> getNr());
    if(*permissions >= this->sender ->getN() - WEAPON_NUMBER) {
        *permissions = 0;
        this -> sender -> waitingForPermissions = false;
        pthread_mutex_unlock(sleep_mutex);
    }
}

//void Receiver::handleWeaponRelease(MSG msg, int sourceRank) {
//    this -> sender -> setClock(msg.p);
//    this->sender->removeIgnoredWeaponRequest(sourceRank);
//}


// MEDIC
void Receiver::handleMedicRequest(MSG msg, int sourceRank) {
    this -> sender -> setClock(msg.p);

    this -> sender -> lock_current_req_p(true);
    this -> lock_state(true);
    double priority = this->sender->getCurrentReqP();
    bool a = (*(this->state)) == INJURED and msg.p < priority;
    bool b = !(*(this->state) == INJURED or *(this->state) == HOSPITALIZED);
    this -> sender -> lock_current_req_p(false);
    this -> lock_state(false);
        //Printer::print({"handle mreq", std::to_string(sourceRank), std::to_string(priority), std::to_string(msg.p), 
        //std::to_string(a), std::to_string(b), std::to_string(this->sender->getClock())}, this -> sender -> getNr());

    if(a or b)
        this->sender->sendMedicPermission(sourceRank, msg.p);
    else 
        this->sender->ignoreMedicRequest(std::make_pair(sourceRank, msg.p));
}

void Receiver::handleMedicPermission(MSG msg) {
    this -> sender -> setClock(msg.p);
    if(msg.req_p != this -> sender -> getCurrentReqP() or 
        !(this -> sender-> waitingForPermissions)) return;
    (*permissions)++;
        //Printer::print({"handle mper", std::to_string(*permissions), std::to_string(this->N - S_MAX),
          //  std::to_string(msg.req_p), std::to_string(this->sender->getCurrentReqP())}, this -> sender -> getNr());

    if(*permissions >= this->sender -> getN() - S_MAX) {
        this -> sender -> waitingForPermissions = false;
        *permissions = 0;
        pthread_mutex_unlock(sleep_mutex);
    }
}

//void Receiver::handleMedicRelease(MSG msg, int sourceRank) {
//    this -> sender -> setClock(msg.p);
//    this->sender->removeIgnoredMedicRequest(sourceRank);
//}


// CENTER
void Receiver::handleCenterRequest(MSG msg, int sourceRank) {
    this -> sender -> setClock(msg.p);

    this -> sender -> lock_current_req_p(true);
    this -> lock_state(true);
    double priority = this->sender->getCurrentReqP();
    bool a = (*(this->state) == WAITING_CENTER and msg.p < priority );
    bool b = !(*(this->state) == WAITING_CENTER or *(this->state) == IN_CENTER);
    this -> sender -> lock_current_req_p(false);
    this -> lock_state(false);

    this -> sender -> setCenterRequest(sourceRank, std::make_pair(msg.weight, msg.p));
    if (a or b)
        this->sender->sendCenterPermission(W_MAX, sourceRank, msg.p);
    else
        this->sender->ignoreCenterRequest(std::make_pair(sourceRank, msg.p));
}

void Receiver::handleCenterPermission(MSG msg){
    if(msg.req_p != this -> sender -> getCurrentReqP() or
        !(this -> sender-> waitingForPermissions)) return;

    this -> sender -> setClock(msg.p);
    *permissions += msg.permission_weight;

        //Printer::print({"handle cper", std::to_string(*permissions), std::to_string(this->N*W_MAX - T_MAX),
          //  std::to_string(msg.req_p), std::to_string(this->sender->getCurrentReqP())}, this -> sender -> getNr());
    if(*permissions >= this->sender->getN() * W_MAX - T_MAX) {
        this -> sender -> waitingForPermissions = false;
        *permissions = 0;
        pthread_mutex_unlock(sleep_mutex);
    }
}

//void Receiver::handleCenterRelease(MSG msg, int sourceRank) {
//    this -> sender -> setClock(msg.p);
//    this->sender->removeIgnoredCenterRequest(sourceRank);
//}


// DEATH
void Receiver::handleDeath(MSG msg) {
    this -> sender -> setClock(msg.p);
    (this->P)--;
    if(this -> P == 0){ 
        this -> stopReceiving();
        pthread_mutex_unlock(this -> sleep_mutex);
    }

}


//helpers================================================
void Receiver::lock_state(bool val){
    if(val)
        pthread_mutex_lock(&(this->STATE_MUTEX));
    else
        pthread_mutex_unlock(&(this->STATE_MUTEX));
}

void Receiver::lock_weapon(bool val){
    if(val)
        pthread_mutex_lock(&(this->WEAPON_MUTEX));
    else
        pthread_mutex_unlock(&(this->WEAPON_MUTEX));
}
