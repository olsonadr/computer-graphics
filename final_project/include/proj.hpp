// proj.hpp
#ifndef PROJ_HPP // include guard
#define PROJ_HPP


// Includes
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <vector>
#include <string>
#include <cstring>
#include <stdio.h>
#include <stdlib.h>

#define _WOWOW_PROJ_INCLUDED

#define _USE_MATH_DEFINES
#include <math.h>
#include <ctime>
// #include <cmath.h>

#ifdef _WIN32
    #include <windows.h>
    #pragma warning(disable : 4996)
#endif

// all opengl includes (gl, glew, glut, etc)
#include "opengl_includes.hpp"

// osu provided locals (helicopter, osusphere, lightinghelpers)
#include "osu.hpp"

// icosphere implementation
#include "Icosphere.h"

// fast noise lite noise library
#include "FastNoiseLite.h"


// Constants

// title of these windows:
const char *WINDOWTITLE = {"Procedural Planet (FP) -- Nicholas Olson"};
const char *GLUITITLE = {"OpenGL Procedural Planet Generation UI"};

// what the glui package defines as true and false:
const int GLUITRUE = {true};
const int GLUIFALSE = {false};

// the escape key:
#define ESCAPE 0x1b

// initial window size:
const int INIT_WINDOW_SIZE = {600};

// multiplication factors for input interaction:
//  (these are known from previous experience)
const float ANGFACT = {1.};
const float SCLFACT = {0.005f};

// minimum allowable scale factor:
const float MINSCALE = {0.05f};

// scroll wheel button values:
const int SCROLL_WHEEL_UP = {3};
const int SCROLL_WHEEL_DOWN = {4};

// equivalent mouse movement when we click a the scroll wheel:
const float SCROLL_WHEEL_CLICK_FACTOR = {5.};

// active mouse buttons (or them together):
const int LEFT = {4};
const int MIDDLE = {2};
const int RIGHT = {1};

// which projection:
enum Projections {
    ORTHO,
    PERSP
};

// which button:
enum ButtonVals {
    RESET,
    QUIT
};

// window background color (rgba):
const GLfloat BACKCOLOR[] = {0.1, 0.1, 0.1, 1.};
// const GLfloat BACKCOLOR[] = {0.2, 0.2, 0.2, 1.};

const int SCENE_ROTATE_TIME = 3000;


// non-constant global variables:
int ActiveButton; // current button that is down

int DebugOn;         // != 0 means to print debugging info
bool DrawHelpText;   // whether to draw the help text
bool fixedLighting = true;
bool THREE_DEE = true;
bool SceneRotateOn = false;
Projections WhichProjection;

int MainWindow;     // window id for main graphics window
int Xmouse, Ymouse; // mouse values

float Scale;                           // scaling factor
float Xrot, Yrot, Zrot;                // whole scene rotation angles in degrees

int msOfPriorAnimate;          // ms value of the prior animate call
int SphereArcSliceAnimateDir;  // either -1 or 1 to show which way arcs are animating
int SphereVertSliceAnimateDir; // either -1 or 1 to show which way verts are animating

Icosphere sphere(1.0f, 1, false);    // radius, subdivision, smooth
Icosphere sphere2(1.0f, 1, false);   // radius, subdivision, smooth

bool vboSupported = false;
GLuint vboId;
GLuint iboId;
GLuint vboId2;
GLuint iboId2;

GLSLProgram *IcoShader;
GLuint uniformMatrixModelView;
GLuint uniformMatrixModelViewProjection;
GLuint uniformMatrixNormal;
GLuint attribVertexPosition;
GLuint attribVertexNormal;
GLuint attribVertexTexCoord;

GLuint texId;
GLuint texId0;
GLuint texId1;
int texW, texH;
unsigned char *IcoTexture;

float sphere2_rad;
float oceanThresh;
const float oceanThreshMin = 0.7;
const float oceanThreshMax = 1.3;
const int oceanThreshSteps = 14;
const int oceanThreshDefStep = 3*14/5;
auto calcOcean = [](int step) { return ((float)step / (float)oceanThreshSteps) * (oceanThreshMax - oceanThreshMin) + oceanThreshMin; };
const float oceanThreshDefVal = calcOcean(oceanThreshDefStep);



// function prototypes:
void Animate();
void Display();
void DoRasterString(float, float, float, char *);
void DoStrokeString(float, float, float, float, char *);
float ElapsedSeconds();
void InitGraphics();
void InitMenus();
void Keyboard(unsigned char, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void Reset();
void Reset(bool);
void Resize(int, int);
void Visibility(int);

void RecalcSphereMesh(Icosphere sphere, GLuint &vboId, GLuint &iboId);
void DrawIcoSphere(Icosphere sphere, GLuint &vbo, GLuint &ibo, GLSLProgram *Shader, bool useNoise);
float GenerateNewPlanet();


#endif /* PROJ_HPP */
