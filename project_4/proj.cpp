// Intro to Computer Graphics - Project 4 - Lighting
// Author - Nicholas Olson
// Date -   10/20/2021
// Texture Source - https://opengameart.org/content/helicopter-3


// Includes
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

#ifdef _WIN32
    #include <windows.h>
    #pragma warning(disable : 4996)
#endif

// #include <GL/glew.h>
#include "include/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "include/glut.h"



//	The left mouse button does rotation
//	The middle mouse button does scaling
//	The user interface allows:
//		1. The axes to be turned on and off
//		2. The color of the axes to be changed
//		3. Debugging to be turned on and off
//		4. Depth cueing to be turned on and off
//		5. The projection to be changed
//		6. The transformations to be reset
//		7. The program to quit
//



// locals (helicopter, osusphere, lightinghelpers)
#include "heli.550"
#include "lightinghelpers.cpp"
#include "osusphere.cpp"

// Structs for objects and lights
struct object {
    GLuint List;            // object's display list
    float BR, BG, BB = 0.8; // object's back color as RGB
    float R, G, B = 1.0;    // object's front color as RGB
    float X, Y, Z = 0.0;    // object's x,y,z translation
    float Xr, Yr, Zr = 0.0; // object's rotation about x,y,z
    float Scale = 1.0;      // object's scale for x,y,z
};
struct light {
    // float* color = Array3(1., 1., 1.); // light's color
    // float* pos = Array3(0., 0., 0.);   // light's x,y,z pos
    float X, Y, Z = 0.;     // light's pos as XYZ
    float R, G, B = 1.;     // light's color as RGB
    float ambient = 0.;     // light's ambient component
    float diffuse = 1.;     // light's diffuse component
    float specular = 1.;    // light's specular component
};



// void UpdateLightColor(struct light l, float r, float g, float b)
// {
//     l.color = Array3(r, g, b);
// }



// Constants

// title of these windows:
const char *WINDOWTITLE = {"Texture Mapping (P3) -- Nicholas Olson"};
const char *GLUITITLE = {"Texture-Mapped Helicopter UI"};

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
const int SPHERE_MIN_ARC = {2};
const int SPHERE_MAX_VERT = {30}; // different from eachother so they cycle
const int SPHERE_MAX_ARC = {40};  // around to get a full range of combinations
const int SPHERE_DEF_VERT = {10};
const int SPHERE_DEF_ARC = {12};

// rotate speeds and overall sphere rotate time
const int SPHERE_ROTATE_TIME = 1000;
const int SPHERE_SLICE_CYCLE_TIME = 4000;
const float SPHERE_ROTATE_Z_SPEED = 0.03f;
const float SPHERE_ROTATE_Y_SPEED = 0.12f;
const float SPHERE_ROTATE_X_SPEED = 0.07f;

// helicoptor and blade parameters
#define BLADE_RADIUS	 1.0
#define BLADE_WIDTH		 0.4
const int BLADE_SPIN_TIME = 500;
const float TAIL_SPEED_RATIO = 2.;

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
const GLfloat BACKCOLOR[] = {0.2, 0.2, 0.2, 1.};

// line width for the axes:
const GLfloat AXES_WIDTH = {3.};

// the color numbers:
// this order must match the radio button order
enum Colors {
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
    (char *)"Black"
};

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
    2, 3, 4, 5, 6, 10, 20, 50, 100
};
const GLint ArcSlices[] = {
    2, 3, 4, 5, 6, 10, 20, 50, 100
};

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
char *TEXTURE_PATH = "chopper_transparent.bmp";
// char *TEXTURE_PATH = "chopper.bmp";
int TEXTURE_WIDTH = 235;
int TEXTURE_HEIGHT = 190;
const int DISTORT_TEX_TIME = 1000;
const char* TEX_DISPLAY_OPTIONS[] = {"No Texture", "Tex Replaced", "Tex Modulated"};
const char* TEX_DISTORT_OPTIONS[] = {"No Animation", "Small Animation", "Whoosh Animation"};


// non-constant global variables:
int ActiveButton;    // current button that is down

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

GLuint AxesList;     // list to hold the axes
GLuint BoxList;      // box display list
GLuint SphereList;   // sphere display list
GLuint HeliList;     // helicopter display list
GLuint BladeList;    // helicopter blade display list

int MainWindow;      // window id for main graphics window
int Xmouse, Ymouse;  // mouse values

float Scale;         // scaling factor
float Xrot, Yrot, Zrot; // whole scene rotation angles in degrees
float FrontXrot, FrontYrot, FrontZrot; // subscene in front of heli rotation angles in degrees
float TopBladeRot, TailBladeRot; // heli blade rotation angles in degrees

float SphereVertSlices;
float SphereArcSlices;

int msOfPriorAnimate; // ms value of the prior animate call
int SphereArcSliceAnimateDir; // either -1 or 1 to show which way arcs are animating
int SphereVertSliceAnimateDir; // either -1 or 1 to show which way verts are animating

unsigned char *Texture;
GLuint Tex0;
int TexSel;
int TexDistortSel;
float DistortTexTheta;
int CurrView;        // current viewmode (0=outside, 1=inside)

struct object Obj1;
struct object Obj2;
struct object Obj3;

struct light L1;
struct light L2;
struct light L3;
struct object LObj1;
struct object LObj2;
struct object LObj3;


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
void DrawHeli();
void CreateBladeList();
void UpdateSphereList();
void InitMenus();
void Keyboard(unsigned char, int, int);
void MouseButton(int, int, int, int);
void MouseMotion(int, int);
void Reset();
void Reset(bool);
void Resize(int, int);
void Visibility(int);
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

void Axes(float);
unsigned char *BmpToTexture(char *, int *, int *);
void HsvRgb(float[3], float[3]);
int ReadInt(FILE *);
short ReadShort(FILE *);

void Cross(float[3], float[3], float[3]);
float Dot(float[3], float[3]);
float Unit(float[3], float[3]);

// main program:
int main(int argc, char *argv[])
{
    // turn on the glut package:
    // (do this before checking argc and argv since it might
    // pull some command line arguments out)
    glutInit(&argc, argv);

    // setup all the graphics stuff:
    InitGraphics();

    // init all the global variables used by Display() without redisplaying:
    Reset(false);

    // create the display structures that will not change:
    InitLists();

    // init all the global variables used by Display() with a redisplay
    Reset(true);

    // setup all the user interface stuff:
    InitMenus();

    // draw the scene once and wait for some interaction:
    // (this will never return)
    glutSetWindow(MainWindow);
    glutMainLoop();

    // glutMainLoop( ) never returns
    // this line is here to make the compiler happy:
    return 0;
}

