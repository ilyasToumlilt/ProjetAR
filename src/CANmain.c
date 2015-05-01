#include <CAN.h>

int main(int argc, char** argv)
{
  MPI_Init(&argc, &argv);

  /* init */
  CANinitialize();

  /* insert */
  CANinsert();

  /* message handling */
  printf("message Handling :)\n");
  while(1){
    CANhandleMessage();
  }

  MPI_Finalize();
}
