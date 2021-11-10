// lightinghelpers.hpp
#ifndef LIGHTINGHELPERS_HPP // include guard
#define LIGHTINGHELPERS_HPP

// Includes
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>

// Inline constnats
inline float White[] = {1., 1., 1., 1.};

// Functions
float *Array3(float a, float b, float c);
float *MulArray3(float factor, float array0[3]);

#endif // #ifndef LIGHTINGHELPERS_HPP