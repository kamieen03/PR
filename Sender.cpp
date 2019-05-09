#include "Sender.h"

Sender::Sender(int size, int nr) {
	this -> size = size;
	this -> nr = nr;
}

void Sender::broadcastWeaponRequest(weaponType w, float p){
	WeaponRequest wr = { .w = w, .p = p };	
	size_t size = sizeof wr;
	for(int i = 0; i < this -> getSize(); i++)
		if (i != this -> getNr())
			MPI_Send(&wr, size, MPI_BYTE, i, W_REQ, MPI_COMM_WORLD);
	return;
}

void Sender::broadcastWeaponRelease(weaponType w) {
	WeaponRelease wr = { .w = w };
	size_t size = sizeof wr;
	for(int i = 0; i < this -> getSize(); i++)
		if (i != this -> getNr())
			MPI_Send(&wr, size, MPI_BYTE, i, W_REL, MPI_COMM_WORLD);
	return;
}	


void Sender::broadcastMedicRequest(float p){
	MedicRequest mr = { .p = p };	
	size_t size = sizeof mr;
	for(int i = 0; i < this -> getSize(); i++)
		if (i != this -> getNr())
			MPI_Send(&mr, size, MPI_BYTE, i, M_REQ, MPI_COMM_WORLD);
	return;
}

void Sender::broadcastMedicRelease() {
	MedicRelease mr = {};
	size_t size = sizeof mr;
	for(int i = 0; i < this -> getSize(); i++)
		if (i != this -> getNr())
			MPI_Send(&mr, size, MPI_BYTE, i, M_REL, MPI_COMM_WORLD);
	return;
}	



void Sender::broadcastCenterRequest(int w, float p){
	CenterRequest cr = { .w = w, .p = p };	
	size_t size = sizeof cr;
	for(int i = 0; i < this -> getSize(); i++)
		if (i != this -> getNr())
			MPI_Send(&cr, size, MPI_BYTE, i, C_REQ, MPI_COMM_WORLD);
	return;
}

void Sender::broadcastCenterRelease(int w) {
	CenterRelease cr = { .w = w };
	size_t size = sizeof cr;
	for(int i = 0; i < this -> getSize(); i++)
		if (i != this -> getNr())
			MPI_Send(&cr, size, MPI_BYTE, i, C_REL, MPI_COMM_WORLD);
	return;
}	


void Sender::broadcastDeathMsg() {
	DeathMsg dm = {};
	size_t size = sizeof dm;
	for(int i = 0; i < this -> getSize(); i++)
		if (i != this -> getNr())
			MPI_Send(&dm, size, MPI_BYTE, i, DEATH, MPI_COMM_WORLD);
	return;
}

//TODO: doimplementować do metod Release wysyłanie zgód na 
//uprzednio zigonorowane requesty
