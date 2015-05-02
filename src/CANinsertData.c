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

/* 
 * Tente d'inserer nbData données dans le CAN
 */
void CANinsertDataInit(int nbData)
{
  int buf, i;
  MPI_Status status;

  for(i=0; i<nbData; i++){
    /* Création de la data */
    data d = newData(newRandomPoint());

    /* On  demande à inserer la data*/
    MPI_Send(&d, sizeof(data), MPI_BYTE, BOOTSTRAP_NODE, REQUEST_INSERT_DATA, MPI_COMM_WORLD);

    /* Attente de bonne insertion de la data */
    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, DONE_INSERT_DATA,
         MPI_COMM_WORLD, &status);
    gestionStockage(i, d);
  }
}


/*
 * Traitement d'une reception d'insertion de donnée
 */
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
    for (j = NB_STOCKAGE-1; j > 0 ; --j)
    {
      lastInsert[j] = lastInsert[j-1];
    }
    lastInsert[0] = d;
  return;
}