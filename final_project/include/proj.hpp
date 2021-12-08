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


// Structs for objects and lights
struct object
{
    GLuint List;                     // object's display list
    float BR = .8, BG = .8, BB = .8; // object's back color as RGB
    float R = 1., G = 1., B = 1.;    // object's front color as RGB
    float X = 0., Y = 0., Z = .0;    // object's x,y,z translation
    float Xr = 0., Yr = 0., Zr = 0.; // object's rotation about x,y,z
    float Scale = 1.0;               // object's scale for x,y,z
    float ambient = 1.;              // object's ambient reflection
    float diffuse = 1.;              // object's diffuse reflection
    float specular = 0.4;            // object's specular reflection
    float shiny = 1.;                // object's shininess
};
struct light
{
    // float* color = Array3(1., 1., 1.); // light's color
    // float* pos = Array3(0., 0., 0.);   // light's x,y,z pos
    float X = 0., Y = 0., Z = 0.; // light's pos as XYZ
    float R = 1., G = 1., B = 1.; // light's color as RGB
    float ambient = 0.;           // light's ambient component
    float diffuse = 1.;           // light's diffuse component
    float specular = 1.;          // light's specular component
    GLuint num;
};
struct spotlight : public light
{
    float DX = 0., DY = -1., DZ = 0.; // spotlight's direction
    float exp = 1.;                   // directional intensity
    float spread = 30.;               // spotlight spread angle
};
struct bezierpoint
{
    float x0, y0, z0;   // original coords
    float x, y, z;      // animated coords
};
struct beziercurve
{
    struct bezierpoint p0, p1, p2, p3; // the bezier points for the curve
    glm::vec3 color;
    float colorPercent = 1;
    int t = 50;
};


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

// size of the 3d box:
const float BOXSIZE = {2.f};

// 3d sphere parameters:
const float SPHERE_RADIUS = {1.f};
const int SPHERE_MIN_VERT = {2};
const int SPHERE_MIN_ARC = {3};
const int SPHERE_MAX_VERT = {60}; // different from eachother so they cycle
const int SPHERE_MAX_ARC = {50};  // around to get a full range of combinations
const int SPHERE_DEF_VERT = {10};
const int SPHERE_DEF_ARC = {12};

// rotate speeds and overall sphere rotate time
const int SPHERE_ROTATE_TIME = 1000;
const int SPHERE_SLICE_CYCLE_TIME = 8000;
const float SPHERE_ROTATE_Z_SPEED = 0.03f;
const float SPHERE_ROTATE_Y_SPEED = 0.12f;
const float SPHERE_ROTATE_X_SPEED = 0.07f;

// helicoptor and blade parameters
#define BLADE_RADIUS 1.0
#define BLADE_WIDTH 0.4
const int BLADE_SPIN_TIME = 500;
const float TAIL_SPEED_RATIO = 2.;

// lit object and light animation parameters
const int OBJECT_ROT_TIME = 4000;
const int LIGHT_ROT_TIME = 3000;

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
enum Projections
{
    ORTHO,
    PERSP
};

// which button:
enum ButtonVals
{
    RESET,
    QUIT
};

// window background color (rgba):
const GLfloat BACKCOLOR[] = {0.1, 0.1, 0.1, 1.};
// const GLfloat BACKCOLOR[] = {0.2, 0.2, 0.2, 1.};

// line width for the axes:
const GLfloat AXES_WIDTH = {3.};

// the color numbers:
// this order must match the radio button order
enum Colors
{
    RED,
    YELLOW,
    GREEN,
    CYAN,
    BLUE,
    MAGENTA,
    WHITE,
    BLACK
};

char *ColorNames[] = {
    (char *)"Red",
    (char *)"Yellow",
    (char *)"Green",
    (char *)"Cyan",
    (char *)"Blue",
    (char *)"Magenta",
    (char *)"White",
    (char *)"Black"};

// the color definitions:
// this order must match the menu order
const GLfloat Colors[][3] = {
    {1., 0., 0.}, // red
    {1., 1., 0.}, // yellow
    {0., 1., 0.}, // green
    {0., 1., 1.}, // cyan
    {0., 0., 1.}, // blue
    {1., 0., 1.}, // magenta
    {1., 1., 1.}, // white
    {0., 0., 0.}, // black
};

// the vert and arc slice definitions
const GLint VertSlices[] = {
    2, 3, 4, 5, 6, 10, 20, 50, 100};
const GLint ArcSlices[] = {
    2, 3, 4, 5, 6, 10, 20, 50, 100};

// fog parameters:
const GLenum FOGMODE = {GL_LINEAR};
const GLfloat FOGCOLOR[4] = {.0, .0, .0, 0.5};
const GLfloat FOGDENSITY = {0.10f};
const GLfloat FOGSTART = {5};
const GLfloat FOGEND = {25.};

