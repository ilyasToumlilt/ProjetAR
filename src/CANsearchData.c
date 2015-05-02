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
void searchRandom(int nb);
void searchData(data d);

/* 
 * Lance la recherche sur les NB_STOCKAGE premieres data ajoutés 
 * et les NB_STOCKAGE dernieres
 */
void CANsearchDataInit()
{
  int i;
  data d;

  for(i=0; i<NB_STOCKAGE*2; i++){
    if(i < 5){
      /* Premieres datas inserées */
      d = getFirstInsert()[i];
    }else{
      /* Dernieres datas inserées */
      d = getLastInsert()[(i/2)];
    } 

    searchData(d);

  }

  searchRandom(4);
}


/**
 * Traitement d'une reception de demande de recherche
 */
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

/**
 * Recherche une data specifique
 */
void searchData(data d){
  int buf;
  MPI_Status status;
    /* On  lance la recherche */
    MPI_Send(&d, sizeof(data), MPI_BYTE, BOOTSTRAP_NODE, REQUEST_SEARCH_DATA, MPI_COMM_WORLD);
      
    /* Attente de bonne insertion de la data */
    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
         MPI_COMM_WORLD, &status);

    if(status.MPI_TAG == FAILED_SEARCH_DATA){
      printf("Donnée absente: x: %d, y: %d\n", d.coord.x, d.coord.y);
    }else{
      printf("Donnée présente sur le noeud: %d\n", buf);
    }
  }   

/**
 * Recherche nb data aleatoire
 */
void searchRandom(int nb){
  int buf, i;
  data d;
  MPI_Status status;

  for(i=0; i<nb; i++){
    d = newData(newRandomPoint());

    /* On  lance la recherche */
    MPI_Send(&d, sizeof(data), MPI_BYTE, BOOTSTRAP_NODE, REQUEST_SEARCH_DATA, MPI_COMM_WORLD);
      
    /* Attente de bonne insertion de la data */
    MPI_Recv(&buf, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
         MPI_COMM_WORLD, &status);

    if(status.MPI_TAG == FAILED_SEARCH_DATA){
      printf("Donnée absente: x: %d, y: %d\n", d.coord.x, d.coord.y);
    }else{
      printf("Donnée présente sur le noeud: %d\n", buf);
    }
  }  
}