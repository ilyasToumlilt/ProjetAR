#include <CAN.h>
#include <CANinsertData.h>
#include <display.h>

int main(int argc, char** argv)
{
  int ret = 0;
  MPI_Init(&argc, &argv);

  /* init */
  CANinitialize();

  /* insert */
  CANinsert();

  /* myId */
  int myId, nbProcess;
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  MPI_Comm_size(MPI_COMM_WORLD, &nbProcess);

  if(myId == INIT_NODE){
    /* Action du noeud init */
    CANinsertDataInit();

    displayInit(nbProcess);
    CANend();

  }else{
    /* message handling */
    printf("%d -> message Handling :)\n", myId);
    while(ret >= 0){
      ret = CANhandleMessage();
    }  
  }
  MPI_Finalize();

  return 1;
}
