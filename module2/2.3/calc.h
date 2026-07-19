#ifndef CALC_H
#define CALC_H

#include <stdio.h>


float add(float x, float y);
float sub(float x, float y);
float mul(float x, float y);
float div(float x, float y);
char print_oper(int choice);

float (*select (int choice)) (float, float);

#endif