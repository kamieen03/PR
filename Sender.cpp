#include "Sender.h"

Sender::Sender(int size, int nr){
	this -> N = size;
	this -> nr = nr;

	this -> clock = 0;

	this -> ignoredWeaponRequests = new std::list<std::pair<int, weaponType>>;
	this -> ignoredMedicRequests = new std::list<int>;
    this -> ignoredCenterRequests = new std::list<int>;
    this -> centerRequests = new std::pair<int, float>*[N];

    this -> iwrMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> imrMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> icrMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> crMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> clockMutex = PTHREAD_MUTEX_INITIALIZER;
}

void Sender::broadcastWeaponRequest(weaponType w, float p){
	WeaponRequest wr = { .w = w, .p = p };	
	size_t size = sizeof wr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr)
			MPI_Send(&wr, size, MPI_BYTE, i, W_REQ, MPI_COMM_WORLD);
	return;
}

void Sender::broadcastWeaponRelease(weaponType w) {
	WeaponRelease wr = { .w = w };
	size_t size = sizeof wr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr)
			MPI_Send(&wr, size, MPI_BYTE, i, W_REL, MPI_COMM_WORLD);
	this -> sendWeaponPermission(w);
	return;
}	


void Sender::broadcastMedicRequest(float p){
	MedicRequest mr = { .p = p };	
	size_t size = sizeof mr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr)
			MPI_Send(&mr, size, MPI_BYTE, i, M_REQ, MPI_COMM_WORLD);
	return;
}

void Sender::broadcastMedicRelease() {
	MedicRelease mr = {};
	size_t size = sizeof mr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr)
			MPI_Send(&mr, size, MPI_BYTE, i, M_REL, MPI_COMM_WORLD);
	this -> sendMedicPermission();
	return;
}	



//w - waga upolowanego bandersnatcha
//p - priorytet tego procesu
//permissions - przeazana zmienna, do której zapiszemy zliczone zgody
//return - łączna waga pozwoleń uzyskanych ze zliczania w tablicy
int Sender::broadcastCenterRequest(int w, float p, int* permissions){
	CenterRequest cr = { .w = w, .p = p };	
	size_t size = sizeof cr;
	int granted_permissions = this -> countCenterPermissions(p);
	*permissions = granted_permissions;

	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr)
			MPI_Send(&cr, size, MPI_BYTE, i, C_REQ, MPI_COMM_WORLD);
	return granted_permissions;
}

//w - waga upolowanego bandersnatcha
void Sender::broadcastCenterRelease(int w) {
	CenterRelease cr = { .w = w };
	size_t size = sizeof cr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr)
			MPI_Send(&cr, size, MPI_BYTE, i, C_REL, MPI_COMM_WORLD);
	this -> sendCenterPermission(w);
	return;
}	


void Sender::broadcastDeathMsg(weaponType w) {
	DeathMsg dm = { .w = w };
	size_t size = sizeof dm;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr)
			MPI_Send(&dm, size, MPI_BYTE, i, DEATH, MPI_COMM_WORLD);
	this -> sendWeaponPermission(w);
	return;
}

void Sender::incClock(){
	pthread_mutex_lock(this -> clockMutex);
	this -> clock++;
	pthread_mutex_unlock(this -> clockMutex);
}

void Sender::getClock(){
	pthread_mutex_lock(this -> clockMutex);
	double clock = this -> clock;
	pthread_mutex_unlock(this -> clockMutex);
	return clock;
}

double Sender::getPriority(){
        return this -> getClock() + (double)this->nr/this->N;
}






//w - broń zwolniona przez proces
void Sender::sendWeaponPermission(weaponType w, int nr = -1) {
    if(nr == -1) {
        pthread_mutex_lock(this -> iwrMutex);
        if(this->ignoredWeaponRequests.size() == 0){
            pthread_mutex_unlock(this -> iwrMutex);
            return;
        }
        auto iwr = this -> ignoredWeaponRequests;

        for (auto it = iwr -> begin(); it != iwr -> end(); it++){
            std::pair<int, weaponType> req = *it;
            int nr = req.first; weaponType req_w = req.second;
            if (w == req_w){
                iwr -> erase(it);
                break;
            }
        }
        pthread_mutex_unlock(this -> iwrMutex);

    }

	int temp = 1;
        MPI_Send(&temp, 1, MPI_INT, nr, W_PER, MPI_COMM_WORLD);
	return;
}

