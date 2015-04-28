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
space* newSpace(point* south_east, point* north_west) 
{
  space* ret = (space*)malloc(sizeof(space));

  ret->south_east = south_east;
  ret->north_west = north_west;

  return ret;
}

space* newSpaceWithCoord(int x_sw, int y_sw, int x_ne, int y_ne)
{
  return newSpace(newPoint(x_sw, y_sw), newPoint(x_ne, y_ne));
}

/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeSpace(space* sp)
{
  freePoint(sp->south_east);
  freePoint(sp->north_west);
  free(sp);

  return;
}

