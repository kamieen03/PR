#include "Sender.h"

Sender::Sender(int size, int nr){
	this -> N = size;
	this -> nr = nr;

	this -> clock = 0;
    this -> current_req_p = 0;

    this -> waitingForPermissions = false;

	this -> ignoredWeaponRequests = new std::list<std::pair<int, float>>;
	this -> ignoredMedicRequests = new std::list<std::pair<int, float>>;
    this -> ignoredCenterRequests = new std::list<std::pair<int, float>>;
    this -> centerRequests = new std::pair<int, float>*[N];
    for(int i = 0; i < N; i++) this-> centerRequests[i] = nullptr;

    this -> iwrMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> imrMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> icrMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> crMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> clockMutex = PTHREAD_MUTEX_INITIALIZER;
    this -> mpi_mutex = PTHREAD_MUTEX_INITIALIZER;
    this -> current_req_p_mutex = PTHREAD_MUTEX_INITIALIZER;
}

//---------------------------Broadcasty - requesty i releasy-----------------------------
void Sender::broadcastWeaponRequest(weaponType w){
    this -> incClock();
    float p = this -> getPriority();
    this -> lock_current_req_p(true);
    this -> current_req_p = p;
    this -> lock_current_req_p(false);

    this -> waitingForPermissions = true;
	MSG wr;
    wr.w = w; wr.p = p;
	size_t size = sizeof wr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&(this -> mpi_mutex));
			MPI_Send(&wr, size, MPI_BYTE, i, W_REQ, MPI_COMM_WORLD);
            pthread_mutex_unlock(&(this -> mpi_mutex));
            //std::cout << this -> nr << " " << i << std::endl;
        }
	return;
}

void Sender::broadcastWeaponRelease(weaponType w) {
	this -> broadcastWeaponPermission();
	return;
}	


void Sender::broadcastMedicRequest(){
    this -> incClock();
    float p = this -> getPriority();
    this -> lock_current_req_p(true);
    this -> current_req_p = p;
    this -> lock_current_req_p(false);
    this -> waitingForPermissions = true;
	MSG mr;
    mr.p = p;
	size_t size = sizeof mr;
	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&(this -> mpi_mutex));
			MPI_Send(&mr, size, MPI_BYTE, i, M_REQ, MPI_COMM_WORLD);
            pthread_mutex_unlock(&(this -> mpi_mutex));
        }
	return;
}

void Sender::broadcastMedicRelease() {
	this -> broadcastMedicPermission();
	return;
}	



//w - waga upolowanego bandersnatcha
//p - priorytet tego procesu
//permissions - przeazana zmienna, do której zapiszemy zliczone zgody
//return - łączna waga pozwoleń uzyskanych ze zliczania w tablicy
int Sender::broadcastCenterRequest(int w, int* permissions){
    this -> incClock();
    float p = this -> getPriority();
    this -> lock_current_req_p(true);
    this -> current_req_p = p;
    this -> lock_current_req_p(false);
    this -> waitingForPermissions = true;
    //Printer::print({std::to_string(p)}, this -> nr);
	int granted_permissions = this -> countCenterPermissions(p);
	*permissions = granted_permissions;

	MSG cr;
    cr.weight = w; cr.p = p;
	size_t size = sizeof cr;

	int N = this -> getN();
	int nr = this -> getNr();
	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&(this -> mpi_mutex));
			MPI_Send(&cr, size, MPI_BYTE, i, C_REQ, MPI_COMM_WORLD);
            pthread_mutex_unlock(&(this -> mpi_mutex));
        }
	return granted_permissions;
}

//w - waga upolowanego bandersnatcha
void Sender::broadcastCenterRelease(int w) {
	this -> broadcastCenterPermission(w);
	return;
}	


