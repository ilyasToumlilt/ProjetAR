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

  srand((unsigned)time(NULL)+getpid()*myId+nbProcess);

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
    /* Tire une node aleatoirement puis le retire */
    /* Lance une recherche sur une des anciennes data du noeud
     Pour voir si elle a changé de pere */

    printf("--------- Fin Remove ----------\n");

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