// view mode constants
const int OUTSIDE_VIEW = 0;
const int INSIDE_VIEW = 1;

// texture constants
char *TEXTURE_PATH = (char *)"obj/chopper.bmp";
// char *TEXTURE_PATH = (char *)"obj/chopper_transparent.bmp";
int TEXTURE_WIDTH = 0;
int TEXTURE_HEIGHT = 0;
// int TEXTURE_WIDTH = 235;
// int TEXTURE_HEIGHT = 190;
const int DISTORT_TEX_TIME = 1000;
const char *TEX_DISPLAY_OPTIONS[] = {"No Texture", "Tex Replaced", "Tex Modulated"};
const char *TEX_DISTORT_OPTIONS[] = {"No Animation", "Small Animation", "Whoosh Animation"};

// shader constants
const GLuint POS_INDEX = 0;
const GLuint TEX_INDEX = 1;
const int VERT_ANIM_TIME = 1000;
const int FRAG_ANIM_TIME = 2000;
const int SHADER_CHECK_MIN = {2};   // min number of checks on the shader checkerboard
const int SHADER_CHECK_MAX = {1000}; // max number of checks on the shader checkerboard

// non-constant global variables:
int ActiveButton; // current button that is down

int OriginAxesOn;    // != 0 means to draw the origin axes
int FrontAxesOn;     // != 0 means to draw the front scene fixed axes
int SphereAxesOn;    // != 0 means to draw the front scene rotated axes
int OriginAxesScale; // size of the origin axes
int DebugOn;         // != 0 means to print debugging info
int DepthCueOn;      // != 0 means to use intensity depth cueing
int DepthBufferOn;   // != 0 means to use the z-buffer
int DepthFightingOn; // != 0 means to force the creation of z-fighting
int ShadowsOn;       // != 0 means to turn shadows on
int WhichColor;      // index into Colors[ ]
int WhichProjection; // ORTHO or PERSP
bool DrawHelpText;   // whether to draw the help text
bool DrawSlicesText; // whether to draw the slices text
bool SphereRotateOn;
bool BladesRotateOn;
bool SphereSliceAnimateOn;
bool WireframeMode;
bool DistortTexOn;
bool NewBladesOn;

GLuint AxesList;         // list to hold the axes
GLuint BoxList;          // box display list
GLuint SphereList;       // sphere display list
GLuint HeliList;         // helicopter display list
GLuint OldBladeList;     // old helicopter blade display list
GLuint NewTopBladeList;  // new top helicopter blade display list
GLuint NewTailBladeList; // new tail helicopter blade display list

int MainWindow;     // window id for main graphics window
int Xmouse, Ymouse; // mouse values

float Scale;                           // scaling factor
float Xrot, Yrot, Zrot;                // whole scene rotation angles in degrees
float FrontXrot, FrontYrot, FrontZrot; // subscene in front of heli rotation angles in degrees
float TopBladeRot, TailBladeRot;       // heli blade rotation angles in degrees
float ObjectRot, LightRot;             // object and light rotation angles in degrees

float SphereVertSlices;
float SphereArcSlices;

int msOfPriorAnimate;          // ms value of the prior animate call
int SphereArcSliceAnimateDir;  // either -1 or 1 to show which way arcs are animating
int SphereVertSliceAnimateDir; // either -1 or 1 to show which way verts are animating

unsigned char *Texture;
GLuint Tex0;
int TexSel;
int TexDistortSel;
float DistortTexTheta;
int CurrView; // current viewmode (0=outside, 1=inside)

struct object Obj1;
struct object Obj2;
struct object Obj3;

struct light L1;     // red spinny
struct light L2;     // white stationary
struct light L3;     // blue spinny
struct spotlight L0; // white spotlight
struct object LObj1;
struct object LObj2;
struct object LObj3;
struct object LObj0;

bool L1_On;
bool L2_On;
bool L3_On;
bool L0_On;
bool ObjAnimOn;
bool LightAnimOn;

struct object HeliObject;

GLSLProgram *Pattern;  // Shader GLSLProgram object
GLSLProgram *BCShader; // Shader object for bezier curves
float ShaderVertTime;  // Time var to be used with vert shaders [0,1)
float ShaderFragTime;  // Time var to be used with frag shaders [0,1)
float ShaderNumChecks; // Number of checks in the shader checkerboard
bool VertShaderAnimOn; // Whether to animate the vert shader
bool FragShaderAnimOn; // Whether to animate the frag shader
bool ShaderRainbowOn;  // Whether to overlay rainbow with shader
int VertShaderAnimSel;  // 0 or 1 for different animations
int FragShaderColorSel; // 0 or 1 for different colors