// this is where one would put code that is to be called
// everytime the glut main loop has nothing to do
//
// this is typically where animation parameters are set
//
// do not call Display( ) from here -- let glutMainLoop( ) do it
void Animate()
{
    // put animation stuff in here -- change some global variables
    // for Display( ) to find:

    // Get time since last update and enforce new val
    int ms = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMS = ms - msOfPriorAnimate;
    msOfPriorAnimate = ms;

    // Handle sphere rotation if enabled
    if (SphereRotateOn) {
        FrontZrot += 360.f * SPHERE_ROTATE_Z_SPEED * ((float)elapsedMS) / SPHERE_ROTATE_TIME;
        FrontYrot += 360.f * SPHERE_ROTATE_Y_SPEED * ((float)elapsedMS) / SPHERE_ROTATE_TIME;
        FrontXrot += 360.f * SPHERE_ROTATE_X_SPEED * ((float)elapsedMS) / SPHERE_ROTATE_TIME;
    }

    // Handle sphere slice animating if enabled
    if (SphereSliceAnimateOn) {
        // Get how far into the arc and vert animations they should be
        float arc_step = (SPHERE_MAX_ARC - SPHERE_MIN_ARC) * ((float)elapsedMS) / SPHERE_SLICE_CYCLE_TIME;
        float vert_step = (SPHERE_MAX_VERT - SPHERE_MIN_VERT) * ((float)elapsedMS) / SPHERE_SLICE_CYCLE_TIME;
        SphereArcSlices += SphereArcSliceAnimateDir * arc_step;
        SphereVertSlices += SphereVertSliceAnimateDir * arc_step;

        // Switch direction if at min or max arc
        if (SphereArcSlices > SPHERE_MAX_ARC) {
            SphereArcSliceAnimateDir = -1;
            SphereArcSlices = SPHERE_MAX_ARC;
        } else if (SphereArcSlices < SPHERE_MIN_ARC) {
            SphereArcSliceAnimateDir = 1;
            SphereArcSlices = SPHERE_MIN_ARC;
        }

        // Switch directions if at min or max vert
        if (SphereVertSlices > SPHERE_MAX_VERT) {
            SphereVertSliceAnimateDir = -1;
            SphereVertSlices = SPHERE_MAX_VERT;
        } else if (SphereVertSlices < SPHERE_MIN_VERT) {
            SphereVertSliceAnimateDir = 1;
            SphereVertSlices = SPHERE_MIN_VERT;
        }

        // Push the update through
        UpdateSphereList();        
    }

    // Inline double modulo
    auto mod = [](double x, double y) { return x - (int)(x/y) * y; };
    
    // Handle blade spinning (if enabled)!
    if (BladesRotateOn) {
        TopBladeRot = mod((TopBladeRot + 180. * float(elapsedMS) / BLADE_SPIN_TIME), 180.);
        TailBladeRot = TAIL_SPEED_RATIO * TopBladeRot;
    }

    // Handle texture distortion
    if (DistortTexOn) {
        DistortTexTheta = 2.0 * M_PI * (float) (ms % DISTORT_TEX_TIME) / DISTORT_TEX_TIME;
    }

    // force a call to Display( ) next time it is convenient:
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// draw the complete scene:

void Display()
{
    // if (DebugOn != 0) {
    //     fprintf(stderr, "Display\n");
    // }

    // set which window we want to do the graphics into:
    glutSetWindow(MainWindow);

    // erase the background:
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // specify shading to be default flat:
    glShadeModel(GL_FLAT);

    // set the viewport to a square centered in the window:
    GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
    GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
    GLsizei v = vx < vy ? vx : vy; // minimum dimension
    GLint xl = (vx - v) / 2;
    GLint yb = (vy - v) / 2;
    glViewport(xl, yb, v, v);

    // set the viewing volume:
    // remember that the Z clipping  values are actually
    // given as DISTANCES IN FRONT OF THE EYE
    // USE gluOrtho2D( ) IF YOU ARE DOING 2D !
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (WhichProjection == ORTHO)
        glOrtho(-3., 3., -3., 3., 0.1, 1000.);
    else
        gluPerspective(90., 1., 0.1, 1000.);

    // place the objects into the scene:
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // since we are using glScalef( ), be sure normals get unitized:
    glEnable(GL_NORMALIZE);

    // setup and place the lights
    // glShadeModel(GL_SMOOTH);
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, MulArray3(0.3, White));
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);


    // EnableLight(GL_LIGHT0, L1);
    // EnableLight(GL_LIGHT1, L2);
    // EnableLight(GL_LIGHT2, L3);





    // set the eye position, look-at position, and up-vector dending on view
    if (CurrView == OUTSIDE_VIEW)
        gluLookAt(-0.4, 1.8, -12., 0., 0., 0., 0., 1., 0.);
    else if (CurrView == INSIDE_VIEW)
        gluLookAt(-0.4, 1.8, -4.4+8.5, -0.4, 1.8, -4.4 - 1., 0., 1., 0.);
    else
        gluLookAt(0., 0., 3., 0., 0., 0., 0., 1., 0.);






    // uniformly rotate and scale the scene, but only if in outside view
    if (CurrView == OUTSIDE_VIEW) {
        glRotatef((GLfloat)Xrot, 1., 0., 0.);
        glRotatef((GLfloat)Yrot, 0., 1., 0.);
        glRotatef((GLfloat)Zrot, 0., 0., 1.);

        if (Scale < MINSCALE)
            Scale = MINSCALE;
        glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
    }

    // enable and configure the individual lights
    EnableLight(GL_LIGHT0, L1);
    EnableLight(GL_LIGHT1, L2);
    // EnableLight(GL_LIGHT2, L3);

    // draw the light objects
    DrawObjectTransform(LObj1);
    DrawObjectTransform(LObj2);
    DrawObjectTransform(LObj3);

    // enable lighting
    glEnable(GL_LIGHTING);

    // set the fog parameters:
    if (DepthCueOn != 0) {
        glFogi(GL_FOG_MODE, FOGMODE);
        glFogfv(GL_FOG_COLOR, FOGCOLOR);
        glFogf(GL_FOG_DENSITY, FOGDENSITY);
        glFogf(GL_FOG_START, FOGSTART);
        glFogf(GL_FOG_END, FOGEND);
        glEnable(GL_FOG);
    } else {
        glDisable(GL_FOG);
    }



    // since we are using glScalef( ), be sure normals get unitized:
    glEnable(GL_NORMALIZE);

    // possibly draw the origin axes
    if (OriginAxesOn != 0) {
        glPushMatrix();
        glScalef(OriginAxesScale, OriginAxesScale, OriginAxesScale);
        glColor3fv(&Colors[WhichColor][0]);
        glCallList(AxesList);
        glPopMatrix();
    }

    // Draw the main scene objects (helicopter and blades)
    // glCallList(HeliList);
    DrawHeli();
    DrawHelicopterBlades();

    // Apply front scene transformations to a copy of curr matrix
    glPushMatrix();
        // Apply front-specific translation transformation
        glTranslatef(0., 0., -2.);
        // glTranslatef(0., 1., -10.);
        // possibly draw the fixed front axes
        if (FrontAxesOn != 0) {
            glColor3fv(&Colors[WhichColor][0]);
            glCallList(AxesList);
        }
        // Apply front-specific rotation transformation
        glRotatef((GLfloat)FrontXrot, 1., 0., 0.);
        glRotatef((GLfloat)FrontYrot, 0., 1., 0.);
        glRotatef((GLfloat)FrontZrot, 0., 0., 1.);
        // Draw front scene objects (sphere)
        glCallList(SphereList);
        // possibly draw the rotated sphere front axes
        if (SphereAxesOn != 0) {
            glColor3fv(&Colors[WhichColor][0]);
            glCallList(AxesList);
        }
    // Pop transformations
    glPopMatrix();

    // Draw object1, object2, and object3
    DrawObjectTransform(Obj1, GL_FLAT);
    DrawObjectTransform(Obj2, GL_SMOOTH);
    DrawObjectTransform(Obj3, GL_FLAT);

    // Disable lighting
    glDisable(GL_LIGHTING);

    // draw the light objects
    DrawObjectTransform(LObj1);
    DrawObjectTransform(LObj2);
    DrawObjectTransform(LObj3);



    // draw some gratuitous text that just rotates on top of the scene:
    // glDisable(GL_DEPTH_TEST);
    // glColor3f(0., 1., 1.);
    // DoRasterString(0., 1., 0., (char *)"Text That Moves");

    // maybe draw some gratuitous text that is fixed on the screen:
    // the projection matrix is reset to define a scene whose
    // world coordinate system goes from 0-100 in each axis
    //
    // this is called "percent units", and is just a convenience
    //
    // the modelview matrix is reset to identity as we don't
    // want to transform these coordinates
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0., 100., 0., 100.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    char sliceStr[50];
    if (DrawHelpText) {
        glColor3f(1., 1., 1.);
        sprintf(sliceStr,
                "Controls = Space, t, T, F, R, Q, E, V, /, ?, ., [shift]wasd",
                // "Vertical=%i; Arc=%i; Controls=Space,R,Q,E,.,v,[shift]wasd",
                (int)SphereVertSlices, (int)SphereArcSlices);
        DoRasterString(5., 5., 0., sliceStr);
    }
    if (DrawSlicesText) {
        glColor3f(1., 1., 1.);
        sprintf(sliceStr, "Vertical = %i", (int)SphereVertSlices);
        DoRasterString(5., 10., 0., sliceStr);
        sprintf(sliceStr, "Arc = %i", (int)SphereArcSlices);
        DoRasterString(30., 10., 0., sliceStr);
    }

    // swap the double-buffered framebuffers:
    glutSwapBuffers();

    // be sure the graphics buffer has been sent:
    // note: be sure to use glFlush( ) here, not glFinish( ) !
    glFlush();
}

