#include "Sender.h"

Sender::Sender(int size, int nr){
	this -> N = size;
	this -> nr = nr;

	this -> clock = 0;
    this -> current_req_p = 0;

	this -> ignoredWeaponRequests = new std::list<std::pair<int, weaponType>>;
	this -> ignoredMedicRequests = new std::list<int>;
    this -> ignoredCenterRequests = new std::list<int>;
    this -> centerRequests = new std::pair<int, float>*[N];
    for(int i = 0; i < N; i++) this-> centerRequests[i] = nullptr;

    this -> iwrMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> imrMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> icrMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> crMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> clockMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> mpi_mutex = PTHREAD_MUTEX_INITIALIZER;
}

//---------------------------Broadcasty - requesty i releasy-----------------------------
void Sender::broadcastWeaponRequest(weaponType w){
    this -> incClock();
    float p = this -> getPriority();
    this -> current_req_p = p;
	WeaponRequest wr = { .w = w, .p = p };	
    //Printer::print({"W_REQ", std::to_string(p)}, this -> nr);
	size_t size = sizeof wr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&(this -> mpi_mutex));
			MPI_Send(&wr, size, MPI_BYTE, i, W_REQ, MPI_COMM_WORLD);
            pthread_mutex_unlock(&mpi_mutex);
        }
	return;
}

void Sender::broadcastWeaponRelease(weaponType w) {
    this -> incClock();
    float p = this -> getPriority();
	WeaponRelease wr = { .w = w, .p = p };
	size_t size = sizeof wr;
	int N = this -> getN();
	int nr = this -> getNr();
    //Printer::print({"W_REL", Printer::weapon2str(w)}, this -> nr);
	for(int i = 0; i < N; i++)
		if (i != nr){
            //Printer::print({"wr to", std::to_string(i)}, nr);
            pthread_mutex_lock(&mpi_mutex);
			MPI_Send(&wr, size, MPI_BYTE, i, W_REL, MPI_COMM_WORLD);
            pthread_mutex_unlock(&mpi_mutex);
        }
	this -> sendWeaponPermission(w);
	return;
}	


void Sender::broadcastMedicRequest(){
    this -> incClock();
    float p = this -> getPriority();
    this -> current_req_p = p;
	MedicRequest mr = { .p = p };	
	size_t size = sizeof mr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&mpi_mutex);
			MPI_Send(&mr, size, MPI_BYTE, i, M_REQ, MPI_COMM_WORLD);
            pthread_mutex_unlock(&mpi_mutex);
        }
	return;
}

void Sender::broadcastMedicRelease() {
    this -> incClock();
    float p = this -> getPriority();
	MedicRelease mr = { .p = p };
	size_t size = sizeof mr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&mpi_mutex);
			MPI_Send(&mr, size, MPI_BYTE, i, M_REL, MPI_COMM_WORLD);
            pthread_mutex_unlock(&mpi_mutex);
        }
	this -> sendMedicPermission();
	return;
}	



//w - waga upolowanego bandersnatcha
//p - priorytet tego procesu
//permissions - przeazana zmienna, do której zapiszemy zliczone zgody
//return - łączna waga pozwoleń uzyskanych ze zliczania w tablicy
int Sender::broadcastCenterRequest(int w, int* permissions){
    this -> incClock();
    float p = this -> getPriority();
    this -> current_req_p = p;
	CenterRequest cr = { .w = w, .p = p };	
	size_t size = sizeof cr;
	int granted_permissions = this -> countCenterPermissions(p);
	*permissions = granted_permissions;

	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&mpi_mutex);
			MPI_Send(&cr, size, MPI_BYTE, i, C_REQ, MPI_COMM_WORLD);
            pthread_mutex_unlock(&mpi_mutex);
        }
	return granted_permissions;
}

//w - waga upolowanego bandersnatcha
void Sender::broadcastCenterRelease(int w) {
    this -> incClock();
    float p = this -> getPriority();
	CenterRelease cr = { .w = w, .p = p };
	size_t size = sizeof cr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&mpi_mutex);
			MPI_Send(&cr, size, MPI_BYTE, i, C_REL, MPI_COMM_WORLD);
            pthread_mutex_unlock(&mpi_mutex);
        }
	this -> sendCenterPermission(w);
	return;
}	


void Sender::broadcastDeathMsg(weaponType w) {
    this -> incClock();
    float p = this -> getPriority();
	DeathMsg dm = { .w = w, .p = p };
	size_t size = sizeof dm;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&mpi_mutex);
			MPI_Send(&dm, size, MPI_BYTE, i, DEATH, MPI_COMM_WORLD);
            pthread_mutex_unlock(&mpi_mutex);
        }
	this -> sendWeaponPermission(w);
	return;
}


//------------------------------------Pomocnicze------------------------------------

void Sender::incClock(){
	pthread_mutex_lock(&(this -> clockMutex));
	this -> clock++;
	pthread_mutex_unlock(&(this -> clockMutex));
}

void Sender::setClock(float p){ //the other process' priority
    int clka = this -> getClock();
    int clkb = (int) p;
	pthread_mutex_lock(&(this -> clockMutex));
	double clock = (clka > clkb ? clka : clkb) + 1; 
	pthread_mutex_unlock(&(this -> clockMutex));
}

int Sender::getClock(){
	pthread_mutex_lock(&(this -> clockMutex));
	double clock = this -> clock;
	pthread_mutex_unlock(&(this -> clockMutex));
	return clock;
}

double Sender::getPriority(){
    return this -> getClock() + (double)this->nr/this->N;
}

double Sender::getCurrentReqP(){
    return this -> current_req_p;
}




//---------------------------------Pozwolenia-------------------------------

