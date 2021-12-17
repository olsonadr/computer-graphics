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
#include <opengl_includes.hpp>

// osu provided locals (helicopter, osusphere, lightinghelpers)
#include <osu.hpp>

// icosphere implementation
#include <Icosphere.h>

// fast noise lite noise library
#include <FastNoiseLite.h>

// skybox cubemap texture object
#include <cubemap.hpp>


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
const int INIT_WINDOW_SIZE = {1000};
// const int INIT_WINDOW_SIZE = {1200};
// const int INIT_WINDOW_SIZE = {600};

// multiplication factors for input interaction:
//  (these are known from previous experience)
const float ANGFACT = {0.6};
// const float ANGFACT = {1.};
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
const float rat = 0.02;
const GLfloat BACKCOLOR[] = {rat, rat, rat, rat};
// const GLfloat BACKCOLOR[] = {0.1, 0.1, 0.1, 1.};
// const GLfloat BACKCOLOR[] = {0.2, 0.2, 0.2, 1.};

const int SCENE_ROTATE_TIME = 6000;
float SceneRotateSpeed = 1.;
float MinSceneRotateSpeed = 0;
float MaxSceneRotateSpeed = 10.;



// non-constant global variables:
int ActiveButton; // current button that is down

int DebugOn;         // != 0 means to print debugging info
bool DrawHelpText;   // whether to draw the help text
bool fixedLighting = false;
bool THREE_DEE = true;
bool SceneRotateOn = false;
Projections WhichProjection;

int MainWindow;     // window id for main graphics window
int Xmouse, Ymouse; // mouse values

float Scale;                           // scaling factor
float Xrot, Yrot, Zrot;                // whole scene rotation angles in degrees
float TXrot, TYrot;                    // whole scene target rotation angles in degrees (smooth turning)
float OXrot, OYrot, OZrot;             // planet rotation angles in degrees
float TOXrot, TOYrot;                    // planet target rotation angles in degrees (smooth turning)

int msOfPriorAnimate;          // ms value of the prior animate call
int SphereArcSliceAnimateDir;  // either -1 or 1 to show which way arcs are animating
int SphereVertSliceAnimateDir; // either -1 or 1 to show which way verts are animating

Icosphere sphere(1.0f, 5, true);    // radius, subdivision, smooth
Icosphere sphere2(1.0f, 5, true);   // radius, subdivision, smooth
Icosphere sphere3(1.5f, 5, true);   // radius, subdivision, smooth

bool vboSupported = false;
GLuint vboId, iboId;
GLuint vboId2, iboId2;
GLuint vboId3, iboId3;

enum ShaderSel { ICO, ATM, FBO };

GLSLProgram *IcoShader;
GLint uniformMatrixModelView;
GLint uniformMatrixModelViewProjection;
GLint uniformMatrixNormal;
GLint attribVertexPosition;
GLint attribVertexNormal;
GLint attribVertexTexCoord;
bool useSSAO;

GLSLProgram *AtmShader;
GLint uniformAtmMatrixModelView;
GLint uniformAtmMatrixModelViewProjection;
GLint uniformAtmMatrixNormal;
GLint attribAtmVertexPosition;
GLint attribAtmVertexNormal;
GLint attribAtmVertexTexCoord;

GLSLProgram *DepthShader;
GLint uniformFBOMatrixModelView;
GLint uniformFBOMatrixModelViewProjection;
GLint uniformFBOMatrixNormal;
GLint attribFBOVertexPosition;
GLint attribFBOVertexNormal;
GLint attribFBOVertexTexCoord;
GLuint DepthTexture;
bool DepthTextureSet = false;
GLuint OpticalDepthTexture;
bool OpticalDepthTextureSet = false;
GLuint fbo;
float depthMin = 2;
float depthMax = 10;
GLuint NoiseTextureIdx = 0;
GLuint DepthTextureIdx = 1;
GLuint SkyTextureIdx = 2;
GLuint OpticalDepthTextureIdx = 3;
bool useBakedOpticalDepth;
// float depthMin = 0.1;
// float depthMax = 1000;

GLuint texId;
GLuint texId0;
GLuint texId1;
int texW, texH;
unsigned char *IcoTexture;

float sphere2_rad = 1;
float sphere3_rad = 1;
float oceanThresh;
const float oceanThreshMin = 0.7;
const float oceanThreshMax = 1.3;
const int oceanThreshSteps = 14;
const int oceanThreshDefStep = 3*14/5;
auto calcOcean = [](int step) { return ((float)step / (float)oceanThreshSteps) * (oceanThreshMax - oceanThreshMin) + oceanThreshMin; };
const float oceanThreshDefVal = calcOcean(oceanThreshDefStep);
float aveNoise = 0;
float maxNoise = 0;
float minNoise = 0;
int resizeTimer = 0;
int RESIZE_TIMER_VAL = 100;
// int RESIZE_TIMER_VAL = 500;


