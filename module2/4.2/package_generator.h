#ifndef PACKAGE_GENERATOR_H
#define PACKAGE_GENERATOR_H

#include <stdlib.h>
#include <time.h>
#include "queue.h"

void gen_package(char *buffer);
void add_package_to_q(Queue *q, int priority);
#endif