//w - broń zwolniona przez proces
void Sender::sendWeaponPermission(weaponType w, int nr) {
    if(nr == -1) {
        pthread_mutex_lock(&(this -> iwrMutex));
        if(this->ignoredWeaponRequests -> size() == 0){
            pthread_mutex_unlock(&(this -> iwrMutex));
            return;
        }
        auto iwr = this -> ignoredWeaponRequests;

        for (auto it = iwr -> begin(); it != iwr -> end(); it++){
            std::pair<int, weaponType> req = *it;
            nr = req.first; weaponType req_w = req.second;
            if (w == req_w){
                //Printer::print({"swp to", std::to_string(nr)}, this -> nr);
                iwr -> erase(it);
                break;
            }
        }
        pthread_mutex_unlock(&(this -> iwrMutex));

    }

    this -> incClock();
    float p = this -> getPriority();
	WeaponPermission wp = { .p = p };
	size_t size = sizeof wp;
    pthread_mutex_lock(&mpi_mutex);
    MPI_Send(&wp, size, MPI_BYTE, nr, W_PER, MPI_COMM_WORLD);
    pthread_mutex_unlock(&mpi_mutex);
	return;
}

//nr - nr procesu-adresata
void Sender::sendMedicPermission(int nr) {
    if(nr == -1) {
        pthread_mutex_lock(&(this -> imrMutex));
        if(this->ignoredMedicRequests -> size() == 0){
            pthread_mutex_unlock(&(this -> imrMutex));
            return;
        }
        nr = this->ignoredMedicRequests -> front();
        this -> ignoredWeaponRequests -> pop_front();
        pthread_mutex_unlock(&(this -> imrMutex));

    }
    this -> incClock();
    float p = this -> getPriority();
	MedicPermission mp = { .p = p };
	size_t size = sizeof mp;
    pthread_mutex_lock(&mpi_mutex);
    MPI_Send(&mp, size, MPI_BYTE, nr, M_PER, MPI_COMM_WORLD);
    pthread_mutex_unlock(&mpi_mutex);
	return;
}

//nr - nr procesu-adresata
//permission_weight - waga z jaką wysłać zgodę
void Sender::sendCenterPermission(int permission_weight, int nr) {
    if(nr == -1) {
        pthread_mutex_lock(&(this -> imrMutex));
        if(this->ignoredCenterRequests -> size() == 0){
            pthread_mutex_unlock(&(this -> icrMutex));
            return;
        }
        nr = this->ignoredCenterRequests -> front();
        this -> ignoredCenterRequests -> pop_front();
        pthread_mutex_unlock(&(this -> icrMutex));
    }
    this -> incClock();
    float p = this -> getPriority();
	CenterPermission cp = { .w = permission_weight, .p = p };
	size_t size = sizeof cp;
    pthread_mutex_lock(&mpi_mutex);
    MPI_Send(&cp, size, MPI_BYTE, nr, C_PER, MPI_COMM_WORLD);
    pthread_mutex_unlock(&mpi_mutex);
	return;
}

//my_p - priorytet tego procesu
int Sender::countCenterPermissions(float my_p){
	int N = this -> getN();
	std::pair<int, float>* cr;
	int sum = 0;

	pthread_mutex_lock(&(this -> crMutex));
	for(int i = 0; i < this -> N; i++){
		cr = this -> centerRequests[i];
        if(cr != nullptr){
            if (cr -> second < my_p ){
                sum += W_MAX - cr -> first;
            }
        }
	}
	pthread_mutex_unlock(&(this -> crMutex));
	return sum;
}


//----------------------------Adding and setting lists------------------

void Sender::ignoreWeaponRequest(std::pair<int, weaponType> req){
    pthread_mutex_lock(&(this -> iwrMutex));
    this -> ignoredWeaponRequests -> push_back(req);
    pthread_mutex_unlock(&(this -> iwrMutex));
}

void Sender::ignoreMedicRequest(int nr){
    pthread_mutex_lock(&(this -> imrMutex));
    this -> ignoredMedicRequests -> push_back(nr);
    pthread_mutex_unlock(&(this -> imrMutex));
}

void Sender::ignoreCenterRequest(int nr){
    pthread_mutex_lock(&(this -> icrMutex));
    this -> ignoredCenterRequests -> push_back(nr);
    pthread_mutex_unlock(&(this -> icrMutex));
}

void Sender::setCenterRequest(int nr, std::pair<int, float> req){
    pthread_mutex_lock(&(this -> crMutex));
    this -> centerRequests[nr] = new std::pair<int, float> (req);
    pthread_mutex_unlock(&(this -> crMutex));
}

//---------------------Remove requests from lists upon receiving release-------------------

void Sender::removeIgnoredWeaponRequest(int nr){
    pthread_mutex_lock(&(this -> iwrMutex));
    this -> ignoredWeaponRequests -> remove_if([&nr]
        (const std::pair<int, weaponType>& req) {return req.first == nr;}
    ); 
    pthread_mutex_unlock(&(this -> iwrMutex));
}

void Sender::removeIgnoredMedicRequest(int nr){
    pthread_mutex_lock(&(this -> imrMutex));
    this -> ignoredMedicRequests -> remove_if([&nr]
        (const int& req) {return req == nr;}
    ); 
    pthread_mutex_unlock(&(this -> imrMutex));
}

void Sender::removeIgnoredCenterRequest(int nr){
    pthread_mutex_lock(&(this -> icrMutex));
    this -> ignoredCenterRequests -> remove_if([&nr]
        (const int& req) {return req == nr;}
    ); 
    pthread_mutex_unlock(&(this -> icrMutex));

    pthread_mutex_lock(&(this -> crMutex));
    this -> centerRequests[nr] = nullptr;
    pthread_mutex_unlock(&(this -> crMutex));
}

