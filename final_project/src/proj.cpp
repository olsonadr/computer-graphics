// Intro to Computer Graphics - Final Project - Procedural Planet Generation
// Author - Nicholas Olson
// Date -   12/07/2021
// Icosphere Source - http://www.songho.ca/opengl/gl_sphere.html
// Inpritation Source - https://www.youtube.com/playlist?list=PLFt_AvWsXl0cONs3T0By4puYy6GM22ko8


//	The left mouse button does rotation
//	The middle mouse button does scaling


// Primary include for includes, paramaters, prototypes, and constants of this program
#include "../include/proj.hpp"


// main program
int main(int argc, char *argv[]) {
    // turn on the glut package:
    // (do this before checking argc and argv since it might
    // pull some command line arguments out)
    glutInit(&argc, argv);

    // init all the global variables used by Display() without redisplaying:
    Reset(false);

    // setup all the graphics stuff:
    InitGraphics();

    // setup all the user interface stuff:
    InitMenus();

    // init all the global variables used by Display() with a redisplay
    Reset(true);

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
    // Get time since last update and enforce new val
    int ms = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMS = ms - msOfPriorAnimate;
    msOfPriorAnimate = ms;

    // Handle scene rotation if enabled
    if (SceneRotateOn) {
        // Yrot += 180.f * ((float)elapsedMS) / SCENE_ROTATE_TIME; // rotate view
        OYrot += 180.f * ((float)elapsedMS) / SCENE_ROTATE_TIME; // rotate object
    }

    // force a call to Display( ) next time it is convenient:
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// draw the complete scene:
void Display()
{
    // set which window we want to do the graphics into:
    glutSetWindow(MainWindow);

    // erase the background:
    glDrawBuffer(GL_BACK);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);

    // specify shading to be default flat:
    glShadeModel(GL_FLAT);

    // set viewport to be the entire window
    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (WhichProjection == ORTHO) {
        // squish appropriately
        if (screenWidth != screenHeight) 
            glScalef(1., (float)screenWidth/(float)screenHeight, 1.);
        glOrtho(-3., 3., -3., 3., 0.1, 1000.);
    } else {
        gluPerspective(45.f, (float)(screenWidth)/screenHeight, 0.1f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip
    }

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // since we are using glScalef( ), be sure normals get unitized:
    glEnable(GL_NORMALIZE);

    // set the eye position, look-at position, and up-vector
    gluLookAt(eyeX, eyeY, eyeZ, 0., 0., 0., 0., 1., 0.);

    // draw skybox sphere w/ shader before scaling or rotating
    DrawSkyBox();

    // uniformly rotate and scale the scene
    glRotatef((GLfloat)Xrot, 1., 0., 0.);
    glRotatef((GLfloat)Yrot, 0., 1., 0.);
    if (Scale < MINSCALE)
        Scale = MINSCALE;
    glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);

    // Draw icospheres w/ shaders
    DrawIcoSphere(sphere, vboId, iboId, IcoShader, true);
    DrawIcoSphere(sphere2, vboId2, iboId2, IcoShader, false);

    


    // maybe draw some gratuitous text that is fixed on the screen:
    //  the projection matrix is reset to define a scene whose
    //  world coordinate system goes from 0-100 in each axis
    // this is called "percent units", and is just a convenience
    // the modelview matrix is reset to identity as we don't
    //  want to transform these coordinates
    glDisable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0., 100., 0., 100.);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glScalef(.4, .4, .4);
    char sliceStr[50];
    if (DrawHelpText) {
        glColor3f(1., 1., 1.);
        sprintf(sliceStr,
                "Ctrls: Space, Enter, WASD, L, M, O, P, R, Q, ?");
        DoRasterString(5., 5., 0., sliceStr);
    }

    // swap the double-buffered framebuffers:
    glutSwapBuffers();

    // be sure the graphics buffer has been sent:
    // note: be sure to use glFlush( ) here, not glFinish( ) !
    glFlush();
}

