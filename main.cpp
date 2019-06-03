#include "Hunter.h"
#include "Printer.h"
#include <mpi.h>


int initialize_hunter(int size, int rank) {
	Hunter* h = new Hunter(size, rank);
	h -> mainLoop();
	return 0;
}

int main(int argc, char** argv) {
	MPI_Init(&argc, &argv);

	int size,rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);


	initialize_hunter(size, rank);

	return 0;
}
