#include "Hunter.h"
#include "Printer.h"
#include <mpi.h>


int initialize_hunter(int size, int rank) {
	Hunter* h = new Hunter(size, rank);
	h -> mainLoop();
	return 0;
}

int main(int argc, char** argv) {
    int provided;
	MPI_Init_thread(&argc, &argv, MPI_THREAD_MULTIPLE, &provided);
    //MPI_Init(&argc, &argv);

	int size,rank;
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &size);

    srand(rank*1337 + time(NULL));
	initialize_hunter(size, rank);
    MPI_Finalize();

	return 0;
}
