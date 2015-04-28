#include <CAN.h>
#include <mpi.h>

int main(int argc, char** argv)
{
  MPI_Init(&argc, argv);

  /* getting total number of process */
  int nbProcess;
  MPI_Comm_size(MPI_COMM_WORLD, &nbProcess);

  /* getting my process id */
  int idProcess;
  MPI_Comm_rank(MPI_COMM_WORLD, &idProcess);

  
  
  MPI_Finalize();
}