const int NUM_RAD_BC = 5;
const int NUM_FIX_BC = 1;
const int NUM_BC_POINTS = 4;
const int WHITE_COLOR = 2;
const int GRAY_COLOR = 3;

struct beziercurve radial_curves[NUM_RAD_BC];
struct beziercurve fixed_curves[NUM_FIX_BC];
GLuint vao, vbo[4];

bool BezierAnimOn;
bool BezierPointsOn;
bool BezierLinesOn;
const int BEZ_TIME = 2*4000;
const float BC_RAD = 0.5;


//Icosphere sphere(1.0f);             // radius only
//Icosphere sphere(1.0f, 0);          // radius, subdivision
Icosphere sphere(1.0f, 1, false);    // radius, subdivision, smooth
Icosphere sphere2(1.0f, 1, false);    // radius, subdivision, smooth
// Icosphere sphere(1.0f, 0, false);    // radius, subdivision, smooth
GLuint vboId;
GLuint iboId;
GLuint vboId2;
GLuint iboId2;
GLuint attribVertex = 0;
GLuint attribNormal = 1;
GLuint attribTexCoord = 2;
GLSLProgram *IcoShader;
bool vboSupported = false;
GLuint texId;
GLuint texId0;
GLuint texId1;
int texW, texH;
unsigned char *IcoTexture;
GLuint uniformMatrixModelView;
GLuint uniformMatrixModelViewProjection;
GLuint uniformMatrixNormal;

GLuint attribVertexPosition;
GLuint attribVertexNormal;
GLuint attribVertexTexCoord;

float sphere2_rad;
float oceanThresh;
const float oceanThreshMin = 0.7;
const float oceanThreshMax = 1.3;
const int oceanThreshSteps = 14;
const int oceanThreshDefStep = 5;
auto calcOcean = [](int step) { return ((float)step / (float)oceanThreshSteps) * (oceanThreshMax - oceanThreshMin) + oceanThreshMin; };
const float oceanThreshDefVal = calcOcean(oceanThreshDefStep);

bool fixedLighting = true;
bool THREE_DEE = true;
bool SceneRotateOn = false;
const int SCENE_ROTATE_TIME = 3000;




// function prototypes:
void Animate();
void Display();
void DrawHelicopterBlades();
void DoAxesMenu(int);
void DoColorMenu(int);
void DoDepthBufferMenu(int);
void DoDepthFightingMenu(int);
void DoDepthMenu(int);
void DoDebugMenu(int);
void DoMainMenu(int);
void DoProjectMenu(int);
void DoShadowMenu();
void DoRasterString(float, float, float, char *);
void DoStrokeString(float, float, float, float, char *);
float ElapsedSeconds();
void InitGraphics();
void InitLists();
void CreateHeliList();
void DrawHeliFromObj();
void DrawHeli();
void CreateBladeList();
void UpdateSphereList();
void DrawSphere();
void InitMenus();
void Keyboard(unsigned char, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void Reset();
void Reset(bool);
void Resize(int, int);
void Visibility(int);
void DrawObjectShaded(struct object Obj,
                      GLSLProgram *Shader = Pattern,
                      bool WithAxes = false);
void DrawListTransform(GLuint, float, float, float,
                       float, float, float, float, GLenum);
void DrawListTransform(GLuint, float, float, float,
                       float, float, float, float);
void DrawObjectTransform(struct object Obj, GLenum);
void DrawObjectTransform(struct object Obj);
void UpdateLightColor(struct light l, float, float, float);
void DrawOsuSphereLit(float, int, int, struct object Obj);
void DrawOsuSphereUnlit(float, int, int, struct object Obj);
void UseObjectMaterial(struct object Obj);
void EnableLight(int, struct light l);
void EnableLight(int, struct spotlight l);
void DrawObjectFromObj(char *, struct object Obj);

void Axes(float);
unsigned char *BmpToTexture(char *, int *, int *);
void HsvRgb(float[3], float[3]);
int ReadInt(FILE *);
short ReadShort(FILE *);

void Cross(float[3], float[3], float[3]);
float Dot(float[3], float[3]);
float Unit(float[3], float[3]);

void DrawBezierCurve(struct beziercurve BC);
void ResetBezierCurve(struct beziercurve *BC);

void RotateY(struct bezierpoint* p, float, float, float, float);

void RecalcSphereMesh(Icosphere sphere, GLuint &vboId, GLuint &iboId);

void DrawIcoSphere(Icosphere sphere, GLuint &vbo, GLuint &ibo, GLSLProgram *Shader, bool useNoise);

float GenerateNewPlanet();


#endif /* PROJ_HPP */
