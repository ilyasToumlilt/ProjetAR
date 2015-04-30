*/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des donn�es - Header
 *
 * Une Donn�e contient une valeur, ses coordonn�es,
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabillot@etu.upmc.fr>
 *
 * @version 1.0
 */

#include <data.h>

/*******************************************************************************
 * Contructor
 ******************************************************************************/
data* newData(point* coord)
{
  data* ret = (data*)malloc(sizeof(data));

  ret->val = coord->x+coord->y;
  ret->coord = coord;
  
  return ret;
}


/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeData(data* d)
{
  freePoint(d->coord);
  free(d);

  return;
}

/*******************************************************************************
 * Data containing
 ******************************************************************************/
int isDataInCoord(data* d, int x1, int y1, int x2, int y2)
{
  return (d->coord->x >= x1 &&
	  d->coord->x <  x2 &&
	  d->coord->y >= y1 &&
	  d->coord->y <  y2) ? 1 : 0;
}

int isDataInSpace(data* d, space* sp)
{
  return isDataInCoord(d,
		       sp->down_left->x,
		       sp->down_left->y,
		       sp->up_right->x,
		       sp->up_right->y);
}

/*
 * Is src in trg's space ?
 */
int isDataInNodesSpace(data* d, node* trg)
{
  return isNodeInSpace(d, trg->area);
}
