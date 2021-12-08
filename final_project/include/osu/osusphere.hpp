// osusphere.hpp
#ifndef OSUSPHERE_HPP // include guard
#define OSUSPHERE_HPP

// Includes
#include <stdio.h>
#include <math.h>
#include <GL/gl.h>
// #include "../proj.hpp" // for structs

// Globals
inline int NumLngs, NumLats;
inline struct osuSpherePoint *Pts;

// Structs
struct osuSpherePoint {
    float x, y, z;    // coordinates
    float nx, ny, nz; // surface normal
    float s, t;       // texture coords
};

// Prototypes
struct osuSpherePoint* PtsPointer(int lat, int lng);
void DrawPoint(struct osuSpherePoint *p);
void DrawPoint(struct osuSpherePoint *p, float r, float g, float b);
void OsuSphere(float radius, int slices, int stacks, float r, float g, float b);
void OsuSphere(float radius, int slices, int stacks);

#endif // #ifndef OSUSPHERE_HPP