void DrawIcoSphere(Icosphere sphere,
                   GLuint &vbo, GLuint &ibo,
                   GLSLProgram *Shader,
                   bool useNoise = true) {
    glPushMatrix();

    // Draw w/ shaders
    Shader->Use();
        // set matrix uniforms every frame
        float modelViewArr[16]; 
        float projArr[16]; 
        float modelViewProjArr[16]; 
        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewArr);
        glGetFloatv(GL_PROJECTION_MATRIX, projArr);
        glm::mat4 matrixWorldView = glm::make_mat4(modelViewArr);

        // rotate the object using OXRot, OYRot
        glRotatef((GLfloat)OXrot, 1., 0., 0.);
        glRotatef((GLfloat)OYrot, 0., 1., 0.);

        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewArr);
        glm::mat4 matrixModelView = glm::make_mat4(modelViewArr);
        glm::mat4 matrixProj = glm::make_mat4(projArr);
        glm::mat4 matrixModelViewProjection = matrixProj * matrixModelView;
        glm::mat4 matrixNormal = matrixModelView;
        matrixNormal[3] = glm::vec4(0,0,0,1);

        Shader->SetUniformVariableMat4("matrixModelView", &matrixModelView[0][0]);
        Shader->SetUniformVariableMat4("matrixWorldView", &matrixWorldView[0][0]);
        Shader->SetUniformVariableMat4("matrixModelViewProjection", &matrixModelViewProjection[0][0]);
        Shader->SetUniformVariableMat4("matrixNormal", &matrixNormal[0][0]);
        Shader->SetUniformVariable("useNoise", useNoise);
        Shader->SetUniformVariable("fixedLighting", fixedLighting);

        // bind VBOs
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        // activate attribs
        glEnableVertexAttribArray(attribVertexPosition);
        glEnableVertexAttribArray(attribVertexNormal);
        glEnableVertexAttribArray(attribVertexTexCoord);

        // set attrib arrays using glVertexAttribPointer()
        int stride = sphere.getInterleavedStride();
        glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
        glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
        glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));

        if (THREE_DEE) {
            glBindTexture(GL_TEXTURE_3D, texId);
        } else {
            glBindTexture(GL_TEXTURE_2D, texId);
        }
        
        // Draw the sphere!
        glDrawElements(GL_TRIANGLES,            // primitive type
                       sphere.getIndexCount(),  // # of indices
                       GL_UNSIGNED_INT,         // data type
                       (void*)0);               // ptr to indices

        if (THREE_DEE) {
            glBindTexture(GL_TEXTURE_3D, 0);
        } else {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glDisableVertexAttribArray(attribVertexPosition);
        glDisableVertexAttribArray(attribVertexNormal);
        glDisableVertexAttribArray(attribVertexTexCoord);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Shader->UnUse();

    glPopMatrix();
}

