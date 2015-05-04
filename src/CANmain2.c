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

  /* myId */
  int myId, nbProcess;
  MPI_Comm_rank(MPI_COMM_WORLD, &myId);
  MPI_Comm_size(MPI_COMM_WORLD, &nbProcess);

  srand((unsigned)time(NULL)+getpid()*myId+nbProcess);

  /* insert */
  CANinsert();

  if(myId == INIT_NODE){
    printf("*******************\n");
    printf("%d nodes à ajouter\n", nbProcess);
    printf("%d datas à ajouter\n", nbProcess*10);
    printf("*******************\n");

    /* Action du noeud init */
    printf("---------Debut Insertion----------\n");
    CANinsertDataInit(nbProcess*10); 
    printf("--------- Fin Insertion ----------\n");

    printf("---------Debut Search----------\n");
     CANsearchDataInit(); 
    printf("--------- Fin Search ----------\n");

    printf("---------Debut Affichage----------\n");
    displayInit(nbProcess, "P2P_Viewer/trace/test-1.txt");
    printf("--------- Fin Affichage ----------\n");

    printf("---------Debut Remove----------\n");
    CANremoveNode(5);
    printf("--------- Fin Remove ----------\n");

    printf("---------Debut Affichage----------\n");
    displayInit(nbProcess, "P2P_Viewer/trace/test-2.txt");
    printf("--------- Fin Affichage ----------\n");

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