// Draws an object's list given all standard transformations
void DrawListTransform(GLuint ObjList, float ObjScale,
                       float ObjX,  float ObjY,  float ObjZ,
                       float ObjXr, float ObjYr, float ObjZr,
                       GLenum ShadeModel)
{
    // Use shade model provided
    glShadeModel(ShadeModel);

    // Apply object transformations to a copy of curr matrix
    glPushMatrix();
        // Scale, rotate, and translate object1
        glRotatef((GLfloat)ObjXr, 1., 0., 0.);
        glRotatef((GLfloat)ObjYr, 0., 1., 0.);
        glRotatef((GLfloat)ObjZr, 0., 0., 1.);
        glTranslatef(ObjX, ObjY, ObjZ);
        glScalef(ObjScale, ObjScale, ObjScale);
        // Draw object
        glCallList(ObjList);
    // Pop transformations
    glPopMatrix();
}
void DrawListTransform(GLuint ObjList, float ObjScale,
                       float ObjX,  float ObjY,  float ObjZ,
                       float ObjXr, float ObjYr, float ObjZr)
{
    DrawListTransform(ObjList, ObjScale, ObjX, ObjY, ObjZ, ObjXr, ObjYr, ObjZr, GL_SMOOTH);
}

void DrawObjectTransform(struct object Obj, GLenum ShadeModel)
{
    DrawListTransform(Obj.List, Obj.Scale, Obj.X, Obj.Y, Obj.Z, Obj.Xr, Obj.Yr, Obj.Zr, ShadeModel);
}

void DrawObjectTransform(struct object Obj)
{
    DrawListTransform(Obj.List, Obj.Scale, Obj.X, Obj.Y, Obj.Z, Obj.Xr, Obj.Yr, Obj.Zr);
}

void DrawHelicopterBlades()
{
    // Draw the top blade
    glPushMatrix();
        // Apply translation up to top of heli
        glTranslatef(0., 2.9, -1.5+8.5);
        // glTranslatef(0., 2.9, -1.5);
        // Apply scaling up to radius 5 (from 1)
        glScalef(5., 5., 5.);
        // Apply animation rotation
        glRotatef(TopBladeRot, 0., 1., 0.);
        // Apply rotation abt X
        glRotatef(90., 1., 0., 0.);
        // Draw the blade
        glCallList(BladeList);
    // Pop top blade transormations
    glPopMatrix();

    // Draw the tail blade
    glPushMatrix();
        // Apply translation up to top of heli
        glTranslatef(0.5, 2.5, 9.+8.5);
        // glTranslatef(0.5, 2.5, 9.);
        // Apply scaling up to radius 3 (from 1)
        glScalef(3., 3., 3.);
        // Apply animation rotation
        glRotatef(TailBladeRot, 1., 0., 0.);
        // Apply rotation abt Y
        glRotatef(90., 0., 1., 0.);
        // Draw the blade
        glCallList(BladeList);
    // Pop top blade transormations
    glPopMatrix();
}

