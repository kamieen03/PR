#include "Sender.h"

Sender::Sender(int size, int nr, std::list<std::pair<int, weaponType>>* ignoredWeaponRequests,
	std::list<int>* ignoredMedicRequests, std::list<int>* ignoredCenterRequests,
	std::pair<int, float>** centerRequests){
	this -> N = size;
	this -> nr = nr;
	this -> ignoredWeaponRequests = ignoredWeaponRequests;
	this -> ignoredMedicRequests = ignoredMedicRequests;
	this -> ignoredCenterRequests = ignoredCenterRequests;
	this -> centerRequests = centerRequests;
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




//-----------------------------private--------------------------------



//w - broń zwolniona przez proces
void Sender::sendWeaponPermission(weaponType w) {
	auto iwr = this -> ignoredWeaponRequests;
	for (auto it = iwr -> begin(); it != iwr -> end(); it++){
		std::pair<int, weaponType> req = *it;
		int nr = req.first; weaponType req_w = req.second; 
		if (w == req_w){
			iwr -> erase(it);
			break;
		}
	}

	int temp = 1;
        MPI_Send(&temp, 1, MPI_INT, nr, W_PER, MPI_COMM_WORLD);
	return;
}

//nr - nr procesu-adresata
void Sender::sendMedicPermission() {
        int nr = this->ignoredMedicRequests -> front();
        this -> ignoredWeaponRequests -> pop_front();

	int temp = 1;
        MPI_Send(&temp, 1, MPI_INT, nr, M_PER, MPI_COMM_WORLD);
	return;
}

//nr - nr procesu-adresata
//permission_weight - waga z jaką wysłać zgodę
void Sender::sendCenterPermission(int permission_weight) {
        int nr = this->ignoredCenterRequests -> front();
        this -> ignoredCenterRequests -> pop_front();

	int temp = permission_weight;
        MPI_Send(&temp, 1, MPI_INT, nr, C_PER, MPI_COMM_WORLD);
	return;
}

//my_p - priorytet tego procesu
int Sender::countCenterPermissions(float my_p){
	int N = this -> getN();
	std::pair<int, float>* cr;
	int sum = 0;

	for(int i = 0; i < this -> N; i++){
		cr = this -> centerRequests[i];
		if (cr -> second < my_p )
			sum += W_MAX - cr -> first;
	}
	return sum;
}

//TODO: mutexy na dostępach do zmiennych wsþółdzielonych
//TODO: zabezpieczyć funkcje sendPermission przed przypadkiem pustych list
//TODO: update clock

