// Intro to Computer Graphics - Final Project - Procedural Planet Generation
// Author - Nicholas Olson
// Date -   11/28/2021
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
        Yrot += 180.f * ((float)elapsedMS) / SCENE_ROTATE_TIME;
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

    // set the eye position, look-at position, and up-vector dending on view
    gluLookAt(0., 0., 5., 0., 0., 0., 0., 1., 0.);

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
    // Draw w/ shaders
    Shader->Use();
        // set matrix uniforms every frame
        float modelViewArr[16]; 
        float projArr[16]; 
        float modelViewProjArr[16]; 
        glGetFloatv(GL_MODELVIEW_MATRIX, modelViewArr);
        glGetFloatv(GL_PROJECTION_MATRIX, projArr);
        glm::mat4 matrixModelView = glm::make_mat4(modelViewArr);
        glm::mat4 matrixProj = glm::make_mat4(projArr);
        glm::mat4 matrixModelViewProjection = matrixProj * matrixModelView;
        glm::mat4 matrixNormal = matrixModelView;
        matrixNormal[3] = glm::vec4(0,0,0,1);

        Shader->SetUniformVariableMat4("matrixModelView", &matrixModelView[0][0]);
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
}

// Generates planet noise into gen'd `texId`, returning ave noise value
float GenerateNewPlanet() {
    // Declare return val
    float aveNoise;

    // If 2D, not what we want
    if (!THREE_DEE) {
        // Setup and generate 2D noise for planet
        int baseSeed = time(NULL) % 10000;   // this one
        const int numLayers = 9;  // this one
        float baseFreq = .01;
        float baseAmp = 1.4;  // this one
        const int dim = 128;
        float noiseData[dim*dim];
        std::fill_n(noiseData, dim*dim, 1);
        int index;
        float total = 0;

        FastNoiseLite noise_generators[numLayers];
        for (int i = 0; i < numLayers; i++) {
            FastNoiseLite noise = noise_generators[i];
            noise.SetSeed(baseSeed + (i+1)*100);
            noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            noise.SetFrequency((1+i*1.5)*baseFreq);
                    noise.SetFractalLacunarity(2.f);
                    noise.SetFractalGain(0.5f);
                    noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
            index = 0;
            for (int y = 0; y < dim; y++)
                for (int x = 0; x < dim; x++) {
                    noiseData[index++] *= (1-float(i)/float(numLayers))*baseAmp*(0.5 + ( noise.GetNoise((float)x, (float)y) + 1 ) / 2.);
                    if (i == numLayers-1) total += noiseData[index-1];
                }
        }

        aveNoise = total / float(dim*dim);
        sphere2_rad = aveNoise;

        // Use noise as texture for icosphere
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId); // bind

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // or GL_CLAMP
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // or GL_LINEAR
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dim, dim, 0,
                     GL_RED, GL_FLOAT, noiseData);

        glBindTexture(GL_TEXTURE_2D, 0); // unbind
    }

    // 3D Mapping (What we Want)
    else if (THREE_DEE) {

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
                        v = (v+1)*0.5f*amplitude; // v = [0, amplitude]
                        v = std::max(0.f, v-min); // v = [max(0, min), amplitude]
                        v *= strength; // [max(0, min*strength), amplitude*strength]

                        // Place val in image
                        noiseData3D[index-1] += v; // store

                        // If in last layer, contribute to average noise val
                        if (i == numLayers-1) {
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
    }

    return aveNoise;
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
    float aveNoise = GenerateNewPlanet();


    // Setup shader uniforms
    IcoShader->Use();
        // Set uniforms
        glm::vec3 lightColor = glm::vec3(1., 1., 1.);
        glm::vec3 objColor1 = glm::vec3(0.3, 0.5, 0.2);
        // glm::vec3 objColor2 = glm::vec3(0.1, 0.45, 0.1); // green under
        glm::vec3 objColor2 = glm::vec3(0.1, 0.3, 0.7);  // blue under oceans
        glm::vec3 lightPos = glm::vec3(0.5, 0.5, 1.);
        const float lAmb = 0.6, lDif = 0.8, lSpec = 0.6;
        const float oAmb1 = 0.5, oDif1 = 0.7, oSpec1 = 0.2, oShin1 = 0.5;
        const float oAmb2 = 0.5, oDif2 = 0.7, oSpec2 = 0.4, oShin2 = 5;

        IcoShader->SetUniformVariable("lightPosition", glm::vec4(lightPos, 0));
        IcoShader->SetUniformVariable("lightAmbient", glm::vec4(lAmb*lightColor, 1));
        IcoShader->SetUniformVariable("lightDiffuse", glm::vec4(lDif*lightColor, 1));
        IcoShader->SetUniformVariable("lightSpecular", glm::vec4(lSpec*lightColor, 1));
        IcoShader->SetUniformVariable("materialAmbient1", glm::vec4(oAmb1*objColor1, 1));
        IcoShader->SetUniformVariable("materialDiffuse1", glm::vec4(oDif1*objColor1, 1));
        IcoShader->SetUniformVariable("materialSpecular1", glm::vec4(oSpec1*objColor1, 1));
        IcoShader->SetUniformVariable("materialShininess1", oShin1);
        IcoShader->SetUniformVariable("materialAmbient2", glm::vec4(oAmb2*objColor2, 1));
        IcoShader->SetUniformVariable("materialDiffuse2", glm::vec4(oDif2*objColor2, 1));
        IcoShader->SetUniformVariable("materialSpecular2", glm::vec4(oSpec2*objColor2, 1));
        IcoShader->SetUniformVariable("materialShininess2", oShin2);
        IcoShader->SetUniformVariable("oceanThresh", oceanThresh);
        IcoShader->SetUniformVariable("aveNoise", aveNoise);
        IcoShader->SetUniformVariable("useNoise", 1);
        IcoShader->SetUniformVariable("map0", 0);
    IcoShader->UnUse();
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
    int dz = 0;

    if ((ActiveButton & LEFT) != 0)
    {
        Xrot += (ANGFACT * dy);
        Yrot += (ANGFACT * dx);
        Zrot += (ANGFACT * dz);
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
    ActiveButton = 0;
    WhichProjection = ORTHO;
    DrawHelpText = false;
    Scale = 1.5;
    fixedLighting = true;
    oceanThresh = oceanThreshDefVal;
    SceneRotateOn = false;

    // reset icospheres
    sphere.setRadius(1.f);
    sphere.setSubdivision(5);
    sphere.setSmooth(true);
    if (redisplay) RecalcSphereMesh(sphere, vboId, iboId);
    sphere2.setRadius(sphere2_rad);
    sphere2.setSubdivision(5);
    sphere2.setSmooth(true);
    if (redisplay) RecalcSphereMesh(sphere2, vboId2, iboId2);

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

