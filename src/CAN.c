#include <CAN.h>

/*******************************************************************************
 * Initialization
 ******************************************************************************/
int nbProcess;
int idProcess;
void CANinitialize()
{
  /* getting total number of process */
  MPI_Comm_size(MPI_COMM_WORLD, &nbProcess);

  /* getting my process id */
  MPI_Comm_rank(MPI_COMM_WORLD, &idProcess);
}

