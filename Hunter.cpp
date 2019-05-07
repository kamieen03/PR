#include "Hunter.h"


std::discrete_distribution<int> HUNT_DISTRIBUTION {0, 0, 0, 15, 5, 0, 80};


Hunter::Hunter(int N, int nr){
	this -> N = N;
	this -> nr = nr;
	this -> P = N;
	this -> clock = 0;
	this -> weapon = weaponType(NONE);
	this -> permissions = 0;
	this -> currentState = State(NEW);
	this -> Communicator = new Communicator();

	this -> centerRequests = new std::pair<int, float>*[this->N];
}

int Hunter::getNr(){
	return this->nr;
}


void Hunter::decreaseP(){
	this -> P--;
}

int Hunter::getP(){
	return this -> P;
}


void Hunter::incrementClock(){
	this -> clock++;
}

int Hunter::getClock(){
	return this -> clock;
}

float Hunter::getPriority(){
	return this -> clock + (float)this->nr/this->N; 
}


weaponType Hunter::drawNewWeaponType(){
	weaponType w = static_cast<weaponType>(rand()%2 + 1);
	return w;
}

void Hunter::setWeapon(weaponType w){
	this -> weapon = w;
}

weaponType Hunter::getWeaponType(){
	return this -> weapon;
}


void Hunter::incPermissions(){
	this -> permissions++;
}

void resetPermissions(){
	this -> permissions = 0;
}

int Hunter::getPermissions(){
	return this -> permissions;
}



void Hunter::ignoreWeaponRequest(int nr, char weapon){
	std::pair<int, char> req (nr, weapon);
	this -> ignoredWeaponRequests.push_back(req);
}

std::pair<int, char> Hunter::deignoreWeaponRequest(int, char){
	std::pair<int, char> req = this->ignoredWeaponRequests.front();
	std::pair<int, char> req_copy (req);
	this -> ignoredWeaponRequests.pop_front();
	return req_copy;
}

void Hunter::ignoreMedicRequest(int nr){
	this -> ignoredMedicRequests.push_back(nr);
}

int Hunter::deignoreMedicRequest(int){
	int nr = this->ignoredMedicRequests.front();
	this -> ignoredMedicRequests.pop_front();
	return nr;
}

void Hunter::ignoreCenterRequest(int nr){
	this -> ignoredCenterRequests.push_back(nr);
}

int Hunter::deignoreCenterRequest(int){
	int nr = this->ignoredCenterRequests.front();
	this -> ignoredCenterRequests.pop_front();
	return nr;
}

void Hunter::recordCenterRequest(int nr, int weight, float priority){
	std::pair<int, float>* req = new std::pair<int,float> (weight, priority);
	this -> centerRequests[nr] = req;
}

void Hunter::forgetCenterRequest(int nr){
	delete this->centerRequests[nr];
	this -> centerRequests[nr] = NULL;
}


void Hunter::setState(State s){
	this -> currentState = s;
}

State Hunter::getState(){
	return this -> currentState;
}

void Hunter::start(){
	pthread t;
	pthread_create(&t, NULL, this->cellphone->start(), NULL);
	this -> setState(State(WAITING_WEAPON));
	return;	
}

void Hunter::requestWeapon(){
	weaponType w = this -> drawNewWeaponType();
	this -> cellphone -> broadcastWeaponRequest(w, &(this->P));
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
	this -> cellphone -> broadcastDeathMsg();
	this -> cellphone -> broadcastWeaponRelease(this -> weapon);
	pause();	//the final pause
}


void Hunter::requestMedic(){
	this -> cellphone -> broadcastMedicRequest();
	pause();	//czekamy na wakeup od wątku komunikacyjnego
	this -> setState(State(HOSPITALIZED));
	return;
}

void Hunter::getHospitalized(){
	this -> randSleep();
	return;
}

void Hunter::randSleep(){
	//sleep between 3 to 7 seconds
	//with 1 milisecond resolution
	int a = 3; int b = 7;
	int time = a * pow(10, 9);
	time += (int) pow(10, 6) * (rand()%((b-a)*1000));
	nanosleep(time);
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
//TODO: implement requestCenter
