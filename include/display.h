#ifndef __display_H_
#define __display_H_

#include <CAN.h>

#include <point.h>
#include <space.h>
#include <node.h>
#include <data.h>
#include <list_node.h>

#define SIZE_INFO 256

void handleDisplayRequest();
void displayInit(int nbProcess);

#endif