//nr - nr procesu-adresata
void Sender::sendMedicPermission(int nr = -1) {
    if(nr == -1) {
        pthread_mutex_lock(this -> imrMutex);
        if(this->ignoredMedicRequests.size() == 0){
            pthread_mutex_unlock(this -> imrMutex);
            return;
        }
        int nr = this->ignoredMedicRequests -> front();
        this -> ignoredWeaponRequests -> pop_front();
        pthread_mutex_unlock(this -> imrMutex);

    }
	int temp = 1;
        MPI_Send(&temp, 1, MPI_INT, nr, M_PER, MPI_COMM_WORLD);
	return;
}

//nr - nr procesu-adresata
//permission_weight - waga z jaką wysłać zgodę
void Sender::sendCenterPermission(int permission_weight, int nr = -1) {
    if(nr == -1) {
        pthread_mutex_lock(this -> imrMutex);
        if(this->ignoredCenterRequests.size() == 0){
            pthread_mutex_unlock(this -> icrMutex);
            return;
        }
        int nr = this->ignoredCenterRequests -> front();
        this -> ignoredCenterRequests -> pop_front();
        pthread_mutex_unlock(this -> icrMutex);
    }

	int temp = permission_weight;
        MPI_Send(&temp, 1, MPI_INT, nr, C_PER, MPI_COMM_WORLD);
	return;
}

//my_p - priorytet tego procesu
int Sender::countCenterPermissions(float my_p){
	int N = this -> getN();
	std::pair<int, float>* cr;
	int sum = 0;

	pthread_mutex_lock(this -> crMutex);
	for(int i = 0; i < this -> N; i++){
		cr = this -> centerRequests[i];
		if (cr -> second < my_p )
			sum += W_MAX - cr -> first;
	}
	pthread_mutex_unlock(this -> crMutex);
	return sum;
}


//----------------------------Adding and setting lists------------------

void Sender::ignoreWeaponRequest(std::pair<int, weaponType> req){
    pthread_mutex_lock(this -> iwrMutex);
    this -> ignoredWeaponRequests -> push_back(req);
    pthread_mutex_unlock(this -> iwrMutex);
}

void Sender::ignoreMedicRequest(int nr){
    pthread_mutex_lock(this -> imrMutex);
    this -> ignoredMedicRequests -> push_back(nr);
    pthread_mutex_unlock(this -> imrMutex);
}

void Sender::ignoreCenterRequest(int nr){
    pthread_mutex_lock(this -> icrMutex);
    this -> ignoredCenterRequests -> push_back(nr);
    pthread_mutex_unlock(this -> icrMutex);
}

void Sender::SetCenterRequest(int nr, std::pair<int, float> req){
    pthread_mutex_lock(this -> crMutex);
    this -> centerRequests[nr] = req;
    pthread_mutex_unlock(this -> crMutex);
}

//---------------------Remove requests from lists upon receiving release-------------------

void removeIgnoredWeaponRequest(int nr){
    pthread_mutex_lock(this -> iwrMutex);
    this -> ignoredWeaponRequests -> remove_if([&nr](req) {req.first == nr}); 
    pthread_mutex_unlock(this -> iwrMutex);
}

void removeIgnoredMedicRequest(int nr){
    pthread_mutex_lock(this -> imrMutex);
    this -> ignoredMedicRequests -> remove_if([&nr](req) {req == nr}); 
    pthread_mutex_unlock(this -> imrMutex);
}

void removeIgnoredCenterRequest(int nr){
    pthread_mutex_lock(this -> icrMutex);
    this -> ignoredCenterRequests -> remove_if([&nr](req) {req == nr}); 
    pthread_mutex_unlock(this -> icrMutex);

    pthread_mutex_lock(this -> crMutex);
    this -> centerRequests[nr] = nullptr;
    pthread_mutex_unlock(this -> crMutex);
}


//DONE: zabezpieczyć funkcje sendPermission przed przypadkiem pustych list
//TODO: update clock