// skybox stuff (cubemap generated using https://tools.wwwtyro.net/space-3d/index.html)
GLSLProgram *SkyShader;
GLuint attribPosition;
GLuint skyTexId;
CubemapTexture *skyTex;
const std::string &SkyPosXFilename = "right.bmp";
const std::string &SkyNegXFilename = "left.bmp";
const std::string &SkyPosYFilename = "bottom.bmp";
const std::string &SkyNegYFilename = "top.bmp";
const std::string &SkyPosZFilename = "front.bmp";
const std::string &SkyNegZFilename = "back.bmp";
Icosphere skySphere(1.0f, 4, true);
GLuint vboIdSky;
GLuint iboIdSky;
float eyeX = 0, eyeY = 0, eyeZ = 5;
// All of these must have same order as Skies
const int NumSkies = 6;
enum Skies                      { NORMAL,                       BLUEGREEN,                      PINK,                           GREEN,                          BLACKMIN,                     BLACKMORE,                    };
const std::string SkyPaths[] =  { "obj/skyboxes/normal",        "obj/skyboxes/bluegreen",       "obj/skyboxes/pink",            "obj/skyboxes/green",           "obj/skyboxes/blackmin",      "obj/skyboxes/blackmore",     };
const glm::vec3 SkyLightPos[] = { glm::vec3(-1.5, 1, -1),       glm::vec3(-1.5, -.2, 1),        glm::vec3(-1, 0.2, -1),         glm::vec3(-1, 0, 0),            glm::vec3(-2, -2, 2),         glm::vec3(-2, -2, 2),         };
const glm::vec4 SkyLightCol[] = { glm::vec4(1, 1, 1, 1),        glm::vec4(.2, .8, .3, 1),       glm::vec4(1., .9, 1, 1),        glm::vec4(.7, 1, .7, 1),        glm::vec4(1, 1, 1, 1),        glm::vec4(1, 1, 1, 1),        };
const glm::vec4 SkyAmbCol[] =   { glm::vec4(1, 1, 1, .5*.3),    glm::vec4(.1, 0, .8, .5*.3),    glm::vec4(1, .1, .6, .8*.3),    glm::vec4(.2, .7, 0, .7*.3),    glm::vec4(1, 1, 1, .5*.3),    glm::vec4(1, 1, 1, .5*.3),    };
// const glm::vec4 SkyAmbCol[] =   { glm::vec4(1, 1, 1, .5),   glm::vec4(.1, 0, .8, .5),   glm::vec4(1, .1, .6, .8),   glm::vec4(.2, .7, 0, .7), };
Skies CurrSky = NORMAL;
CubemapTexture **skyTexArr;



// camera stuff
bool orbitCam;
float camX, camY, camZ;
float camTX, camTY, camTZ;
float camPitch, camYaw;
float camTPitch, camTYaw;
glm::vec3 camFwdInit(0.f,0.f,-1.f);
glm::vec3 camRghtInit(1.f,0.f,0.f);
glm::vec3 camUpInit(0.f,1.f,0.f);
glm::vec3 camVFwd(0.f,0.f,-1.f);
glm::vec3 camVRght(-1.f,0.f,0.f);
glm::vec3 camVUp(0.f,1.f,0.f);
glm::vec3 camMFwd(0.f,0.f,-1.f);
glm::vec3 camMRght(-1.f,0.f,0.f);
glm::vec3 camMUp(0.f,1.f,0.f);
bool trapMouse;
int MouseProcessingTimer;
int MouseTrapTimer;
float trapMouseRad;
int camFwdMove, camRghtMove, camUpMove;  // 1 fwd, 0 stop, -1 back
const float CAM_WASD_SPEED = 1.5; // units per second

// Timer based animation and rendering
int elapsedMS = 0;
const float DESIRED_FR = 144;
const int ANIMATE_TIMER = 1000.f / DESIRED_FR;
bool DrawFPS;

// Test Quad stuff
GLSLProgram *TestQuadShader;
GLuint testQuadAttribPosIdx;


// function prototypes:
void Animate();
void AnimateTimer(int); // wrapper for animate to use with timer
void Display();
void DoRasterString(float, float, float, char *);
void DoStrokeString(float, float, float, float, char *);
float ElapsedSeconds();
void InitGraphics();
void InitMenus();
void Keyboard(unsigned char, int, int);
void KeyboardUp(unsigned char, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void Reset();
void Reset(bool);
void Reset(int);
void Resize(int, int);
void Visibility(int);

void RecalcSphereMesh(Icosphere sphere, GLuint &vboId, GLuint &iboId);
void DrawIcoSphere(Icosphere sphere, GLuint &vbo, GLuint &ibo, GLSLProgram *Shader, bool useNoise, ShaderSel sel);
void DrawIcoSpherePreGLM(Icosphere sphere, GLuint &vbo, GLuint &ibo, GLSLProgram *Shader, glm::mat4 worldMat, glm::mat4 modelMat, glm::mat4 viewMat, glm::mat4 projMat, bool useNoise, ShaderSel sel);
void DrawIcoSpherePostGLM(Icosphere sphere, GLuint &vbo, GLuint &ibo, GLSLProgram *Shader, glm::mat4 M, glm::mat4 MV, glm::mat4 MVP, glm::mat4 WV, bool useNoise, ShaderSel sel);
float GenerateNewPlanet();

void DrawSkyBox();
void DrawAtmSphere();
void DrawAtmSpherePostGLM(glm::mat4 M, glm::mat4 MV, glm::mat4 MVP, glm::mat4 WV, glm::mat4 P);
void SetShaderUniforms(GLSLProgram *Shader, bool lite = false);

void GenOpticalDepthTex();
float BakedOpticalDepth(int x, int y, int dim);
float BakedDensityAtPoint(glm::vec3 samplePoint, glm::vec3 planetCenter, float planetRad, float atmosphereRad);
bool inSphere(glm::vec3 point, glm::vec3 center, float rad);

GLuint setDepthTexture();
float WrapVal(float, float, float);
void UpdateCamDirs();
void DrawTestQuad(int TextureIdx, GLuint Texture, bool blend);

void DoMainMenu(int id);

#endif /* PROJ_HPP */
