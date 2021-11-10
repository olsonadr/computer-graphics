// loadobj.hpp
#ifndef LOADOBJ_HPP // include guard
#define LOADOBJ_HPP

// Includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <GL/gl.h>
#include <vector>

// Delimiters for parsing the obj file
#define OBJDELIMS " \t"

// Structs
struct OBJVertex {
    float x, y, z;
};
struct OBJNormal {
    float nx, ny, nz;
};
struct OBJTextureCoord {
    float s, t, p;
};
struct OBJface {
    int v, n, t;
};

// Prototypes
void OBJCross(float[3], float[3], float[3]);
char *ReadRestOfLine(FILE *);
void ReadObjVTN(char *, int *, int *, int *);
float OBJUnit(float[3]);
float OBJUnit(float[3], float[3]);
int LoadObjFile(char *name);
int LoadObjFileFromBlender(char *name);

#endif // #ifndef LOADOBJ_HPP