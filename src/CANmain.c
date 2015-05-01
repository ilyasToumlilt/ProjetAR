#include <CAN.h>

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);

  /* init */
  CANinitialize();

  /* insert */
  CANinsert();

  /* myId */
  int myId;
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);

  /* message handling */
  printf("%d -> message Handling :)\n", myId);
  while(1){
    CANhandleMessage();
  }

  MPI_Finalize();
}
