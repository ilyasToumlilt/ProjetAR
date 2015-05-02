#include <CANinsertData.h>

#include <point.h>
#include <space.h>
#include <node.h>
#include <data.h>
#include <list_node.h>

/*******************************************************************************
 * Private Declaration
 ******************************************************************************/
void gestionStockage(int indexData, data d);

data lastInsert[NB_STOCKAGE];
data firstInsert[NB_STOCKAGE];
data* getLastInsert(){ return lastInsert;}
data* getFirstInsert(){ return firstInsert;}

void CANinsertDataInit()
{
  int buf, i;
  MPI_Status status;

  for(i=0; i<10; i++){
    /* Création de la data */
    data d = newData(newRandomPoint());
    printf("x: %d, y: %d\n",d.coord.x, d.coord.y);

    /* On  demande d'inserer la data*/
    MPI_Send(&d, sizeof(data), MPI_BYTE, BOOTSTRAP_NODE, REQUEST_INSERT_DATA, MPI_COMM_WORLD);

    /* Attente de bonne insertion de la data */
    MPI_Recv(&buf, 1, MPI_INT, BOOTSTRAP_NODE, MPI_ANY_TAG,
         MPI_COMM_WORLD, &status);
    gestionStockage(i, d);
  }
}


/* Reception d'une demande */
void CANhandleInsertDataRequest(data* d)
{  
  int buf;
  node n = getNode();
  fflush(NULL);
  /* la data fait-il partie de mon espace ? */
  if( isPointInNodesSpace(&(d->coord), &n) ){

    /* si oui, j'ajoute ls data a la liste de mon noeud */
    n.ld[n.nbData] = *d;
    n.nbData++;
    setNode(n);

    /* insertion reussie done */
    MPI_Send(&buf, 1, MPI_INT, INIT_NODE, DONE_INSERT_DATA, MPI_COMM_WORLD);
  }
  else {
    /* redirection d'insertion */
    int trg = findInsertDirection(&(d->coord), &n);
    if( trg >= NB_DIRECTIONS )
      trg = chooseDirectionRandomly(trg);
    
      MPI_Send(d, sizeof(data), MPI_BYTE, n.neighbors[trg].idList[0], 
         REQUEST_INSERT_DATA, MPI_COMM_WORLD);
    
  }
  
  return;
}

void gestionStockage(int indexData, data d)
{
  int j;
    if(indexData < NB_STOCKAGE){
      firstInsert[indexData] = d;
    }
    /* Decallage lastInsert */
    for (j = 0; j < NB_STOCKAGE-1; ++j)
    {
      lastInsert[j+1] = lastInsert[j];
    }
    lastInsert[0] = d;
  return;
}