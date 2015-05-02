#include <CANsearchData.h>
#include <CANinsertData.h>

#include <point.h>
#include <space.h>
#include <node.h>
#include <data.h>
#include <list_node.h>

/*******************************************************************************
 * Private Declaration
 ******************************************************************************/
void CANsearchDataInit();
void CANhandleSearchDataRequest(data* n);

void CANsearchDataInit()
{
  int buf, i;
  data d;
  MPI_Status status;

  for(i=0; i<NB_STOCKAGE*2; i++){
    if(i < NB_STOCKAGE){
      d = getFirstInsert()[i];
    }else{
      d = getLastInsert()[(i/2)];
    }

    /* On  lance la recherche */
    MPI_Send(&d, sizeof(data), MPI_BYTE, BOOTSTRAP_NODE, REQUEST_SEARCH_DATA, MPI_COMM_WORLD);
      
    /* Attente de bonne insertion de la data */
    MPI_Recv(&buf, 1, MPI_INT, BOOTSTRAP_NODE, MPI_ANY_TAG,
	       MPI_COMM_WORLD, &status);

    if(status.MPI_TAG == FAILED_SEARCH_DATA){
      printf("Donnée absente\n");
    }else{
      printf("Donnée présente sur le noeud: %d\n", buf);
    }
  }
}


/* Reception d'une demande de recherche*/
void CANhandleSearchDataRequest(data* d)
{  
  int buf = getNode().id;
  node n = getNode();
  fflush(NULL);
  /* la data fait-il partie de mon espace ? */
  if( isPointInNodesSpace(&(d->coord), &n) ){
   /* Si oui on verifie sa presence dans le tableau */
    if(containData(n, *d) != -1)
    {
      MPI_Send(&buf, 1, MPI_INT, INIT_NODE, DONE_INSERT_DATA, MPI_COMM_WORLD);  
    }else{
      MPI_Send(&buf, 1, MPI_INT, INIT_NODE, FAILED_SEARCH_DATA, MPI_COMM_WORLD);
    }
  }
  else {
    /* redirection d'insertion */
    int trg = findInsertDirection(&(d->coord), &n);
    if( trg >= NB_DIRECTIONS )
      trg = chooseDirectionRandomly(trg);
    
	    MPI_Send(d, sizeof(data), MPI_BYTE, n.neighbors[trg].idList[0], 
		     REQUEST_SEARCH_DATA, MPI_COMM_WORLD);
    
  }
  
  return;
}