void DoTexMenu(int id)
{
    TexSel = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDistortMenu(int id)
{
    TexDistortSel = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoVertSlicesMenu(int id)
{
    SphereVertSlices = VertSlices[id];

    UpdateSphereList();

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoArcSlicesMenu(int id)
{
    SphereArcSlices = ArcSlices[id];

    UpdateSphereList();

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoSphereRotateMenu(int id)
{
    SphereRotateOn = (id == 1) ? (true) : (false);

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoSphereSlicesMenu(int id)
{
    SphereSliceAnimateOn = (id == 1) ? (true) : (false);

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoBladesRotateMenu(int id)
{
    BladesRotateOn = (id == 1) ? (true) : (false);

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoAxesMenu(int id)
{
    // Toggle origin or front axes
    if (id < 2)
        OriginAxesOn = id;
    else if (id < 4)
        FrontAxesOn = id-2;
    else
        SphereAxesOn = id-4;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoColorMenu(int id)
{
    WhichColor = id - RED;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDebugMenu(int id)
{
    DebugOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthBufferMenu(int id)
{
    DepthBufferOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthFightingMenu(int id)
{
    DepthFightingOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoDepthMenu(int id)
{
    DepthCueOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// main menu callback:
void DoMainMenu(int id)
{
    switch (id)
    {
    case RESET:
        Reset();
        break;

    case QUIT:
        // gracefully close out the graphics:
        // gracefully close the graphics window:
        // gracefully exit the program:
        glutSetWindow(MainWindow);
        glFinish();
        glutDestroyWindow(MainWindow);
        exit(0);
        break;

    default:
        fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
    }

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoProjectMenu(int id)
{
    WhichProjection = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void DoShadowsMenu(int id)
{
    ShadowsOn = id;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// use glut to display a string of characters using a raster font:
void DoRasterString(float x, float y, float z, char *s)
{
    glRasterPos3f((GLfloat)x, (GLfloat)y, (GLfloat)z);

    char c; // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, c);
    }
}

// use glut to display a string of characters using a stroke font:
void DoStrokeString(float x, float y, float z, float ht, char *s)
{
    glPushMatrix();
    glTranslatef((GLfloat)x, (GLfloat)y, (GLfloat)z);
    float sf = ht / (119.05f + 33.33f);
    glScalef((GLfloat)sf, (GLfloat)sf, (GLfloat)sf);
    char c; // one character to print
    for (; (c = *s) != '\0'; s++)
    {
        glutStrokeCharacter(GLUT_STROKE_ROMAN, c);
    }
    glPopMatrix();
}

// return the number of seconds since the start of the program:
float ElapsedSeconds()
{
    // get # of milliseconds since the start of the program:
    int ms = glutGet(GLUT_ELAPSED_TIME);

    // convert it to seconds:
    return (float)ms / 1000.f;
}

// initialize the glui window:
void InitMenus()
{
    glutSetWindow(MainWindow);

    int numTexOptions = sizeof(TEX_DISPLAY_OPTIONS) / sizeof(char*);
    int texturemenu = glutCreateMenu(DoTexMenu);
    for (int i = 0; i < numTexOptions; i++)
    {
        glutAddMenuEntry(TEX_DISPLAY_OPTIONS[i], i);
    }
    
    int numDistortOptions = sizeof(TEX_DISTORT_OPTIONS) / sizeof(char*);
    int distortmenu = glutCreateMenu(DoDistortMenu);
    for (int i = 0; i < numDistortOptions; i++)
    {
        glutAddMenuEntry(TEX_DISTORT_OPTIONS[i], i);
    }

    int numVertOptions = sizeof(VertSlices) / sizeof(float);
    int vertmenu = glutCreateMenu(DoVertSlicesMenu);
    for (int i = 0; i < numVertOptions; i++)
    {
        glutAddMenuEntry(std::to_string(VertSlices[i]).c_str(), i);
    }

    int numArcOptions = sizeof(ArcSlices) / sizeof(float);
    int arcmenu = glutCreateMenu(DoArcSlicesMenu);
    for (int i = 0; i < numArcOptions; i++)
    {
        glutAddMenuEntry(std::to_string(ArcSlices[i]).c_str(), i);
    }
    
    int sphererotatemenu = glutCreateMenu(DoSphereRotateMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);
    
    int sphereslicesmenu = glutCreateMenu(DoSphereSlicesMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int bladesrotatemenu = glutCreateMenu(DoBladesRotateMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int numColors = sizeof(Colors) / (3 * sizeof(int));
    int colormenu = glutCreateMenu(DoColorMenu);
    for (int i = 0; i < numColors; i++) {
        glutAddMenuEntry(ColorNames[i], i);
    }

    int axesmenu = glutCreateMenu(DoAxesMenu);
    glutAddMenuEntry("Origin Off", 0);
    glutAddMenuEntry("Origin On", 1);
    glutAddMenuEntry("Front Off", 2);
    glutAddMenuEntry("Front On", 3);
    glutAddMenuEntry("Sphere Off", 4);
    glutAddMenuEntry("Sphere On", 5);

    int depthcuemenu = glutCreateMenu(DoDepthMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int depthbuffermenu = glutCreateMenu(DoDepthBufferMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int depthfightingmenu = glutCreateMenu(DoDepthFightingMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int debugmenu = glutCreateMenu(DoDebugMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int projmenu = glutCreateMenu(DoProjectMenu);
    glutAddMenuEntry("Orthographic", ORTHO);
    glutAddMenuEntry("Perspective", PERSP);

    int shadowsmenu = glutCreateMenu(DoShadowsMenu);
    glutAddMenuEntry("Off", 0);
    glutAddMenuEntry("On", 1);

    int mainmenu = glutCreateMenu(DoMainMenu);
    glutAddSubMenu("Texture Options", texturemenu);
    glutAddSubMenu("Distortion Options", distortmenu);
    glutAddSubMenu("Vert Slices", vertmenu);
    glutAddSubMenu("Arc Slices", arcmenu);
    glutAddSubMenu("Rotate Sphere", sphererotatemenu);
    glutAddSubMenu("Animate Sphere Slices", sphereslicesmenu);
    glutAddSubMenu("Rotate Blades", bladesrotatemenu);
    glutAddSubMenu("Toggle Axes", axesmenu);
    glutAddSubMenu("Axes Color", colormenu);
    glutAddSubMenu("Depth Cue", depthcuemenu);
    // glutAddSubMenu("Depth Buffer", depthbuffermenu);
    // glutAddSubMenu("Shadows", shadowsmenu);
    // glutAddSubMenu("Projection", projmenu);
    glutAddMenuEntry("Reset", RESET);
    // glutAddSubMenu("Debug", debugmenu);
    glutAddMenuEntry("Quit", QUIT);

    // attach the pop-up menu to the right mouse button:
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions
void InitGraphics()
{
    // request the display modes:
    // ask for red-green-blue-alpha color, double-buffering, and z-buffering:
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    // set the initial window configuration:
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

    // open the window and set its title:
    MainWindow = glutCreateWindow(WINDOWTITLE);
    glutSetWindowTitle(WINDOWTITLE);

    // set the framebuffer clear values:
    glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);

    // setup the callback functions:
    // DisplayFunc -- redraw the window
    // ReshapeFunc -- handle the user resizing the window
    // KeyboardFunc -- handle a keyboard input
    // MouseFunc -- handle the mouse button going down or up
    // MotionFunc -- handle the mouse moving with a button down
    // PassiveMotionFunc -- handle the mouse moving with a button up
    // VisibilityFunc -- handle a change in window visibility
    // EntryFunc	-- handle the cursor entering or leaving the window
    // SpecialFunc -- handle special keys on the keyboard
    // SpaceballMotionFunc -- handle spaceball translation
    // SpaceballRotateFunc -- handle spaceball rotation
    // SpaceballButtonFunc -- handle spaceball button hits
    // ButtonBoxFunc -- handle button box hits
    // DialsFunc -- handle dial rotations
    // TabletMotionFunc -- handle digitizing tablet motion
    // TabletButtonFunc -- handle digitizing tablet button hits
    // MenuStateFunc -- declare when a pop-up menu is in use
    // TimerFunc -- trigger something to happen a certain time from now
    // IdleFunc -- what to do when nothing else is going on

    glutSetWindow(MainWindow);
    glutDisplayFunc(Display);
    glutReshapeFunc(Resize);
    glutKeyboardFunc(Keyboard);
    glutMouseFunc(MouseButton);
    glutMotionFunc(MouseMotion);
    glutPassiveMotionFunc(MouseMotion);
    //glutPassiveMotionFunc( NULL );
    glutVisibilityFunc(Visibility);
    glutEntryFunc(NULL);
    glutSpecialFunc(NULL);
    glutSpaceballMotionFunc(NULL);
    glutSpaceballRotateFunc(NULL);
    glutSpaceballButtonFunc(NULL);
    glutButtonBoxFunc(NULL);
    glutDialsFunc(NULL);
    glutTabletMotionFunc(NULL);
    glutTabletButtonFunc(NULL);
    glutMenuStateFunc(NULL);
    glutTimerFunc(-1, NULL, 0);
    glutIdleFunc(Animate);

    // init glew (a window must be open to do this):
#ifdef _WIN32
    GLenum err = glewInit();
    if (err != GLEW_OK)
    {
        fprintf(stderr, "glewInit Error\n");
    }
    else
        fprintf(stderr, "GLEW initialized OK\n");
    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
#endif

    // Get textures
    Texture = BmpToTexture(TEXTURE_PATH, &TEXTURE_WIDTH, &TEXTURE_HEIGHT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Create texture objects
    glGenTextures(1, &Tex0);

    // Bind for first time
    glBindTexture(GL_TEXTURE_2D, Tex0);

    // Setup Texture
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // or GL_CLAMP
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // or GL_CLAMP
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // or GL_LINEAR
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // or GL_REPLACE
    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // or GL_MODULATE

    // Rotate texture coord space
    glMatrixMode(GL_TEXTURE);
    glScalef(0.1, 0.15, 1.);
    glTranslatef(4.5, 3.1, 0.);
    glRotatef(15., 0., 0., 1.);
    glMatrixMode(GL_MODELVIEW);

    // Backup
    // // glRotatef(10., 0., 0., 1.);
    // // glTranslatef(0.5, 0., 0.);
    // glScalef(1., 0.15, 1.);
    // // glScalef(1., 0.3, 1.);
    // // glTranslatef(0.5, -1., 0.);
    // glScalef(0.1, 1., 1.);
    // // glScalef(0.08, 1., 1.);
    // // glScalef(0.125, 1., 1.);
    // // glTranslatef(4, -0.5, 0.);
    // glTranslatef(4., 0.75, 0.);
    // glTranslatef(0.5, 2.35, 0.);
    // glRotatef(15., 0., 0., 1.);
    // glMatrixMode(GL_MODELVIEW);

    // Apply Texture
    glTexImage2D(GL_TEXTURE_2D, 0, 4,
                 TEXTURE_WIDTH, TEXTURE_HEIGHT,
                 0, GL_RGB, GL_UNSIGNED_BYTE, Texture);
}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )
void InitLists()
{
    // Create the helicopter list
    CreateHeliList();

    // Create the blade list
    CreateBladeList();

    // Create the axes lists
    AxesList = glGenLists(1);
    glNewList(AxesList, GL_COMPILE);
    glLineWidth(AXES_WIDTH);
    Axes(1.5);
    glLineWidth(1.);
    glEndList();

    // Create the sphere list
    UpdateSphereList();

    // Create the object1 list
    glutSetWindow(MainWindow);
    Obj1.List = glGenLists(1);
    glNewList(Obj1.List, GL_COMPILE);
    DrawOsuSphereLit(1.f, 100, 100, Obj1);
    glEndList();

    // Create the object2 list
    glutSetWindow(MainWindow);
    Obj2.List = glGenLists(1);
    glNewList(Obj2.List, GL_COMPILE);
    DrawOsuSphereLit(1.f, 20, 20, Obj2);
    glEndList();

    // Create the object3 list
    glutSetWindow(MainWindow);
    Obj3.List = glGenLists(1);
    glNewList(Obj3.List, GL_COMPILE);
    DrawOsuSphereLit(1.f, 8, 8, Obj3);
    glEndList();

    // Create the lightobject1 list
    glutSetWindow(MainWindow);
    LObj1.List = glGenLists(1);
    glNewList(LObj1.List, GL_COMPILE);
    DrawOsuSphereUnlit(0.1, 10, 10, LObj1);
    glEndList();

    // Create the lightobject2 list
    glutSetWindow(MainWindow);
    LObj2.List = glGenLists(1);
    glNewList(LObj2.List, GL_COMPILE);
    DrawOsuSphereUnlit(0.1, 10, 10, LObj2);
    glEndList();

    // Create the lightobject3 list
    glutSetWindow(MainWindow);
    LObj3.List = glGenLists(1);
    glNewList(LObj3.List, GL_COMPILE);
    DrawOsuSphereUnlit(0.1, 10, 10, LObj3);
    glEndList();

}

void CreateHeliList() {
    // Set to the main window
    glutSetWindow(MainWindow);
    
    // Start heli list
    HeliList = glGenLists(1);
    glNewList(HeliList, GL_COMPILE);

    // Draw heli
    DrawHeli();

    // End list
    glEndList();
}

void DrawHeli() {
    // Create locals
    int i;
    struct edge *ep;
    struct hPoint *p0, *p1, *p2;
    struct tri *tp;
    float p01[3], p02[3], n[3];

    // Push current matrix
    glPushMatrix();
    glTranslatef(0., -1., 9.);
    // glTranslatef(0., -1., 0.5);
    glRotatef(97., 0., 1., 0.);
    glRotatef(-15., 0., 0., 1.);

    // Set min and max on coords for texture mapping
    float min_z = 0, max_z = 0;
    float min_y = -1, max_y = 1;
    float min_x = -1, max_x = 1;

    // Apply Tex0 (or not) with different settings as requested
    switch (TexSel) {
        case 1: // Replace
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, Tex0);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // or GL_MODULATE
            break;
        case 2: // Modulate
            glEnable(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, Tex0);
            glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // or GL_REPLACE
            break;
        // Do nothing in case 0 of no texture
    }

    // Build geometry
    glBegin(GL_TRIANGLES);
    for (i = 0, tp = Helitris; i < Helintris; i++, tp++)
    {
        // get points of tri
        p0 = &Helipoints[tp->p0];
        p1 = &Helipoints[tp->p1];
        p2 = &Helipoints[tp->p2];

        // fake "lighting" from above:
        p01[0] = p1->x - p0->x;
        p01[1] = p1->y - p0->y;
        p01[2] = p1->z - p0->z;
        p02[0] = p2->x - p0->x;
        p02[1] = p2->y - p0->y;
        p02[2] = p2->z - p0->z;
        Cross(p01, p02, n);
        Unit(n, n);
        n[1] = fabs(n[1]);
        n[1] += .25;
        if (n[1] > 1.)
            n[1] = 1.;
        glColor3f(0.75, 0.75 + (n[1] / 4.), 1.);
        // glColor3f(0., n[1], 0.5);

        // find texture coords
        float p0_s = 1 - (p0->x - min_x) / (max_x - min_x);
        float p1_s = 1 - (p1->x - min_x) / (max_x - min_x);
        float p2_s = 1 - (p2->x - min_x) / (max_x - min_x);
        float p0_t = (p0->y - min_y) / (max_y - min_y);
        float p1_t = (p1->y - min_y) / (max_y - min_y);
        float p2_t = (p2->y - min_y) / (max_y - min_y);

        // calculate distortion params
        float s_off = 0.;
        float t_off = 0.;
        float sine = 1.;
        float cosine = 1.;
        switch (TexDistortSel) {
            case 1: // Small
                s_off = 0.;
                sine = 2 - sin(DistortTexTheta);
                t_off = -2.5;
                cosine = 3 - cos(DistortTexTheta);
                break;
            case 2: // Whoosh
                s_off = 2.;
                sine = -1.5*sin(DistortTexTheta);
                t_off = 0.5;
                cosine = -cos(DistortTexTheta);
                break;
            // Do nothing in case 0 of no animation
        }

        // write texture coords and vertices
        // glTexCoord2f(p0_s, p0_t);
        glTexCoord2f(s_off+sine*p0_s, t_off+cosine*p0_t);
        glVertex3f(p0->x, p0->y, p0->z);
        glTexCoord2f(s_off+sine*p1_s, t_off+cosine*p1_t);
        // glTexCoord2f(p1_s, p1_t);
        glVertex3f(p1->x, p1->y, p1->z);
        glTexCoord2f(s_off+sine*p2_s, t_off+cosine*p2_t);
        // glTexCoord2f(p2_s, p2_t);
        glVertex3f(p2->x, p2->y, p2->z);
    }
    glEnd();

    glDisable(GL_TEXTURE_2D);

    // glBegin( GL_LINES );
    //     for( i=0, ep = Heliedges; i < Helinedges; i++, ep++ )
    //     {
    //         p0 = &Helipoints[ ep->p0 ];
    //         p1 = &Helipoints[ ep->p1 ];
    //         glVertex3f( p0->x, p0->y, p0->z );
    //         glVertex3f( p1->x, p1->y, p1->z );
    //     }
    // glEnd();

    glPopMatrix();
}

void CreateBladeList() {
    // Set to the main window
    glutSetWindow(MainWindow);

    // Start blade list
    BladeList = glGenLists(1);
    glNewList(BladeList, GL_COMPILE);

    // draw the helicopter blade with radius BLADE_RADIUS and
    //	width BLADE_WIDTH centered at (0.,0.,0.) in the XY plane
    glColor3f(0.7, 0.9, 0.7);
    // glColor3f(0.8, 1., 0.8);
    glBegin(GL_TRIANGLES);
        glVertex2f(  BLADE_RADIUS,  BLADE_WIDTH/2. );
        glVertex2f(  0., 0. );
        glVertex2f(  BLADE_RADIUS, -BLADE_WIDTH/2. );

        glVertex2f( -BLADE_RADIUS, -BLADE_WIDTH/2. );
        glVertex2f(  0., 0. );
        glVertex2f( -BLADE_RADIUS,  BLADE_WIDTH/2. );
    glEnd();

    glEndList();
}

void UpdateSphereList() {
    // Set to the main window
    glutSetWindow(MainWindow);

    // Create coordinate struct
    struct vec3 {
        float x;
        float y;
        float z;
    };

    // Create locals
    std::vector<vec3> prev_layer;
    bool first = true;
    vec3 curr;

    // Create the sphere list
    SphereList = glGenLists(1);
    glNewList(SphereList, GL_COMPILE);

    if (WireframeMode) {
        // Enable wireframe
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
    }

    // Push matrix to apply rotation 
    glPushMatrix();

    // Start compiling geometry
    glBegin(GL_TRIANGLE_STRIP);

    // // Use blue color for now
    // glColor3f(0., 0., 1.);

    // For each pair of vertical vertices (slice)
    for (int i = 0; i <= int(SphereVertSlices); i++) {
        // Make new curr
        curr = {0, 0, 0};

        // Get slice radius and y for this slice
        float v_theta = (float(i) / int(SphereVertSlices)) * M_PI;
        float v_rad = SPHERE_RADIUS * sin(v_theta);
        curr.y = SPHERE_RADIUS * cos(v_theta);

        // For each arc slices (and then the first again)
        for (int j = 0; j <= int(SphereArcSlices); j++) {
            // Get x and y for this arc
            float slice_theta = -1 * (float(j) / int(SphereArcSlices)) * 2*M_PI + M_PI/2;
            curr.x = v_rad * cos(slice_theta);
            curr.z = v_rad * sin(slice_theta);

            // If not first slice, add this vert and prev_slice vert to strip
            if (!first) {
                // Change blue hue by arc progression, red by vertical
                glColor3f(float(i) / int(SphereVertSlices),
                          0.f,
                          float(j) / int(SphereArcSlices));

                // // Red and green gradiants
                // glColor3f(float(i) / int(SphereVertSlices),
                //           float(j) / int(SphereArcSlices),
                //           0.f);

                // Always do prev layer as first vertex in triangle chain
                glVertex3f(prev_layer[j].x,
                           prev_layer[j].y,
                           prev_layer[j].z);
                
                // Then do the curr layer
                glVertex3f(curr.x, curr.y, curr.z);
                
                // Then replace this element in prev_layer with the curr value
                prev_layer[j] = curr;
            }
            // Otherwise this is the first slice, push curr into prev_layer
            else {
                prev_layer.push_back(curr);
            }
        }

        // After first slice disable the first flag
        if (first) {
                // Set first to false
                first = false;
        }
    }

    // Pop matrix after creating geometry
    glPopMatrix();

    // End sphere geometry
    glEnd();

    if (WireframeMode) {
        // Disable wireframe
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }

    // End sphere list
    glEndList();
}


// the keyboard callback:
void Keyboard(unsigned char c, int x, int y)
{
    if (DebugOn != 0)
        fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

    switch (c)
    {
    // case 'o':
    // case 'O':
    //     WhichProjection = ORTHO;
    //     break;

    case 'p':
    case 'P':
        WhichProjection = PERSP;
        break;

    case 'v':
    case 'V':
        if (CurrView == OUTSIDE_VIEW)
            CurrView = INSIDE_VIEW;
        else if (CurrView == INSIDE_VIEW)
            CurrView = OUTSIDE_VIEW;
        break;

    case 'a':
        SphereArcSlices -= 1;
        if (SphereArcSlices < SPHERE_MIN_ARC) {SphereArcSlices = SPHERE_MIN_ARC;}
        if (SphereArcSlices > SPHERE_MAX_ARC) {SphereArcSlices = SPHERE_MAX_ARC;}
        UpdateSphereList();
        break;
    case 'A':
        SphereArcSlices -= 10;
        if (SphereArcSlices < SPHERE_MIN_ARC) {SphereArcSlices = SPHERE_MIN_ARC;}
        if (SphereArcSlices > SPHERE_MAX_ARC) {SphereArcSlices = SPHERE_MAX_ARC;}
        UpdateSphereList();
        break;
    case 'd':
        SphereArcSlices += 1;
        if (SphereArcSlices < SPHERE_MIN_ARC) {SphereArcSlices = SPHERE_MIN_ARC;}
        if (SphereArcSlices > SPHERE_MAX_ARC) {SphereArcSlices = SPHERE_MAX_ARC;}
        UpdateSphereList();
        break;
    case 'D':
        SphereArcSlices += 10;
        if (SphereArcSlices < SPHERE_MIN_ARC) {SphereArcSlices = SPHERE_MIN_ARC;}
        if (SphereArcSlices > SPHERE_MAX_ARC) {SphereArcSlices = SPHERE_MAX_ARC;}
        UpdateSphereList();
        break;
    case 's':
        SphereVertSlices -= 1;
        if (SphereVertSlices < SPHERE_MIN_VERT) {SphereVertSlices = SPHERE_MIN_VERT;}
        if (SphereVertSlices > SPHERE_MAX_VERT) {SphereVertSlices = SPHERE_MAX_VERT;}
        UpdateSphereList();
        break;
    case 'S':
        SphereVertSlices -= 10;
        if (SphereVertSlices < SPHERE_MIN_VERT) {SphereVertSlices = SPHERE_MIN_VERT;}
        if (SphereVertSlices > SPHERE_MAX_VERT) {SphereVertSlices = SPHERE_MAX_VERT;}
        UpdateSphereList();
        break;
    case 'w':
        SphereVertSlices += 1;
        if (SphereVertSlices < SPHERE_MIN_VERT) {SphereVertSlices = SPHERE_MIN_VERT;}
        if (SphereVertSlices > SPHERE_MAX_VERT) {SphereVertSlices = SPHERE_MAX_VERT;}
        UpdateSphereList();
        break;
    case 'W':
        SphereVertSlices += 10;
        if (SphereVertSlices < SPHERE_MIN_VERT) {SphereVertSlices = SPHERE_MIN_VERT;}
        if (SphereVertSlices > SPHERE_MAX_VERT) {SphereVertSlices = SPHERE_MAX_VERT;}
        UpdateSphereList();
        break;

    case 'r':
    case 'R':
        Reset();
        break;

    case ' ':
        SphereRotateOn = !SphereRotateOn;
        SphereSliceAnimateOn = !SphereSliceAnimateOn;
        break;

    case 'f':
        BladesRotateOn = !BladesRotateOn;
        break;

    case 'e':
    case 'E':
        WireframeMode = !WireframeMode;
        UpdateSphereList();
        break;

    case '.':
        OriginAxesOn = !OriginAxesOn;
        FrontAxesOn = !FrontAxesOn;
        SphereAxesOn = !SphereAxesOn;
        break;

    case '?':
        DrawHelpText = !DrawHelpText;
        break;

    case '/':
        DrawSlicesText = !DrawSlicesText;
        break;

    case 't':
        // Cycle through texture options
        TexSel += 1;
        if (TexSel >= sizeof(TEX_DISPLAY_OPTIONS) / sizeof(char*))
            TexSel = 0;
        break;

    case 'T':
        // Cycle through texture distortion options
        TexDistortSel += 1;
        if (TexDistortSel >= sizeof(TEX_DISTORT_OPTIONS) / sizeof(char*))
            TexDistortSel = 0;
        break;

    case 'q':
    case 'Q':
    case ESCAPE:
        DoMainMenu(QUIT); // will not return here
        break;            // happy compiler

    default:
        fprintf(stderr, "Don't know what to do with keyboard hit: '%c' (0x%0x)\n", c, c);
    }

    // force a call to Display( ):

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// called when the mouse button transitions down or up:

void MouseButton(int button, int state, int x, int y)
{
    int b = 0; // LEFT, MIDDLE, or RIGHT

    if (DebugOn != 0)
        fprintf(stderr, "MouseButton: %d, %d, %d, %d\n", button, state, x, y);

    // get the proper button bit mask:

    switch (button)
    {
    case GLUT_LEFT_BUTTON:
        b = LEFT;
        break;

    case GLUT_MIDDLE_BUTTON:
        b = MIDDLE;
        break;

    case GLUT_RIGHT_BUTTON:
        b = RIGHT;
        break;

    case SCROLL_WHEEL_UP:
        // only change if in outside view
        if (CurrView == OUTSIDE_VIEW) {
            Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
            // keep object from turning inside-out or disappearing:
            if (Scale < MINSCALE)
                Scale = MINSCALE;
        }
        break;

    case SCROLL_WHEEL_DOWN:
        if (CurrView == OUTSIDE_VIEW) {
            Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
            // keep object from turning inside-out or disappearing:
            if (Scale < MINSCALE)
                Scale = MINSCALE;
        }
        break;

    default:
        b = 0;
        fprintf(stderr, "Unknown mouse button: %d\n", button);
    }

    // button down sets the bit, up clears the bit:

    if (state == GLUT_DOWN)
    {
        Xmouse = x;
        Ymouse = y;
        ActiveButton |= b; // set the proper bit
    }
    else
    {
        ActiveButton &= ~b; // clear the proper bit
    }

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// called when the mouse moves while a button is down:

void MouseMotion(int x, int y)
{
    // if( true )
    // 	fprintf( stderr, "MouseMotion: %d, %d\n", x, y );

    int dx = x - Xmouse; // change in mouse coords
    int dy = Ymouse - y;
    int dz = 0;

    if ((ActiveButton & LEFT) != 0)
    {
        if (CurrView == OUTSIDE_VIEW) {
            Xrot += (ANGFACT * dy);
            Yrot += (ANGFACT * dx);
            Zrot += (ANGFACT * dz);
        }
    }

    if ((ActiveButton & MIDDLE) != 0)
    {
        if (CurrView == OUTSIDE_VIEW) {
            Scale += SCLFACT * (float)(dx - dy);

            // keep object from turning inside-out or disappearing:
            if (Scale < MINSCALE)
                Scale = MINSCALE;
        }
    }

    Xmouse = x; // new current position
    Ymouse = y;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// class Array {
//     float a, b, c;
//     Array(float a, float b, float c) {
//         this.a = a, this.b = b, this.c = c;
//     }
    

// }

// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene

void Reset(bool redisplay)
{
    ActiveButton = 0;
    OriginAxesOn = 0;
    FrontAxesOn = 0;
    SphereAxesOn = 0;
    OriginAxesScale = 2.5;
    DebugOn = 0;
    DepthBufferOn = 1;
    DepthFightingOn = 0;
    DepthCueOn = 0;
    Scale = 1.5;
    ShadowsOn = 0;
    WhichColor = WHITE;
    WhichProjection = PERSP; 
    Xrot = -10.;
    Yrot = -20.;
    Zrot = 0.;
    TopBladeRot, TailBladeRot = 0., 0.;
    BladesRotateOn = true;
    SphereVertSlices = SPHERE_DEF_VERT;
    SphereArcSlices = SPHERE_DEF_ARC;
    SphereRotateOn = true;
    SphereSliceAnimateOn = true;
    SphereArcSliceAnimateDir = 1; 
    SphereVertSliceAnimateDir = 1;
    WireframeMode = false;
    DrawHelpText = true;
    DrawSlicesText = false;
    CurrView = OUTSIDE_VIEW;
    DistortTexOn = true;
    TexDistortSel = 0;
    TexSel = 2;
    DistortTexTheta = 0.;

    // reset objects  
    Obj1.X = 0., Obj1.Y = 2., Obj1.Z = 0.;
    Obj1.Xr = 0., Obj1.Yr = 0., Obj1.Zr = 0.;
    Obj1.R = 1., Obj1.G = 1., Obj1.B = 0.;
    Obj1.Scale = 1.;

    Obj2.X = -2., Obj2.Y = 1., Obj2.Z = 0.;
    Obj2.Xr = 0., Obj2.Yr = 0., Obj2.Zr = 0.;
    Obj2.R = 0., Obj2.G = 1., Obj2.B = 0.;
    Obj2.Scale = 1.;

    Obj3.X = 2., Obj3.Y = 1., Obj3.Z = 0.;
    Obj3.Xr = 0., Obj3.Yr = 0., Obj3.Zr = 0.;
    Obj3.R = 0., Obj3.G = 0., Obj3.B = 1.;
    Obj3.Scale = 1.;

    // reset lights
    L1.X = 0., L1.Y = 4., L1.Z = 0.;
    L1.R = 1., L1.G = 0., L1.B = 0.;
    L1.ambient = 0., L1.diffuse = 1, L1.specular = 1;

    L2.X = 0., L2.Y = 0., L2.Z = 0.;
    L2.R = 0., L2.G = 0., L2.B = 1.;
    L2.ambient = 0., L2.diffuse = 1., L2.specular = 1.;

    L3.X = 0., L3.Y = 5., L3.Z = 0.;
    L3.R = 1., L3.G = 1., L3.B = 1.;
    L3.ambient = 0., L3.diffuse = 1., L3.specular = 1.;

    // reset light objects
    LObj1.X = L1.X, LObj1.Y = L1.Y, LObj1.Z = L1.Z;
    LObj1.Xr = 0., LObj1.Yr = 0., LObj1.Zr = 0.;
    LObj1.R = L1.R, LObj1.G = L1.G, LObj1.B = L1.B;
    LObj1.Scale = 1.0;

    LObj2.X = L2.X, LObj2.Y = L2.Y, LObj2.Z = L2.Z;
    LObj2.Xr = 0., LObj2.Yr = 0., LObj2.Zr = 0.;
    LObj2.R = L2.R, LObj2.G = L2.G, LObj2.B = L2.B;
    LObj2.Scale = 1.0;

    LObj3.X = L3.X, LObj3.Y = L3.Y, LObj3.Z = L3.Z;
    LObj3.Xr = 0., LObj3.Yr = 0., LObj3.Zr = 0.;
    LObj3.R = L3.R, LObj3.G = L3.G, LObj3.B = L3.B;
    LObj3.Scale = 1.0;
    
    UpdateSphereList();

    // redisplay if requested
    if (redisplay) {
        glutSetWindow(MainWindow);
        glutPostRedisplay();
    }
}

void Reset()
{
    Reset(false);
}

// called when user resizes the window:
void Resize(int width, int height)
{
    if (DebugOn != 0)
        fprintf(stderr, "ReSize: %d, %d\n", width, height);

    // don't really need to do anything since window size is
    // checked each time in Display( ):

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// handle a change to the window's visibility:
void Visibility(int state)
{
    if (DebugOn != 0)
        fprintf(stderr, "Visibility: %d\n", state);

    if (state == GLUT_VISIBLE)
    {
        glutSetWindow(MainWindow);
        glutPostRedisplay();
    }
    else
    {
        // could optimize by keeping track of the fact
        // that the window is not visible and avoid
        // animating or redrawing it ...
    }
}

///////////////////////////////////////   HANDY UTILITIES:  //////////////////////////

// the stroke characters 'X' 'Y' 'Z' :

static float xx[] = {
    0.f, 1.f, 0.f, 1.f};

static float xy[] = {
    -.5f, .5f, .5f, -.5f};

static int xorder[] = {
    1, 2, -3, 4};

static float yx[] = {
    0.f, 0.f, -.5f, .5f};

static float yy[] = {
    0.f, .6f, 1.f, 1.f};

static int yorder[] = {
    1, 2, 3, -2, 4};

static float zx[] = {
    1.f, 0.f, 1.f, 0.f, .25f, .75f};

static float zy[] = {
    .5f, .5f, -.5f, -.5f, 0.f, 0.f};

static int zorder[] = {
    1, 2, 3, 4, -5, 6};

// fraction of the length to use as height of the characters:
const float LENFRAC = 0.10f;

// fraction of length to use as start location of the characters:
const float BASEFRAC = 1.10f;

//	Draw a set of 3D axes:
//	(length is the axis length in world coordinates)

void Axes(float length)
{
    glBegin(GL_LINE_STRIP);
    glVertex3f(length, 0., 0.);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., length, 0.);
    glEnd();
    glBegin(GL_LINE_STRIP);
    glVertex3f(0., 0., 0.);
    glVertex3f(0., 0., length);
    glEnd();

    float fact = LENFRAC * length;
    float base = BASEFRAC * length;

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 4; i++)
    {
        int j = xorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(base + fact * xx[j], fact * xy[j], 0.0);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 5; i++)
    {
        int j = yorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(fact * yx[j], base + fact * yy[j], 0.0);
    }
    glEnd();

    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < 6; i++)
    {
        int j = zorder[i];
        if (j < 0)
        {

            glEnd();
            glBegin(GL_LINE_STRIP);
            j = -j;
        }
        j--;
        glVertex3f(0.0, fact * zy[j], base + fact * zx[j]);
    }
    glEnd();
}

void UseObjectMaterial(struct object Obj)
{
    // back-facing attributes
    float backRGB[4] = {Obj.BR, Obj.BG, Obj.BB, 1.};
    glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
    glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
    glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(0.3, backRGB));
    glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(1., backRGB));
    glMaterialf(GL_BACK, GL_SHININESS, 1.f);

    // front-facing attributes
    float frontRGB[4] = {Obj.R, Obj.G, Obj.B, 1.};
    glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
    glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(0.8, White));
    glMaterialfv(GL_FRONT, GL_AMBIENT, frontRGB);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, frontRGB);
    glMaterialf(GL_FRONT, GL_SHININESS, 3.f);
}

void EnableLight(int lightNum, struct light l)
{
    glEnable(lightNum);

    float color[4] = {l.R, l.G, l.B, 1.};
    glLightfv(lightNum, GL_AMBIENT, MulArray3(l.ambient, color));
    glLightfv(lightNum, GL_DIFFUSE, MulArray3(l.diffuse, color));
    glLightfv(lightNum, GL_SPECULAR, MulArray3(l.specular, color));

    glLightf(lightNum, GL_CONSTANT_ATTENUATION, 1.);
    glLightf(lightNum, GL_LINEAR_ATTENUATION, 0.);
    glLightf(lightNum, GL_QUADRATIC_ATTENUATION, 0.);

    float pos[4] = {l.X, l.Y, l.Z, 1.};
    glLightfv(lightNum, GL_POSITION, pos);
}

void DrawOsuSphereUnlit(float radius, int slices, int stacks, struct object Obj)
{
    // establish sphere color
    glColor3f(Obj.R, Obj.G, Obj.B);

    // draw sphere
    OsuSphere(radius, slices, stacks);
}

void DrawOsuSphereLit(float radius, int slices, int stacks, struct object Obj)
{
	// establish material characteristics
    UseObjectMaterial(Obj);
    
    // draw sphere
    OsuSphere(radius, slices, stacks);
}


// read a BMP file into a Texture:

#define VERBOSE false
#define BMP_MAGIC_NUMBER 0x4d42
#ifndef BI_RGB
#define BI_RGB 0
#define BI_RLE8 1
#define BI_RLE4 2
#endif

// bmp file header:
struct bmfh
{
    short bfType; // BMP_MAGIC_NUMBER = "BM"
    int bfSize;   // size of this file in bytes
    short bfReserved1;
    short bfReserved2;
    int bfOffBytes; // # bytes to get to the start of the per-pixel data
} FileHeader;

// bmp info header:
struct bmih
{
    int biSize;        // info header size, should be 40
    int biWidth;       // image width
    int biHeight;      // image height
    short biPlanes;    // #color planes, should be 1
    short biBitCount;  // #bits/pixel, should be 1, 4, 8, 16, 24, 32
    int biCompression; // BI_RGB, BI_RLE4, BI_RLE8
    int biSizeImage;
    int biXPixelsPerMeter;
    int biYPixelsPerMeter;
    int biClrUsed; // # colors in the palette
    int biClrImportant;
} InfoHeader;

// read a BMP file into a Texture:
unsigned char *
BmpToTexture(char *filename, int *width, int *height)
{
    FILE *fp;
#ifdef _WIN32
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
        return NULL;
    }
#else
    fp = fopen(filename, "rb");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open Bmp file '%s'\n", filename);
        return NULL;
    }
#endif

    FileHeader.bfType = ReadShort(fp);

    // if bfType is not BMP_MAGIC_NUMBER, the file is not a bmp:

    if (VERBOSE)
        fprintf(stderr, "FileHeader.bfType = 0x%0x = \"%c%c\"\n",
                FileHeader.bfType, FileHeader.bfType & 0xff, (FileHeader.bfType >> 8) & 0xff);
    if (FileHeader.bfType != BMP_MAGIC_NUMBER)
    {
        fprintf(stderr, "Wrong type of file: 0x%0x\n", FileHeader.bfType);
        fclose(fp);
        return NULL;
    }

    FileHeader.bfSize = ReadInt(fp);
    if (VERBOSE)
        fprintf(stderr, "FileHeader.bfSize = %d\n", FileHeader.bfSize);

    FileHeader.bfReserved1 = ReadShort(fp);
    FileHeader.bfReserved2 = ReadShort(fp);

    FileHeader.bfOffBytes = ReadInt(fp);
    if (VERBOSE)
        fprintf(stderr, "FileHeader.bfOffBytes = %d\n", FileHeader.bfOffBytes);

    InfoHeader.biSize = ReadInt(fp);
    if (VERBOSE)
        fprintf(stderr, "InfoHeader.biSize = %d\n", InfoHeader.biSize);
    InfoHeader.biWidth = ReadInt(fp);
    if (VERBOSE)
        fprintf(stderr, "InfoHeader.biWidth = %d\n", InfoHeader.biWidth);
    InfoHeader.biHeight = ReadInt(fp);
    if (VERBOSE)
        fprintf(stderr, "InfoHeader.biHeight = %d\n", InfoHeader.biHeight);

    const int nums = InfoHeader.biWidth;
    const int numt = InfoHeader.biHeight;

    InfoHeader.biPlanes = ReadShort(fp);
    if (VERBOSE)
        fprintf(stderr, "InfoHeader.biPlanes = %d\n", InfoHeader.biPlanes);

    InfoHeader.biBitCount = ReadShort(fp);
    if (VERBOSE)
        fprintf(stderr, "InfoHeader.biBitCount = %d\n", InfoHeader.biBitCount);

    InfoHeader.biCompression = ReadInt(fp);
    if (VERBOSE)
        fprintf(stderr, "InfoHeader.biCompression = %d\n", InfoHeader.biCompression);

    InfoHeader.biSizeImage = ReadInt(fp);
    if (VERBOSE)
        fprintf(stderr, "InfoHeader.biSizeImage = %d\n", InfoHeader.biSizeImage);

    InfoHeader.biXPixelsPerMeter = ReadInt(fp);
    InfoHeader.biYPixelsPerMeter = ReadInt(fp);

    InfoHeader.biClrUsed = ReadInt(fp);
    if (VERBOSE)
        fprintf(stderr, "InfoHeader.biClrUsed = %d\n", InfoHeader.biClrUsed);

    InfoHeader.biClrImportant = ReadInt(fp);

    // fprintf( stderr, "Image size found: %d x %d\n", ImageWidth, ImageHeight );

    // pixels will be stored bottom-to-top, left-to-right:
    unsigned char *texture = new unsigned char[3 * nums * numt];
    if (texture == NULL)
    {
        fprintf(stderr, "Cannot allocate the texture array!\n");
        return NULL;
    }

    // extra padding bytes:
    int requiredRowSizeInBytes = 4 * ((InfoHeader.biBitCount * InfoHeader.biWidth + 31) / 32);
    if (VERBOSE)
        fprintf(stderr, "requiredRowSizeInBytes = %d\n", requiredRowSizeInBytes);

    int myRowSizeInBytes = (InfoHeader.biBitCount * InfoHeader.biWidth + 7) / 8;
    if (VERBOSE)
        fprintf(stderr, "myRowSizeInBytes = %d\n", myRowSizeInBytes);

    int oldNumExtra = 4 * (((3 * InfoHeader.biWidth) + 3) / 4) - 3 * InfoHeader.biWidth;
    if (VERBOSE)
        fprintf(stderr, "Old NumExtra padding = %d\n", oldNumExtra);

    int numExtra = requiredRowSizeInBytes - myRowSizeInBytes;
    if (VERBOSE)
        fprintf(stderr, "New NumExtra padding = %d\n", numExtra);

    // this function does not support compression:
    if (InfoHeader.biCompression != 0)
    {
        fprintf(stderr, "Wrong type of image compression: %d\n", InfoHeader.biCompression);
        fclose(fp);
        return NULL;
    }

    // we can handle 24 bits of direct color:
    if (InfoHeader.biBitCount == 24)
    {
        rewind(fp);
        fseek(fp, FileHeader.bfOffBytes, SEEK_SET);
        int t;
        unsigned char *tp;
        for (t = 0, tp = texture; t < numt; t++)
        {
            for (int s = 0; s < nums; s++, tp += 3)
            {
                *(tp + 2) = fgetc(fp); // b
                *(tp + 1) = fgetc(fp); // g
                *(tp + 0) = fgetc(fp); // r
            }

            for (int e = 0; e < numExtra; e++)
            {
                fgetc(fp);
            }
        }
    }

    // we can also handle 8 bits of indirect color:
    if (InfoHeader.biBitCount == 8 && InfoHeader.biClrUsed == 256)
    {
        struct rgba32
        {
            unsigned char r, g, b, a;
        };
        struct rgba32 *colorTable = new struct rgba32[InfoHeader.biClrUsed];

        rewind(fp);
        fseek(fp, sizeof(struct bmfh) + InfoHeader.biSize - 2, SEEK_SET);
        for (int c = 0; c < InfoHeader.biClrUsed; c++)
        {
            colorTable[c].r = fgetc(fp);
            colorTable[c].g = fgetc(fp);
            colorTable[c].b = fgetc(fp);
            colorTable[c].a = fgetc(fp);
            if (VERBOSE)
                fprintf(stderr, "%4d:\t0x%02x\t0x%02x\t0x%02x\t0x%02x\n",
                        c, colorTable[c].r, colorTable[c].g, colorTable[c].b, colorTable[c].a);
        }

        rewind(fp);
        fseek(fp, FileHeader.bfOffBytes, SEEK_SET);
        int t;
        unsigned char *tp;
        for (t = 0, tp = texture; t < numt; t++)
        {
            for (int s = 0; s < nums; s++, tp += 3)
            {
                int index = fgetc(fp);
                *(tp + 0) = colorTable[index].r; // r
                *(tp + 1) = colorTable[index].g; // g
                *(tp + 2) = colorTable[index].b; // b
            }

            for (int e = 0; e < numExtra; e++)
            {
                fgetc(fp);
            }
        }

        delete[] colorTable;
    }

    fclose(fp);

    *width = nums;
    *height = numt;
    return texture;
}

int ReadInt(FILE *fp)
{
    const unsigned char b0 = fgetc(fp);
    const unsigned char b1 = fgetc(fp);
    const unsigned char b2 = fgetc(fp);
    const unsigned char b3 = fgetc(fp);
    return (b3 << 24) | (b2 << 16) | (b1 << 8) | b0;
}

short ReadShort(FILE *fp)
{
    const unsigned char b0 = fgetc(fp);
    const unsigned char b1 = fgetc(fp);
    return (b1 << 8) | b0;
}

// function to convert HSV to RGB
// 0.  <=  s, v, r, g, b  <=  1.
// 0.  <= h  <=  360.
// when this returns, call:
//		glColor3fv( rgb );

void HsvRgb(float hsv[3], float rgb[3])
{
    // guarantee valid input:

    float h = hsv[0] / 60.f;
    while (h >= 6.)
        h -= 6.;
    while (h < 0.)
        h += 6.;

    float s = hsv[1];
    if (s < 0.)
        s = 0.;
    if (s > 1.)
        s = 1.;

    float v = hsv[2];
    if (v < 0.)
        v = 0.;
    if (v > 1.)
        v = 1.;

    // if sat==0, then is a gray:

    if (s == 0.0)
    {
        rgb[0] = rgb[1] = rgb[2] = v;
        return;
    }

    // get an rgb from the hue itself:

    float i = (float)floor(h);
    float f = h - i;
    float p = v * (1.f - s);
    float q = v * (1.f - s * f);
    float t = v * (1.f - (s * (1.f - f)));

    float r = 0., g = 0., b = 0.; // red, green, blue
    switch ((int)i)
    {
    case 0:
        r = v;
        g = t;
        b = p;
        break;

    case 1:
        r = q;
        g = v;
        b = p;
        break;

    case 2:
        r = p;
        g = v;
        b = t;
        break;

    case 3:
        r = p;
        g = q;
        b = v;
        break;

    case 4:
        r = t;
        g = p;
        b = v;
        break;

    case 5:
        r = v;
        g = p;
        b = q;
        break;
    }

    rgb[0] = r;
    rgb[1] = g;
    rgb[2] = b;
}

void Cross(float v1[3], float v2[3], float vout[3])
{
    float tmp[3];
    tmp[0] = v1[1] * v2[2] - v2[1] * v1[2];
    tmp[1] = v2[0] * v1[2] - v1[0] * v2[2];
    tmp[2] = v1[0] * v2[1] - v2[0] * v1[1];
    vout[0] = tmp[0];
    vout[1] = tmp[1];
    vout[2] = tmp[2];
}

float Dot(float v1[3], float v2[3])
{
    return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

float Unit(float vin[3], float vout[3])
{
    float dist = vin[0] * vin[0] + vin[1] * vin[1] + vin[2] * vin[2];
    if (dist > 0.0)
    {
        dist = sqrtf(dist);
        vout[0] = vin[0] / dist;
        vout[1] = vin[1] / dist;
        vout[2] = vin[2] / dist;
    }
    else
    {
        vout[0] = vin[0];
        vout[1] = vin[1];
        vout[2] = vin[2];
    }
    return dist;
}
