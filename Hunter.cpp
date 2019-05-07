#include "Hunter.h"
#include <mpi.h>
#include <stdlib.h>

std::discrete_distribution<int> HUNT_DISTRIBUTION {0, 0, 0, 15, 5, 0, 80};


Hunter::Hunter(int nr){
	this->nr = nr;
	this->P = this->N;
	this->clock = 0;
	this->weapon = weaponType(NONE);
	this->permissions = 0;
	this->currentState = State(NEW);

	this->centerRequests = new std::pair<int, float>*[this->N];
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
	this -> weapon = w;
	return w;
}

weaponType Hunter::getWeaponType(){
	return this -> weapon;
}


void Hunter::incPermissions(){
	this -> permissions++;
}

int Hunter::getPermissions(){
	return this -> permissions;
}


void Hunter::setState(State s){
	this -> currentState = s;
}

State Hunter::getState(){
	return this -> currentState;
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


State Hunter::hunt(){
	int n = HUNT_DISTRIBUTION(this -> randGenerator);
	return static_cast<State>(n);
}	

