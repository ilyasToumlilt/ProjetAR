/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des espaces - Header
 *
 * Un espace se compose de deux points ( nord-est et sud-ouest )
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabilot@etu.upmc.fr>
 *
 * @version 1.0
 */

#ifndef __SPACE_H__
#define __SPACE_H__

#include <point.h>

typedef struct _space space;
struct _space{
  point south_west;
  point north_east;
};

#include <CAN.h>

/*******************************************************************************
 * Constructors
 ******************************************************************************/
space newSpace(point south_west, point north_east);
space newSpaceWithCoord(int x_sw, int y_sw, int x_ne, int y_ne);

/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeSpace(space* sp);


#endif
