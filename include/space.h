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

#include <CAN.h>

struct _space{
  point* down_left;
  point* up_right;
};

/*******************************************************************************
 * Constructors
 ******************************************************************************/
space* newSpace(point* down_left, point* up_right);
space* newSpaceWithCoord(int x1, int y1, int x2, int y2);

/*******************************************************************************
 * Destructor
 ******************************************************************************/
void freeSpace(space* sp);


#endif
