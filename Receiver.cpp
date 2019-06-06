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

void* Receiver::run(void* args){
    MPI_Status status;
    isRunning = true;
    int flag;
    MSG* msg = new MSG;

    while(isRunning) {
        //pthread_mutex_lock(&(this -> sender -> mpi_mutex));
        MPI_Probe(MPI_ANY_SOURCE, MPI_ANY_TAG, MPI_COMM_WORLD,  &status);
        //pthread_mutex_unlock(&(this -> sender -> mpi_mutex));
        //if( !flag) continue;

        //Printer::print({std::to_string(status.MPI_TAG), std::to_string(status.MPI_SOURCE),
          //  std::to_string(this->sender->getClock())}, this -> sender -> getNr());

        this -> receive(msg, &status);
        switch(status.MPI_TAG) {
            case W_REQ:
                this -> handleWeaponRequest(*msg, status.MPI_SOURCE);
                //Printer::print({Printer::state2str(*state), "- WEAPON REQUEST na ", Printer::weapon2str(*(this->wType))}, status.MPI_SOURCE);
                break;
            case W_REL:
                this -> handleWeaponRelease(*msg, status.MPI_SOURCE);
                //Printer::print({Printer::state2str(*state), "- WEAPON RELEASE na ", Printer::weapon2str(*wType)}, status.MPI_SOURCE);
                break;
            case W_PER:
                this -> handleWeaponPermission(*msg);
                //Printer::print({Printer::state2str(*state), "- WEAPON PERMISSION na ", Printer::weapon2str(*wType)}, status.MPI_SOURCE);
                break;
            case M_REQ:
                this -> handleMedicRequest(*msg, status.MPI_SOURCE);
                break;
            case M_REL:
                this -> handleMedicRelease(*msg, status.MPI_SOURCE);
                break;
            case M_PER:
                this -> handleMedicPermission(*msg);
                break;
            case C_REQ:
                this -> handleCenterRequest(*msg, status.MPI_SOURCE);
                break;
            case C_REL:
                this -> handleCenterRelease(*msg, status.MPI_SOURCE);
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
    //std :: cout << "receive " <<  (sizeof (data)) << " " << sizeof(MSG) << std::endl;
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

    if(a or b or c){
        this->sender->sendWeaponPermission(msg.w, sourceRank);
    }else {
        this->sender->ignoreWeaponRequest(std::make_pair(sourceRank, msg.w));
    }
}

void Receiver::handleWeaponPermission(MSG msg) {
    int WEAPON_NUMBER = (*(this -> wType) == KARABIN) ? K_MAX : M_MAX;
    this -> sender -> setClock(msg.p);

    (*permissions)++;

        Printer::print({"handle wper", std::to_string(*permissions)}, this -> sender -> getNr());
    if(*permissions >= this->P - WEAPON_NUMBER) {
        *permissions = 0;
        pthread_mutex_unlock(sleep_mutex);
    }


}

void Receiver::handleWeaponRelease(MSG msg, int sourceRank) {
    this -> sender -> setClock(msg.p);
    this->sender->removeIgnoredWeaponRequest(sourceRank);
}


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

    if(a or b)
        this->sender->sendMedicPermission(sourceRank);
    else 
        this->sender->ignoreMedicRequest(sourceRank);
}

void Receiver::handleMedicPermission(MSG msg) {
    this -> sender -> setClock(msg.p);
    (*permissions)++;

    if(*permissions >= this->P - S_MAX) {
        *permissions = 0;
        pthread_mutex_unlock(sleep_mutex);
    }
}

void Receiver::handleMedicRelease(MSG msg, int sourceRank) {
    this -> sender -> setClock(msg.p);
    this->sender->removeIgnoredMedicRequest(sourceRank);
}


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

    this -> sender -> setCenterRequest(sourceRank, std::make_pair(msg.w, msg.p));
    if (a or b)
        this->sender->sendCenterPermission(W_MAX, sourceRank);
    else
        this->sender->ignoreCenterRequest(sourceRank);
}

void Receiver::handleCenterPermission(MSG msg){
    this -> sender -> setClock(msg.p);
    *permissions += msg.w;

    if(*permissions >= this->P * W_MAX - T_MAX) {
        *permissions = 0;
        pthread_mutex_unlock(sleep_mutex);
    }
}

void Receiver::handleCenterRelease(MSG msg, int sourceRank) {
    this -> sender -> setClock(msg.p);
    this->sender->removeIgnoredCenterRequest(sourceRank);
}


// DEATH
void Receiver::handleDeath(MSG msg) {
    this -> sender -> setClock(msg.p);
    (this->P)--;
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
