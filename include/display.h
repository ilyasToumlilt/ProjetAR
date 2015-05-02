#ifndef __display_H_
#define __display_H_

#include <CAN.h>

#include <point.h>
#include <space.h>
#include <node.h>
#include <data.h>
#include <list_node.h>

#define SIZE_INFO 256

/* 
 * Demande d'affichage
 */
void handleDisplayRequest();

/* 
 * Reception d'une demande d'affichage
 */
void displayInit(int nbProcess, char* file);

#endif