// Generates planet noise into gen'd `texId`, returning ave noise value
float GenerateNewPlanet() {
    // Declare return val
    // float aveNoise;

    // Setup and generate 3D noise for planet
    const int dim3D = 60;
    float noiseData3D[dim3D*dim3D*dim3D];
    std::fill_n(noiseData3D, dim3D*dim3D*dim3D, 0);

    FastNoiseLite noise;
    int baseSeed = time(NULL) % 10000;
    noise.SetSeed(baseSeed);
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
    noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
    noise.SetFractalLacunarity(3.f); // sweetspot!
    noise.SetFractalGain(0.5f); // sweetspot!
    // noise.SetFractalLacunarity(6.f); // sweetspot?
    // noise.SetFractalGain(0.4f); // sweetspot?
    // noise.SetFractalGain(0.9f); // sweetspot?
    // noise.SetFractalGain(1.f); // sweetspot?
    // noise.SetFractalLacunarity(1.f); // bigger, taller, smoother
    // noise.SetFractalGain(0.1f); // bigger, taller, smoother

    float total = 0;
    float count = 0;
    int index;
    
    const int numLayers = 9;
    float baseFreq = .02;
    float baseAmp = 0.9;
    float amplitude = baseAmp;
    float frequency = baseFreq;
    float roughness = 3.;
    float persistence = 0.5;
    float min = 0;
    float strength = 0.5;

    // lambdas used to filter the noise (choose one)
    auto filter = [](float x) { return pow(1.-abs(sin(x)), 2); }; // peaks
    // auto filter = [](float x) { return pow(1.-abs(sin(x)), 4); }; // bigger peaks
    // auto filter = [](float x) { return x; }; // identity

    // Apply all layers of noise
    bool first = true;
    for (int i = 0; i < numLayers; i++) {
        noise.SetFrequency(frequency);
        index = 0;
        for (int y = 0; y < dim3D; y++) {
            for (int x = 0; x < dim3D; x++) {
                for (int z = 0; z < dim3D; z++) {
                    // Move to this idx
                    index++;

                    // Filter and modify noise val
                    float v = noise.GetNoise((float)x, (float)y, (float)z); // v = [-1,1]
                    // v = filter(v);
                    v = (v+1)*0.5f*amplitude; // v = [0, amplitude]
                    v = std::max(0.f, v-min); // v = [max(0, min), amplitude]
                    v *= strength; // [max(0, min*strength), amplitude*strength]

                    // Place val in image
                    noiseData3D[index-1] += v; // store

                    // If in last layer, contribute to average noise val
                    if (i == numLayers-1) {
                        // If first element, set initial min and max 
                        if (first) {
                            minNoise = noiseData3D[index - 1];
                            maxNoise = noiseData3D[index - 1];
                            first = false;
                        }
                        // Check if new min/max
                        if (noiseData3D[index-1] < minNoise)
                            minNoise = noiseData3D[index-1];
                        if (noiseData3D[index-1] > maxNoise)
                            maxNoise = noiseData3D[index-1];
                        // Contribute to ave
                        total += noiseData3D[index-1];
                        count++;
                    }
                }
            }
        }

        // Update freq and amp for next layer
        frequency *= roughness;
        amplitude *= persistence;
    }

    aveNoise = total / count;
    sphere2_rad = oceanThresh+(aveNoise);

    // std::cout << minNoise << " " << maxNoise << " " << aveNoise << " " << 0.99 * (maxNoise - minNoise) + minNoise << std::endl;

    // Update texture in texId with this new data
    glBindTexture(GL_TEXTURE_3D, texId); // bind

    // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Setup mon and mag filter and mipmap stuff
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP, GL_TRUE);

    // Setup wrap parameter
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // or GL_CLAMP
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

    // Copy 3D noise texture data
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, dim3D, dim3D, dim3D, 0,
                    GL_RED, GL_FLOAT, noiseData3D);

    // Generate mipmap for 3d texture
    glGenerateMipmap(GL_TEXTURE_3D);

    // Unbind tex
    glBindTexture(GL_TEXTURE_3D, 0);

    return aveNoise;
}


void DrawSkyBox()
{
    glPushMatrix();

    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
    GLboolean OldDepthMask;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &OldDepthMask);

    glCullFace(GL_BACK);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    SkyShader->Use();
        // Translate origin back to eye
        glTranslatef(eyeX, eyeY, eyeZ);

        // Scale and rotate scene
        const float skyScale = 5;
        glScalef(skyScale, skyScale, skyScale);
        // glRotatef((GLfloat)Xrot, -1., 0., 0.);
        // glRotatef((GLfloat)Yrot, 0., 1., 0.);
        glRotatef((GLfloat)Xrot, 1., 0., 0.);
        glRotatef((GLfloat)Yrot, 0., 1., 0.);


        // set matrix uniform every frame
        float modelViewArr[16]; 
        float projArr[16]; 
        float modelViewProjArr[16]; 
        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewArr);
        glGetFloatv(GL_PROJECTION_MATRIX, projArr);
        glm::mat4 matrixModelView = glm::make_mat4(modelViewArr);
        glm::mat4 matrixProj = glm::make_mat4(projArr);
        glm::mat4 matrixModelViewProjection = matrixProj * matrixModelView;
        // SkyShader->SetUniformVariableMat4("gWVP", &matrixModelView[0][0]);
        SkyShader->SetUniformVariableMat4("gWVP", &matrixModelViewProjection[0][0]);

        glLoadIdentity(); // so that the uniform has only impact

        // bind VBOs
        glBindBuffer(GL_ARRAY_BUFFER, vboId3);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId3);

        // activate attrib
        glEnableVertexAttribArray(attribPosition);

        // set attrib arrays using glVertexAttribPointer()
        int stride = skySphere.getInterleavedStride();
        glVertexAttribPointer(attribPosition, 3, GL_FLOAT, false, stride, 0);

        // bind texture
        skyTexArr[CurrSky]->Bind(GL_TEXTURE0);
        // skyTex->Bind(0);
        
        // Draw the sphere!
        glDrawElements(GL_TRIANGLES,               // primitive type
                       skySphere.getIndexCount(),  // # of indices
                       GL_UNSIGNED_INT,            // data type
                       (void*)0);                  // ptr to indices


        // Unbind texture
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        // Disable vertex attrib
        glDisableVertexAttribArray(attribPosition);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    SkyShader->UnUse();

    glCullFace(OldCullFaceMode);
    glDepthFunc(OldDepthFuncMode);
    glDepthMask(OldDepthMask);

    glPopMatrix();
}

