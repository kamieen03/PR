#include "Hunter.h"


Hunter::Hunter(int N, int nr){
	this -> N = N;
	this -> nr = nr;
	this -> weapon = weaponType(NONE);
	this -> permissions = new int(0);
	this -> currentState = State(NEW);

	this -> sender = new Sender(N, nr);
	this -> receiver = new Receiver();
}

weaponType Hunter::drawNewWeaponType(){
	weaponType w = static_cast<weaponType>(rand()%2 + 1);
	return w;
}

void Hunter::setWeapon(weaponType w){
	this -> weapon = w;
}

void Hunter::setState(State s){
	this -> currentState = s;
}

void Hunter::start(){
	pthread_t t;
	pthread_create(&t, NULL, this -> receiver -> start, NULL);
	this -> setState(State(WAITING_WEAPON));
	return;	
}

void Hunter::requestWeapon(){
	weaponType w = this -> drawNewWeaponType();
	this -> sender -> broadcastWeaponRequest(w, this -> getPriority());
	pause();	//czekamy na wakeup od wątku komunikacyjnego
	this -> setWeapon(w);
	this -> setState(State(HUNTING));
	return;
}

State Hunter::hunt(){
	this -> randSleep();
	//wylosuj kolejny stan i ustaw jako bieżący
	int n = HUNT_DISTRIBUTION(this -> randGenerator);
	this -> setState(static_cast<State>(n));
	return this -> currentState;
}	

void Hunter::die(){
	//deathMsg zawiera info o zwalnianej broni
	this -> sender -> broadcastDeathMsg(this -> weapon); 
	pause();	//the final pause
}


void Hunter::requestMedic(){
	this -> sender -> broadcastWeaponRelease(this -> weapon);
	this -> sender -> broadcastMedicRequest(this -> getPriority());
	pause();	//czekamy na wakeup od wątku komunikacyjnego
	this -> setState(State(HOSPITALIZED));
	return;
}

void Hunter::getHospitalized(){
	this -> randSleep();
	this -> sender -> broadcastMedicRelease();
	this -> setState(State(WAITING_WEAPON));
	return;
}

void Hunter::requestCenter(){
	int w = this -> randomWeight();
	this -> sender -> broadcastWeaponRelease(this -> weapon);
	this -> sender -> broadcastCenterRequest(w, this -> getPriority(), permissions);
	pause();
	this -> randSleep();
	this -> sender -> broadcastCenterRelease(w);
	this -> setState(State(WAITING_WEAPON));
	return;
}

void Hunter::randSleep(){
	//sleep between 3 to 7 seconds
	//with 1 milisecond resolution
	int a = 3; int b = 7;

	struct timespec ts1;
	ts1.tv_sec = rand()%(b-a) + a;
	ts1.tv_nsec = (rand()%1000) * pow(10, 6);
	nanosleep(&ts1, NULL);
	return;
}

void Hunter::mainLoop(){
	switch(this -> currentState) {
		case NEW: 
			this -> start();
			break;
		case WAITING_WEAPON:
			this -> requestWeapon();
			break;
		case HUNTING:
			this -> hunt();
			break;
		case INJURED:
			this -> requestMedic();
			break;
		case DEAD:
			this -> die();
			break;
		case HOSPITALIZED:
			this -> getHospitalized();
			break;
		case WAITING_CENTER:
			this -> requestCenter();
			break;	
	}
	return;
}
