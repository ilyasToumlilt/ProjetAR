/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des espaces - Implementation
 *
 * Un espace se compose de deux points ( nord-est et sud-ouest )
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabilot@etu.upmc.fr>
 *
 * @version 1.0
 */

/*******************************************************************************
 * Constructors
 ******************************************************************************/
space* newSpace(point* down_left, point* up_right) 
{
  space* ret = (space*)malloc(sizeof(space));

  ret->down_left = down_left;
  ret->up_right  = up_right;

  return ret;
}

space* newSpaceWithCoord(int x1, int y1, int x2, int y2)
{
  return newSpace(newPoint(x1, y1), newPoint(x2, y2));
}

/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeSpace(space* sp)
{
  freePoint(sp->down_left);
  freePoint(sp->up_right);
  free(sp);

  return;
}

