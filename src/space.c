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

#include <space.h>

/*******************************************************************************
 * Constructors
 ******************************************************************************/
space newSpace(point south_west, point north_east) 
{
  space ret;

  ret.south_west = south_west;
  ret.north_east = north_east;

  return ret;
}

space newSpaceWithCoord(int x_sw, int y_sw, int x_ne, int y_ne)
{
  return newSpace(newPoint(x_sw, y_sw), newPoint(x_ne, y_ne));
}

/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeSpace(space* sp)
{
  freePoint(&(sp->south_west));
  freePoint(&(sp->north_east));
  free(sp);

  return;
}

