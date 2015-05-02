/**
 * Projet AR - Content-Adressable Network
 * Primitives de gestion des point - Implementation
 *
 * Un point se compose de deux coordonnées x et y.
 *
 * @author Ilyas Toumlilt <toumlilt.ilyas@gmail.com>
 * @author Paul  Mabillot <paul.mabillot@etu.upmc.fr>
 *
 * @version 1.0
 */

#include <point.h>
#include <time.h>
/*******************************************************************************
 * Constructor
 ******************************************************************************/
/**
 * Constructeur
 *
 * @param x coordonnée horizontale dans [COORD_MIN_X, COORD_MAX_X[
 * @param y coordonnée verticale dans [COORD_MIN_Y, COORD_MAX_Y[
 * @return le point alloué 
 */
point newPoint(int x, int y)
{
  point ret;
  ret.x = x;
  ret.y = y;

  return ret;
}

/**
 * Constructeur d'un point avec des coordonnées aléatoires
 *
 * @return le point alloué
 */
point newRandomPoint()
{
	srand(time(NULL));
  /* @todo ne pas oublier le srand() */
  return newPoint((rand()%COORD_MAX_X)+COORD_MIN_X, 
		  (rand()%COORD_MAX_Y)+COORD_MIN_Y);
}

/*******************************************************************************
 * Destructor
 ******************************************************************************/
/**
 * Destructeur
 *
 * @param p le point à désallouer
 */
void freePoint(point* p)
{
  free(p);

  return;
}
