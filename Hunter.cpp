#include "Hunter.h"
#define forever while(1)
#define TESTING true

Hunter::Hunter(int N, int nr){
	this -> N = N;
	this -> nr = nr;
	this -> weapon = weaponType(NONE);
	this -> permissions = new int(0);
	this -> currentState = State(NEW);
    this -> sleep_mutex = PTHREAD_MUTEX_INITIALIZER;

	this -> sender = new Sender(N, nr);
	this -> receiver = new Receiver(N, permissions, &weapon, &currentState, sender, &(this->sleep_mutex));
}

weaponType Hunter::drawNewWeaponType(){
	weaponType w = static_cast<weaponType>(rand()%2 + 1);
	return w;
}

void Hunter::setWeapon(weaponType w){
    this -> receiver -> lock_state(true);
	this -> weapon = w;
    this -> receiver -> lock_state(false);
}

void Hunter::setState(State s){
    this -> receiver -> lock_weapon(true);
	this -> currentState = s;
    this -> receiver -> lock_weapon(false);
}

void Hunter::start(){
	pthread_t t;
    typedef void * (*THREADFUNCPTR)(void *);
	pthread_create(&t, NULL, (THREADFUNCPTR) &Receiver::run, this -> receiver);
	this -> setState(State(WAITING_WEAPON));
	return;	
}

void Hunter::requestWeapon(){
	weaponType w = this -> drawNewWeaponType();
	this -> setWeapon(w);
    Printer::print({Printer::state2str(this->currentState),
        Printer::weapon2str(w)}, this->nr);
    if(this -> N > 1){
        this -> sender -> broadcastWeaponRequest(w);
        pthread_mutex_lock(& this -> sleep_mutex); //czekamy na wakeup od wątku komunikacyjnego
        pthread_mutex_lock(& this -> sleep_mutex);
        pthread_mutex_unlock(& this -> sleep_mutex);
    }
	this -> setState(State(HUNTING));
    if(TESTING) if(w == MIECZ) std::cout << "MIECZ+\n";
                else std::cout<< "KARABIN+\n";
	return;
}

State Hunter::hunt(){
	this -> randSleep();
	//wylosuj kolejny stan i ustaw jako bieżący
	int n = HUNT_DISTRIBUTION(this -> randGenerator);
	this -> setState(static_cast<State>(n));
    if(TESTING) if(this -> weapon == MIECZ) std::cout << "MIECZ-\n";
                else std::cout<< "KARABIN-\n";
	return this -> currentState;
}	

void Hunter::die(){
	//deathMsg zawiera info o zwalnianej broni
    if(TESTING) std::cout << "DIE\n";
	this -> sender -> broadcastDeathMsg(this -> weapon); 
    this -> receiver -> P--;
    if(this -> receiver -> P == 0) return;
    //TODO: mutex na P
    pthread_mutex_lock(& this -> sleep_mutex);
    pthread_mutex_lock(& this -> sleep_mutex);
}


void Hunter::requestMedic(){
	this -> sender -> broadcastWeaponRelease(this -> weapon);
    if(this -> N > 1){
        this -> sender -> broadcastMedicRequest();
        pthread_mutex_lock(& this -> sleep_mutex); //czekamy na wakeup od wątku komunikacyjnego
        pthread_mutex_lock(& this -> sleep_mutex);
        pthread_mutex_unlock(& this -> sleep_mutex);
    }
	this -> setState(State(HOSPITALIZED));
    if(TESTING) std::cout << "HOSPITALIZED\n";
	return;
}

void Hunter::getHospitalized(){
	this -> randSleep();
	this -> sender -> broadcastMedicRelease();
	this -> setState(State(WAITING_WEAPON));
    if(TESTING) std::cout << "UNHOSPITALIZED\n";
	return;
}

int Hunter::requestCenter(){
	int w = this -> randomWeight();
	this -> sender -> broadcastWeaponRelease(this -> weapon);
    if(this -> N > 1){
        this -> sender -> broadcastCenterRequest(w, permissions);
        pthread_mutex_lock(& this -> sleep_mutex); //czekamy na wakeup od wątku komunikacyjnego
        pthread_mutex_lock(& this -> sleep_mutex);
        pthread_mutex_unlock(& this -> sleep_mutex);
    }
	this -> setState(State(WAITING_WEAPON));
	return w;
}

void Hunter::visitCenter(int w){
	this -> randSleep();
	this -> sender -> broadcastCenterRelease(w);
	this -> setState(State(WAITING_WEAPON));
	return;
}


int Hunter::randomWeight(){
    return rand()%W_MAX + 1;
}

void Hunter::randSleep(){
	//sleep between 3 to 7 seconds
	//with 1 milisecond resolution
	int a = 1; int b = 2;

	struct timespec ts1;
	ts1.tv_sec = rand()%(b-a) + a;
	ts1.tv_nsec = (rand()%1000) * pow(10, 6);
	nanosleep(&ts1, NULL);
	return;
}

void Hunter::mainLoop(){
    int w = -1;
    bool alive = true;
    while(alive){
        this -> sender -> incClock();
        switch(this -> currentState) {
            case NEW: 
                Printer::print({Printer::state2str(this->currentState)}, this -> nr);
                this -> start();
                break;
            case WAITING_WEAPON:
                this -> requestWeapon();
                break;
            case HUNTING:
                Printer::print({Printer::state2str(this->currentState),
                    Printer::weapon2str(this->weapon)}, this->nr);
                this -> hunt();
                break;
            case INJURED:
                Printer::print({Printer::state2str(this->currentState)}, this -> nr);
                this -> requestMedic();
                break;
            case DEAD:
                Printer::print({Printer::state2str(this->currentState)}, this -> nr);
                alive = false;
                this -> die();
                break;
            case HOSPITALIZED:
                Printer::print({Printer::state2str(this->currentState)}, this -> nr);
                this -> getHospitalized();
                break;
            case WAITING_CENTER:
                Printer::print({Printer::state2str(this->currentState)}, this -> nr);
                w = this -> requestCenter();
                break;
            case IN_CENTER:
                Printer::print({Printer::state2str(this->currentState)}, this -> nr);
                this -> visitCenter(w);
                break;
        }
    }
    Printer::print({"FINISH"}, this -> nr);
	return;
}
