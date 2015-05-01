/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des noeuds - Implémentation
 *
 * Un Noeud contient un identifiant unique, ses coordonnées,
 * l'espace dont il s'occupe, et la liste de ses voisins
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabilot@etu.upmc.fr>
 *
 * @version 1.0
 */

#include <node.h>

/*******************************************************************************
 * Contructor
 ******************************************************************************/
node newNode(int id, point coord, space area)
{
  node ret;

  ret.id = id;
  ret.coord = coord;
  ret.area = area;
  int i;
  for(i=0; i<NB_DIRECTIONS; i++)
    ret.neighbors[i]=NULL;
  return ret;
}

node newNodeWithRandomPoint(int id)
{
  return newNode(id, newRandomPoint(), 
		 newSpace(newRandomPoint(),newRandomPoint()));
}

/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeNode(node* n)
{
  freePoint(&(n->coord));
  freeSpace(&(n->area));
  int i;
  for(i=0; i<NB_DIRECTIONS; i++)
    freeListNode(n->neighbors[i]);
  free(n);

  return;
}

/*******************************************************************************
 * Node containing
 ******************************************************************************/
int isNodeInCoord(node* src, int x1, int y1, int x2, int y2)
{
  return (src->coord.x >= x1 &&
	  src->coord.x <  x2 &&
	  src->coord.y >= y1 &&
	  src->coord.y <  y2) ? 1 : 0;
}

int isNodeInSpace(node* src, space* sp)
{
  return isNodeInCoord(src,
		       sp->south_west.x,
		       sp->south_west.y,
		       sp->north_east.x,
		       sp->north_east.y);
}

/*
 * Is src in trg's space ?
 */
int isNodeInNodesSpace(node* src, node* trg)
{
  return isNodeInSpace(src, &(trg->area));
}

int isPointInNodesSpace(point* p, node* n)
{
  return (p->x >= n->area.south_west.x &&
	  p->x <  n->area.north_east.x &&
	  p->y >= n->area.south_west.y &&
	  p->y <  n->area.north_east.y) ? 1 : 0;
}

space getNodesSubSpace(node* src)
{
  int width  = src->area.north_east.x - src->area.south_west.x;
  int height = src->area.north_east.y - src->area.south_west.y;
  
  if( height > width ){
    if( isNodeInCoord(src, src->area.south_west.x, src->area.south_west.y,
		      src->area.north_east.x, src->area.south_west.y + (height/2)) ){
      return newSpaceWithCoord(src->area.south_west.x,
			       src->area.south_west.y + (height/2),
			       src->area.north_east.x,
			       src->area.north_east.y);
    } else {
      return newSpaceWithCoord(src->area.south_west.x,
			       src->area.south_west.y,
			       src->area.north_east.x,
			       src->area.south_west.y + (height/2));
    }
  } else {
     if( isNodeInCoord(src, src->area.south_west.x, src->area.south_west.y,
		       src->area.south_west.x + (width/2), src->area.north_east.y) ){
       return newSpaceWithCoord(src->area.south_west.x + (width/2),
				src->area.south_west.y,
				src->area.north_east.x,
				src->area.north_east.y);
     } else {
       return newSpaceWithCoord(src->area.south_west.x,
				src->area.south_west.y,
				src->area.south_west.x + (width/2),
				src->area.north_east.y);
    }
  }
}

space splitNodesSpace(node* src)
{
  int width  = src->area.north_east.x - src->area.south_west.x;
  int height = src->area.north_east.y - src->area.south_west.y;

  if( height > width ){
    if( isNodeInCoord(src, src->area.south_west.x, src->area.south_west.y,
		      src->area.north_east.x, src->area.south_west.y + (height/2)) ){
      src->area.north_east.y = src->area.south_west.y + (height/2);
      return newSpaceWithCoord(src->area.south_west.x,
			       src->area.north_east.y,
			       src->area.north_east.x,
			       src->area.north_east.y + (height/2));
    } else {
      src->area.south_west.y += (height/2);
      return newSpaceWithCoord(src->area.south_west.x,
			       src->area.south_west.y - (height/2),
			       src->area.north_east.x,
			       src->area.south_west.y);
    }
  } else {
     if( isNodeInCoord(src, src->area.south_west.x, src->area.south_west.y,
		       src->area.south_west.x + (width/2), src->area.north_east.y) ){
       src->area.north_east.x -= (width/2);
       return newSpaceWithCoord(src->area.north_east.x,
				src->area.south_west.y,
				src->area.north_east.x + (width/2),
				src->area.north_east.y);
     } else {
       src->area.south_west.x += (width/2);
       return newSpaceWithCoord(src->area.south_west.x - (width/2),
				src->area.south_west.y,
				src->area.south_west.x,
				src->area.north_east.y);
    }
  }
}
