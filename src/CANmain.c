#include <CAN.h>
#include <CANinsertData.h>
#include <CANsearchData.h>
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
    printf("---------Insertion: OK----------\n");

    printf("---------Search:  ----------\n");
    CANsearchDataInit();
    printf("---------Search: OK----------\n");

    printf("---------Affichage:   ----------\n");
    displayInit(nbProcess, "P2P_Viewer/trace/test-1.txt");
    printf("---------Affichage: OK----------\n");

    printf("---------Remove:   ----------\n");

    printf("---------Remove: OK----------\n");

    printf("---------Affichage:   ----------\n");
    displayInit(nbProcess, "P2P_Viewer/trace/test-2.txt");
    printf("---------Affichage: OK----------\n");

    CANend();

  }else{
    /* message handling */
    while(ret >= 0){
      ret = CANhandleMessage();
    }  
  }
  MPI_Finalize();

  return 1;
}