void Sender::broadcastDeathMsg(weaponType w) {
    this -> incClock();
    float p = this -> getPriority();
	MSG dm;
    dm.w = w; dm.p = p;
	size_t size = sizeof dm;
	int N = this -> getN();
	int nr = this -> getNr();

	for(int i = 0; i < N; i++)
		if (i != nr){
            pthread_mutex_lock(&(this -> mpi_mutex));
			MPI_Send(&dm, size, MPI_BYTE, i, DEATH, MPI_COMM_WORLD);
            pthread_mutex_unlock(&(this -> mpi_mutex));
        }
	this -> broadcastWeaponPermission();
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

void Sender::lock_current_req_p(bool val){
    if(val)
        pthread_mutex_lock(&(this->current_req_p_mutex));
    else
        pthread_mutex_unlock(&(this->current_req_p_mutex));
}




//---------------------------------Pozwolenia-------------------------------

void Sender::broadcastWeaponPermission(){
    pthread_mutex_lock(&(this -> iwrMutex));
    if(this->ignoredWeaponRequests -> size() == 0){
        pthread_mutex_unlock(&(this -> iwrMutex));
        return;
    }
    this -> incClock();
    float p = this -> getPriority();
    MSG wp;
    wp.p = p;
    size_t size = sizeof wp;
    auto iwr = *(this -> ignoredWeaponRequests);
    for(std::pair<int, float>& req: iwr){
        wp.req_p = req.second;
        pthread_mutex_lock(&(this -> mpi_mutex));
        MPI_Send(&wp, size, MPI_BYTE, req.first, W_PER, MPI_COMM_WORLD);
        pthread_mutex_unlock(&(this -> mpi_mutex));
    }
    iwr.clear();
    pthread_mutex_unlock(&(this -> iwrMutex));
}


//w - broń zwolniona przez proces
//priorytet z jakim proces otrzymał request
void Sender::sendWeaponPermission(int nr, float req_p) {
    this -> incClock();
    float p = this -> getPriority();
	MSG wp;
    wp.p = p; wp.req_p = req_p;
	size_t size = sizeof wp;
    pthread_mutex_lock(&(this -> mpi_mutex));
    MPI_Send(&wp, size, MPI_BYTE, nr, W_PER, MPI_COMM_WORLD);
    pthread_mutex_unlock(&(this -> mpi_mutex));
	return;
}


void Sender::broadcastMedicPermission(){
    pthread_mutex_lock(&(this -> imrMutex));
    if(this->ignoredMedicRequests -> size() == 0){
        pthread_mutex_unlock(&(this -> imrMutex));
        return;
    }
    this -> incClock();
    float p = this -> getPriority();
    MSG mp;
    mp.p = p;
    size_t size = sizeof mp;
    auto imr = *(this -> ignoredMedicRequests);
    for(std::pair<int, float>& req: imr){
        mp.req_p = req.second;
        pthread_mutex_lock(&(this -> mpi_mutex));
        MPI_Send(&mp, size, MPI_BYTE, req.first, M_PER, MPI_COMM_WORLD);
        pthread_mutex_unlock(&(this -> mpi_mutex));
    }
    imr.clear();
    pthread_mutex_unlock(&(this -> imrMutex));
}

//nr - nr procesu-adresata
void Sender::sendMedicPermission(int nr, float req_p) {
    this -> incClock();
    float p = this -> getPriority();
	MSG mp;
    mp.p = p; mp.req_p = req_p;
	size_t size = sizeof mp;
    pthread_mutex_lock(&(this -> mpi_mutex));
    MPI_Send(&mp, size, MPI_BYTE, nr, M_PER, MPI_COMM_WORLD);
    pthread_mutex_unlock(&(this -> mpi_mutex));
	return;
}



void Sender::broadcastCenterPermission(int permission_weight){
    pthread_mutex_lock(&(this -> icrMutex));
    if(this->ignoredCenterRequests -> size() == 0){
        pthread_mutex_unlock(&(this -> icrMutex));
        return;
    }
    this -> incClock();
    float p = this -> getPriority();
    MSG cp;
    cp.permission_weight = permission_weight; cp.p = p;
    size_t size = sizeof cp;
    auto icr = *(this -> ignoredCenterRequests);
    for(std::pair<int, float>& req: icr){
        cp.req_p = req.second;
        pthread_mutex_lock(&(this -> mpi_mutex));
        MPI_Send(&cp, size, MPI_BYTE, req.first, C_PER, MPI_COMM_WORLD);
        pthread_mutex_unlock(&(this -> mpi_mutex));
    }
    icr.clear();
    pthread_mutex_unlock(&(this -> icrMutex));
}

//nr - nr procesu-adresata
//permission_weight - waga z jaką wysłać zgodę
void Sender::sendCenterPermission(int permission_weight, int nr, float req_p) {
    this -> incClock();
    float p = this -> getPriority();
	MSG cp;
    cp.permission_weight = permission_weight; cp.p = p; cp.req_p = req_p;
	size_t size = sizeof cp;
    pthread_mutex_lock(&(this -> mpi_mutex));
    MPI_Send(&cp, size, MPI_BYTE, nr, C_PER, MPI_COMM_WORLD);
    pthread_mutex_unlock(&(this -> mpi_mutex));
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

void Sender::ignoreWeaponRequest(std::pair<int, float> req){
    pthread_mutex_lock(&(this -> iwrMutex));
    this -> ignoredWeaponRequests -> push_back(req);
    pthread_mutex_unlock(&(this -> iwrMutex));
}

void Sender::ignoreMedicRequest(std::pair<int, float> req){
    pthread_mutex_lock(&(this -> imrMutex));
    this -> ignoredMedicRequests -> push_back(req);
    pthread_mutex_unlock(&(this -> imrMutex));
}

void Sender::ignoreCenterRequest(std::pair<int, float> req){
    pthread_mutex_lock(&(this -> icrMutex));
    this -> ignoredCenterRequests -> push_back(req);
    pthread_mutex_unlock(&(this -> icrMutex));
}

void Sender::setCenterRequest(int nr, std::pair<int, float> req){
    pthread_mutex_lock(&(this -> crMutex));
    this -> centerRequests[nr] = new std::pair<int, float> (req);
    pthread_mutex_unlock(&(this -> crMutex));
}

//---------------------Remove requests from lists upon receiving release-------------------

void Sender::removeCenterRequest(int nr){
    pthread_mutex_lock(&(this -> crMutex));
    this -> centerRequests[nr] = nullptr;
    pthread_mutex_unlock(&(this -> crMutex));
}

