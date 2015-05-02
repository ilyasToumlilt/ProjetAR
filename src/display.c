#include <display.h>

void traitementInfo(node n, FILE* file);

/* 
 * Demande l'affichage des nbProcess 
 */
void displayInit(int nbProcess, char* file)
{
  int buf, i;
  node n;
  MPI_Status status;
  FILE* fileOut;

  fileOut = fopen(file, "w+");

  /* On demande a chaque noeud ces infos */
  for(i=0; i< nbProcess; i++){
	/* On  demande d'inserer la data*/
	if(i != INIT_NODE){		
	    MPI_Send(&buf, 1, MPI_INT, i, WANT_INFO, MPI_COMM_WORLD);
	      
	    /* Attente de bonne insertion de la data */
	    MPI_Recv(&n, sizeof(node), MPI_BYTE, i, GIVE_INFO,
		       MPI_COMM_WORLD, &status);

	    traitementInfo(n, fileOut);
	}
  }
  fclose(fileOut);
}


/*
 * Reception d'une demande d'affichage
 */
void handleDisplayRequest()
{  
  node n = getNode();
  fflush(NULL);
  /* On envoi seulement le noeud */
  
	MPI_Send(&n, sizeof(node), MPI_BYTE, INIT_NODE, 
		GIVE_INFO, MPI_COMM_WORLD);
   
  return;
}

/* 
 * Ecrit les informations dans le fichier fournit en parametre
 */
void traitementInfo(node n, FILE* file){
	char res[SIZE_INFO];
	int i;
	data d;
	sprintf(res, "Node;%d;%d;%d;%d;%d;%d;%d;\n"
			,n.id, n.coord.x, n.coord.y, n.area.south_west.x, n.area.south_west.y, n.area.north_east.x, n.area.north_east.y);
	fputs(res,file);
	
	/* Pensez a parcourir les nodes */
	for (i = 0; i < n.nbData; ++i)
	{
		
		d = n.ld[i];
		sprintf(res, "Data;%d;%d;%d;\n"
				,n.id, d.coord.x, d.coord.y);
		fputs(res,file);		
	}
}