void SetSkyBoxUniforms(bool lite) {
    // Setup shader uniforms
    IcoShader->Use();
        glm::vec3 landCol1 = glm::vec3(0.3, 0.7, 0.2);
        // glm::vec3 landCol2 = glm::vec3(0.2, 0.15, 0.07); // brown mountains
        glm::vec3 landCol2 = glm::vec3(0.3, 0.3, 0.3); // grey mountains
        glm::vec3 oceanCol = glm::vec3(0.1, 0.3, 0.7);  // blue oceans
        const float lAmb = 0.6, lDif = 0.8, lSpec = 0.6;
        const float oAmbLand1 = 0.5, oDifLand1 = 0.7, oSpecLand1 = 0.2, oShinLand1 = 0.5;
        const float oAmbLand2 = 0.5, oDifLand2 = 0.7, oSpecLand2 = 0.2, oShinLand2 = 0.5;
        const float oAmbOcean = 0.5, oDifOcean = 0.7, oSpecOcean = 0.4, oShinOcean = 5;

        IcoShader->SetUniformVariable("lightPosition", glm::vec4(SkyLightPos[CurrSky], 0));
        IcoShader->SetUniformVariable("lightAmbient", lAmb*SkyAmbCol[CurrSky]);
        IcoShader->SetUniformVariable("lightDiffuse", lDif*SkyLightCol[CurrSky]);
        IcoShader->SetUniformVariable("lightSpecular", lSpec*SkyLightCol[CurrSky]);

        if (!lite) {
            IcoShader->SetUniformVariable("materialAmbientLand1", glm::vec4(oAmbLand1*landCol1, 1));
            IcoShader->SetUniformVariable("materialDiffuseLand1", glm::vec4(oDifLand1*landCol1, 1));
            IcoShader->SetUniformVariable("materialSpecularLand1", glm::vec4(oSpecLand1*landCol1, 1));
            IcoShader->SetUniformVariable("materialShininessLand1", oShinLand1);
            IcoShader->SetUniformVariable("materialAmbientLand2", glm::vec4(oAmbLand2*landCol2, 1));
            IcoShader->SetUniformVariable("materialDiffuseLand2", glm::vec4(oDifLand2*landCol2, 1));
            IcoShader->SetUniformVariable("materialSpecularLand2", glm::vec4(oSpecLand2*landCol2, 1));
            IcoShader->SetUniformVariable("materialShininessLand2", oShinLand2);
            IcoShader->SetUniformVariable("materialAmbientOcean", glm::vec4(oAmbOcean*oceanCol, 1));
            IcoShader->SetUniformVariable("materialDiffuseOcean", glm::vec4(oDifOcean*oceanCol, 1));
            IcoShader->SetUniformVariable("materialSpecularOcean", glm::vec4(oSpecOcean*oceanCol, 1));
            IcoShader->SetUniformVariable("materialShininessOcean", oShinOcean);
            IcoShader->SetUniformVariable("oceanThresh", oceanThresh);
            IcoShader->SetUniformVariable("aveNoise", aveNoise);
            IcoShader->SetUniformVariable("minNoise", minNoise);
            IcoShader->SetUniformVariable("maxNoise", maxNoise);
            IcoShader->SetUniformVariable("useNoise", 1);
            IcoShader->SetUniformVariable("map0", 0);
        }
    IcoShader->UnUse();
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

        // clean up VBOs
        if(vboSupported) {
            glDeleteBuffers(1, &vboId);
            glDeleteBuffers(1, &iboId);
            glDeleteBuffers(1, &vboId2);
            glDeleteBuffers(1, &iboId2);
            vboId = iboId = 0;
            vboId2 = iboId2 = 0;
        }

        // Cleanup skybox
        for (int i = 0; i < NumSkies; i++) {
            delete skyTexArr[i];
        }
        free(skyTexArr);

        // Quit
        exit(0);
        break;

    default:
        fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
    }

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

    int mainmenu = glutCreateMenu(DoMainMenu);
    glutAddMenuEntry("Reset", RESET);
    glutAddMenuEntry("Quit", QUIT);

    // attach the pop-up menu to the right mouse button:
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// initialize the glut and OpenGL libraries:
//	also setup display lists and callback functions
void InitGraphics()
{
    // request the display modes:
    //  ask for red-green-blue-alpha color, double-buffering, and z-buffering:
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH);

    // set the initial window configuration:
    glutInitWindowPosition(0, 0);
    glutInitWindowSize(1.5*INIT_WINDOW_SIZE, INIT_WINDOW_SIZE);

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
    GLenum err = glewInit();
    if (err != GLEW_OK)
        fprintf(stderr, "glewInit Error\n");
    else
        fprintf(stderr, "GLEW initialized OK\n");
        
    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));


    // Icosphere shader setup
    char icovert[50];
    char icofrag[50];

    if (THREE_DEE) {
        strcpy(icovert, "shaders/icosphere3D.vert");
        strcpy(icofrag, "shaders/icosphere3D.frag");
    } else {
        strcpy(icovert, "shaders/icosphere.vert");
        strcpy(icofrag, "shaders/icosphere.frag");
    }

    int ico_attrib_idx[3] = {0, 1, 2};
    const char *ico_attrib_names[3] = {"vertexPosition", "vertexNormal", "vertexTexCoord"};

    IcoShader = new GLSLProgram(3, ico_attrib_idx, ico_attrib_names);
    bool valid = IcoShader->Create(icovert, icofrag);

    if (valid) {
        fprintf(stderr, "Ico shader created.\n");
    } else {
        fprintf(stderr, "Ico shader cannot be created!\n");
        DoMainMenu(QUIT);
    }

    // Get attrib locations
    attribVertexPosition = glGetAttribLocation(IcoShader->Program, "vertexPosition");
    attribVertexNormal   = glGetAttribLocation(IcoShader->Program, "vertexNormal");
    attribVertexTexCoord = glGetAttribLocation(IcoShader->Program, "vertexTexCoord");

    // Copy interleaved vertex data from terrain icosphere
    vboSupported = true;
    glGenBuffers(1, &vboId);
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER,                      // target
                    sphere.getInterleavedVertexSize(), // # of bytes
                    sphere.getInterleavedVertices(),   // ptr to vertices
                    GL_STATIC_DRAW);                   // usage
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &iboId);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sphere.getIndexSize(),
                    sphere.getIndices(),
                    GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Copy interleaved vertex data from ocean icosphere
    glGenBuffers(1, &vboId2);
    glBindBuffer(GL_ARRAY_BUFFER, vboId2);
    glBufferData(GL_ARRAY_BUFFER,                      // target
                    sphere2.getInterleavedVertexSize(), // # of bytes
                    sphere2.getInterleavedVertices(),   // ptr to vertices
                    GL_STATIC_DRAW);                   // usage
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &iboId2);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sphere2.getIndexSize(),
                    sphere2.getIndices(),
                    GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // Generate new planet noise
    glGenTextures(1, &texId); // prepare texId to be used in here
    GenerateNewPlanet();

    // Generate all Skybox textures
    skyTexArr = (CubemapTexture **) malloc(sizeof(CubemapTexture)*NumSkies);
    for (int i = 0; i < NumSkies; i++) {
        skyTexArr[i] = new CubemapTexture(SkyPaths[i], SkyPosXFilename, SkyNegXFilename, SkyPosYFilename,
                                          SkyNegYFilename, SkyPosZFilename, SkyNegZFilename);
        skyTexArr[i]->Load();
    }

    // Setup ico shader uniforms
    SetSkyBoxUniforms(false);

    // Get current skyTex and bind it
    //skyTex = skyTexArr[CurrSky];
    skyTexArr[CurrSky]->Load();

    // Setup Skybox shader
    char sbvert[50];
    char sbfrag[50];
    strcpy(sbvert, "shaders/skybox.vert");
    strcpy(sbfrag, "shaders/skybox.frag");

    const int sb_n = 1;
    int sb_attrib_idx[sb_n] = {0};
    const char *sb_attrib_names[sb_n] = {"Position"};

    SkyShader = new GLSLProgram(sb_n, sb_attrib_idx, sb_attrib_names);
    valid = SkyShader->Create(sbvert, sbfrag);

    if (valid) {
        fprintf(stderr, "Sky shader created.\n");
    } else {
        fprintf(stderr, "Sky shader cannot be created!\n");
        DoMainMenu(QUIT);
    }

    // Get skybox attrib locations
    attribPosition = glGetAttribLocation(SkyShader->Program, "Position");

    // Setup skybox shader uniforms
    SkyShader->Use();
        SkyShader->SetUniformVariable("gCubemapTexture", 0);
    SkyShader->UnUse();

    // Setup skySphere vbo and ibo
    glGenBuffers(1, &vboId3);
    glBindBuffer(GL_ARRAY_BUFFER, vboId3);
    glBufferData(GL_ARRAY_BUFFER,                      // target
                 skySphere.getInterleavedVertexSize(), // # of bytes
                 skySphere.getInterleavedVertices(),   // ptr to vertices
                 GL_STATIC_DRAW);                      // usage
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &iboId3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 skySphere.getIndexSize(),
                 skySphere.getIndices(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// Recalculates the mesh for an Icosphere using vboId, iboId
void RecalcSphereMesh(Icosphere sphere, GLuint &vboId, GLuint &iboId) {
    // Copy interleaved vertex data from icosphere
    glBindBuffer(GL_ARRAY_BUFFER, vboId);
    glBufferData(GL_ARRAY_BUFFER,                      // target
                    sphere.getInterleavedVertexSize(), // # of bytes
                    sphere.getInterleavedVertices(),   // ptr to vertices
                    GL_STATIC_DRAW);                   // usage
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sphere.getIndexSize(),
                    sphere.getIndices(),
                    GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// the keyboard callback:
void Keyboard(unsigned char c, int x, int y)
{
    int sub;
    float subf;

    if (DebugOn != 0)
        fprintf(stderr, "Keyboard: '%c' (0x%0x)\n", c, c);

    switch (c)
    {
    case 'o':
    case 'O':
        WhichProjection = ORTHO;
        break;

    case 'p':
    case 'P':
        WhichProjection = PERSP;
        break;

    case 's':
        // Update ico subdivision
        sub = sphere.getSubdivision() - 1;
        if (sub < 0) {sub = 0;}
        else { sphere.setSubdivision(sub);
               RecalcSphereMesh(sphere, vboId, iboId);
               sphere2.setSubdivision(sub);
               RecalcSphereMesh(sphere2, vboId2, iboId2); }
        break;
    case 'w':
        // Update ico subdivision
        sub = sphere.getSubdivision() + 1;
        if (sub > 6) {sub = 6;}
        else { sphere.setSubdivision(sub);
               RecalcSphereMesh(sphere, vboId, iboId);
               sphere2.setSubdivision(sub);
               RecalcSphereMesh(sphere2, vboId2, iboId2); }
        break;

    case 'a':
        // Decrement oceanThresh
        subf = oceanThresh - (calcOcean(1)-oceanThreshMin);
        if (subf >= oceanThreshMin) {
            sphere2_rad = sphere2_rad - oceanThresh + subf;
            oceanThresh = subf;
            sphere2.setRadius(sphere2_rad);
            RecalcSphereMesh(sphere2, vboId2, iboId2);
            IcoShader->Use();
                IcoShader->SetUniformVariable("oceanThresh", oceanThresh);
            IcoShader->UnUse();
        }
        break;
    case 'd':
        // Increment oceanThresh
        subf = oceanThresh + (calcOcean(1)-oceanThreshMin);
        if (subf <= oceanThreshMax) {
            sphere2_rad = sphere2_rad - oceanThresh + subf;
            oceanThresh = subf;
            sphere2.setRadius(sphere2_rad);
            RecalcSphereMesh(sphere2, vboId2, iboId2);
            IcoShader->Use();
                IcoShader->SetUniformVariable("oceanThresh", oceanThresh);
            IcoShader->UnUse();
        }
        break;

    case 'M' : case 'm' :
        // Invert ico smooth/flat shading
        sphere.setSmooth(!sphere.getSmooth());
        RecalcSphereMesh(sphere, vboId, iboId);
        sphere2.setSmooth(!sphere2.getSmooth());
        RecalcSphereMesh(sphere2, vboId2, iboId2);
        break;

    case 'L':
    case 'l':
        // Toggle fixed lighting
        fixedLighting = !fixedLighting;
        break;

    case ' ':
        // Toggle scene rotation
        SceneRotateOn = !SceneRotateOn;
        break;

    case char(13):
        // Generate new planet
        GenerateNewPlanet();
        break;


    case 'r':
    case 'R':
        Reset(true);
        break;


    case '?':
        DrawHelpText = !DrawHelpText;
        break;


    case '[':
        CurrSky = static_cast<Skies>(static_cast<int>(CurrSky) - 1);
        if (CurrSky < 0) CurrSky = static_cast<Skies>(0);
        SetSkyBoxUniforms(true);
        break;
    case ']':
        CurrSky = static_cast<Skies>(static_cast<int>(CurrSky) + 1);
        if (CurrSky >= NumSkies) CurrSky = static_cast<Skies>(NumSkies-1);
        SetSkyBoxUniforms(true);
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
        Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
        // keep object from turning inside-out or disappearing:
        if (Scale < MINSCALE)
            Scale = MINSCALE;
        break;

    case SCROLL_WHEEL_DOWN:
        Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
        // keep object from turning inside-out or disappearing:
        if (Scale < MINSCALE)
            Scale = MINSCALE;
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
    int dx = x - Xmouse; // change in mouse coords
    int dy = y - Ymouse;

    if ((ActiveButton & LEFT) != 0)
    {
        Xrot += (ANGFACT * dy);
        Yrot += (ANGFACT * dx);
    }

    if ((ActiveButton & MIDDLE) != 0)
    {
        Scale += SCLFACT * (float)(dx - dy);

        // keep object from turning inside-out or disappearing:
        if (Scale < MINSCALE)
            Scale = MINSCALE;
    }

    Xmouse = x; // new current position
    Ymouse = y;

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene
void Reset(bool redisplay)
{
    // reset general
    Xrot = 0;
    Yrot = 0;
    Zrot = 0.;
    OXrot = 0;
    OYrot = 0;
    ActiveButton = 0;
    WhichProjection = PERSP;
    DrawHelpText = false;
    Scale = 0.7;
    fixedLighting = false;
    oceanThresh = oceanThreshDefVal;
    SceneRotateOn = false;

    // reset icospheres
    sphere.setRadius(1.f);
    sphere.setSubdivision(5);
    sphere.setSmooth(true);
    sphere2.setRadius(sphere2_rad);
    sphere2.setSubdivision(5);
    sphere2.setSmooth(true);
    if (redisplay) RecalcSphereMesh(sphere, vboId, iboId);
    if (redisplay) RecalcSphereMesh(sphere2, vboId2, iboId2);
    if (redisplay) RecalcSphereMesh(skySphere, vboId3, iboId3);
    
    // reset CurrSky
    CurrSky = NORMAL;

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

