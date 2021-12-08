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

    // create the display structures that will not change:
    InitLists();

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
    // put animation stuff in here -- change some global variables
    // for Display( ) to find:

    // Get time since last update and enforce new val
    int ms = glutGet(GLUT_ELAPSED_TIME);
    int elapsedMS = ms - msOfPriorAnimate;
    msOfPriorAnimate = ms;

    // Handle scene rotation if enabled
    if (SceneRotateOn) {
        Yrot += 180.f * ((float)elapsedMS) / SCENE_ROTATE_TIME;
    }


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

    // Helpful lambdas
    float epsilon = 0.001;
    auto offset_theta = [](double x, double z, double r, double theta, double epsilon) {
        return theta+atan(z/(x+epsilon))+(x+epsilon>0 ? 0 : M_PI); };
    auto offset_x = [](double x, double z, double r, double theta, double epsilon, auto offset_theta) {
        return r*cos(offset_theta(x,z,r,theta,epsilon)); };
    auto offset_z = [](double x, double z, double r, double theta, double epsilon, auto offset_theta) {
        return r*sin(offset_theta(x,z,r,theta,epsilon)); };

    // Handle object animations
    float ObjLightAnimRad = 2.;
    if (ObjAnimOn) {
        float ObjectRotDiff = 2*M_PI * float(elapsedMS) / OBJECT_ROT_TIME;
        ObjectRot = mod((ObjectRot + ObjectRotDiff), 2*M_PI);
        
        Obj2.X = offset_x(Obj2.X, Obj2.Z, ObjLightAnimRad, ObjectRotDiff, epsilon, offset_theta);
        Obj2.Z = offset_z(Obj2.X, Obj2.Z, ObjLightAnimRad, ObjectRotDiff, epsilon, offset_theta);

        Obj3.X = offset_x(Obj3.X, Obj3.Z, ObjLightAnimRad, ObjectRotDiff, epsilon, offset_theta);
        Obj3.Z = offset_z(Obj3.X, Obj3.Z, ObjLightAnimRad, ObjectRotDiff, epsilon, offset_theta);            
    }

    // Handle light animations
    if (LightAnimOn) {
        float LightRotDiff = 2*M_PI * float(elapsedMS) / LIGHT_ROT_TIME;
        LightRot = mod((LightRot + LightRotDiff), 2*M_PI);

        L1.X = offset_x(L1.X, L1.Z, ObjLightAnimRad, LightRotDiff, epsilon, offset_theta);
        L1.Z = offset_z(L1.X, L1.Z, ObjLightAnimRad, LightRotDiff, epsilon, offset_theta);
        LObj1.X = L1.X;
        LObj1.Z = L1.Z;
        
        L3.X = offset_x(L3.X, L3.Z, ObjLightAnimRad, LightRotDiff, epsilon, offset_theta);
        L3.Z = offset_z(L3.X, L3.Z, ObjLightAnimRad, LightRotDiff, epsilon, offset_theta);
        LObj3.X = L3.X;
        LObj3.Z = L3.Z;
    }
    
    // Handle vertex and fragment shader animations
    if (VertShaderAnimOn)
        ShaderVertTime = mod(ShaderVertTime+(float(elapsedMS)/VERT_ANIM_TIME), 1);
    if (FragShaderAnimOn)
        ShaderFragTime = mod(ShaderFragTime+(float(elapsedMS)/FRAG_ANIM_TIME), 1);

    // Handle bezier point animation
    if (BezierAnimOn) {
        // establish values
        float b_theta = 360. * sin(2*M_PI * float(ms % BEZ_TIME) / BEZ_TIME); // [0,360)
        // float b_theta = 360. * float(ms % BEZ_TIME) / BEZ_TIME; // [0,360)
        float b_phase_diff = 1.25; // how much faster each point will move than the last
        // float b_phase_diff = 0.25; // how much faster each point will move than the last
        float b_phase_base = 1; // the power of the first points diff
        
        for (struct beziercurve *curve = radial_curves; curve != &radial_curves[NUM_RAD_BC]; curve++) {
            RotateY(&curve->p0, (pow(b_phase_diff, 0)-b_phase_base)*b_theta, 0., curve->p0.y, 0.);
            RotateY(&curve->p1, (pow(b_phase_diff, 1)-b_phase_base)*b_theta, 0., curve->p1.y, 0.);
            RotateY(&curve->p2, (pow(b_phase_diff, 2)-b_phase_base)*b_theta, 0., curve->p2.y, 0.);
            RotateY(&curve->p3, (pow(b_phase_diff, 3)-b_phase_base)*b_theta, 0., curve->p3.y, 0.);
        }

        // // move BC0 points
        // RotateY(&BC0.p0, (pow(b_phase_diff, 0)-b_phase_base)*b_theta, 0., BC0.p0.y, 0.);
        // RotateY(&BC0.p1, (pow(b_phase_diff, 1)-b_phase_base)*b_theta, 0., BC0.p1.y, 0.);
        // RotateY(&BC0.p2, (pow(b_phase_diff, 2)-b_phase_base)*b_theta, 0., BC0.p2.y, 0.);
        // RotateY(&BC0.p3, (pow(b_phase_diff, 3)-b_phase_base)*b_theta, 0., BC0.p3.y, 0.);

        // // move BC1 points
        // RotateY(&BC1.p0, (pow(b_phase_diff, 0)-b_phase_base)*b_theta, 0., BC1.p0.y, 0.);
        // RotateY(&BC1.p1, (pow(b_phase_diff, 1)-b_phase_base)*b_theta, 0., BC1.p1.y, 0.);
        // RotateY(&BC1.p2, (pow(b_phase_diff, 2)-b_phase_base)*b_theta, 0., BC1.p2.y, 0.);
        // RotateY(&BC1.p3, (pow(b_phase_diff, 3)-b_phase_base)*b_theta, 0., BC1.p3.y, 0.);

        // // move BC2 points
        // RotateY(&BC2.p0, (pow(b_phase_diff, 0)-b_phase_base)*b_theta, 0., BC2.p0.y, 0.);
        // RotateY(&BC2.p1, (pow(b_phase_diff, 1)-b_phase_base)*b_theta, 0., BC2.p1.y, 0.);
        // RotateY(&BC2.p2, (pow(b_phase_diff, 2)-b_phase_base)*b_theta, 0., BC2.p2.y, 0.);
        // RotateY(&BC2.p3, (pow(b_phase_diff, 3)-b_phase_base)*b_theta, 0., BC2.p3.y, 0.);

        // // move BC3 points
        // RotateY(&BC3.p0, (pow(b_phase_diff, 0)-b_phase_base)*b_theta, 0., BC3.p0.y, 0.);
        // RotateY(&BC3.p1, (pow(b_phase_diff, 1)-b_phase_base)*b_theta, 0., BC3.p1.y, 0.);
        // RotateY(&BC3.p2, (pow(b_phase_diff, 2)-b_phase_base)*b_theta, 0., BC3.p2.y, 0.);
        // RotateY(&BC3.p3, (pow(b_phase_diff, 3)-b_phase_base)*b_theta, 0., BC3.p3.y, 0.);
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




    // // set the viewport to a square centered in the window:
    // GLsizei vx = glutGet(GLUT_WINDOW_WIDTH);
    // GLsizei vy = glutGet(GLUT_WINDOW_HEIGHT);
    // GLsizei v = vx < vy ? vx : vy; // minimum dimension
    // GLint xl = (vx - v) / 2;
    // GLint yb = (vy - v) / 2;
    // glViewport(xl, yb, v, v);

    // // set the viewing volume:
    // // remember that the Z clipping  values are actually
    // // given as DISTANCES IN FRONT OF THE EYE
    // // USE gluOrtho2D( ) IF YOU ARE DOING 2D !
    // glMatrixMode(GL_PROJECTION);
    // glLoadIdentity();

    int screenWidth = glutGet(GLUT_WINDOW_WIDTH);
    int screenHeight = glutGet(GLUT_WINDOW_HEIGHT);
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)screenWidth, (GLsizei)screenHeight);

    // set perspective viewing frustum
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (WhichProjection == ORTHO) {
        glOrtho(-3., 3., -3., 3., 0.1, 1000.);
    } else {
        gluPerspective(45.0f, (float)(screenWidth)/screenHeight, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip
        // gluPerspective(90., 1., 0.1, 1000.);
    }

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();




    // place the objects into the scene:
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // since we are using glScalef( ), be sure normals get unitized:
    glEnable(GL_NORMALIZE);




    // set the eye position, look-at position, and up-vector dending on view
    gluLookAt(0., 0., 5., 0., 0., 0., 0., 1., 0.);

    // uniformly rotate and scale the scene, but only if in outside view
    if (CurrView == OUTSIDE_VIEW) {
        glRotatef((GLfloat)Xrot, 1., 0., 0.);
        glRotatef((GLfloat)Yrot, 0., 1., 0.);
        // glRotatef((GLfloat)Zrot, 0., 0., 1.);

        if (Scale < MINSCALE)
            Scale = MINSCALE;
        glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
    }

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




    // setup and place the lights
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, MulArray3(0.2, White));
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);

    // enable and configure the individual lights
    if (L1_On) { EnableLight(L1.num, L1);
                 DrawObjectTransform(LObj1); }
    else glDisable(L1.num);

    if (L2_On) { EnableLight(L2.num, L2);
                 DrawObjectTransform(LObj2); }
    else glDisable(L2.num);

    if (L3_On) { EnableLight(L3.num, L3);
                 DrawObjectTransform(LObj3); }
    else glDisable(L3.num);

    if (L0_On) { EnableLight(L0.num, L0);
                 DrawObjectTransform(LObj0); }
    else glDisable(L0.num);
   
    

    // enable lighting
    glEnable(GL_LIGHTING);




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

    //
    // // Draw the main scene objects (helicopter and blades)
    // glShadeModel(GL_SMOOTH);
    // glCallList(HeliList);
    // DrawHelicopterBlades();
    //
    // // Apply front scene (var sphere) tfrms to a copy of curr matrix
    // glPushMatrix();
    //     // Apply front-specific translation transformation
    //     glTranslatef(-3., -2., 0.);
    //     // possibly draw the fixed front axes
    //     if (FrontAxesOn != 0) {
    //         glColor3fv(&Colors[WhichColor][0]);
    //         glCallList(AxesList);
    //     }
    //     // Apply front-specific rotation transformation
    //     glRotatef((GLfloat)FrontXrot, 1., 0., 0.);
    //     glRotatef((GLfloat)FrontYrot, 0., 1., 0.);
    //     glRotatef((GLfloat)FrontZrot, 0., 0., 1.);
    //     // Draw front scene objects (sphere)
    //     glShadeModel(GL_FLAT); // glShadeModel(GL_SMOOTH);
    //     DrawSphere();
    //     // possibly draw the rotated sphere front axes
    //     if (SphereAxesOn != 0) {
    //         glColor3fv(&Colors[WhichColor][0]);
    //         glCallList(AxesList);
    //     }
    // // Pop transformations
    // glPopMatrix();
    //
    // Draw object1
    // DrawObjectTransform(Obj1, GL_FLAT);
    // DrawObjectTransform(Obj2, GL_FLAT);


    // // Set uniform variables for shaders
    // char uVertTime[] = "uVertTime";
    // Pattern->SetUniformVariable(uVertTime, ShaderVertTime);
    // char uFragTime[] = "uFragTime";
    // Pattern->SetUniformVariable(uFragTime, ShaderFragTime);
    // char uNumChecks[] = "uNumChecks";
    // Pattern->SetUniformVariable(uNumChecks, ShaderNumChecks);
    // char uColorPercent[] = "uColorPercent";
    // float ShaderColorPercent = ShaderRainbowOn ? 1.:0.;
    // Pattern->SetUniformVariable(uColorPercent, ShaderColorPercent);

    // // Draw object2 and object3 using shaders
    // DrawObjectShaded(Obj2, Pattern, false);
    // DrawObjectShaded(Obj3, Pattern, false);


    // // Draw Object 1 very small
    // Obj1.Scale = 0.1;
    // DrawObjectTransform(Obj1, GL_FLAT);




    // // Set uniform variables for BC Shader
    // char uVertTime[] = "uVertTime";
    // BCShader->SetUniformVariable(uVertTime, ShaderVertTime);
    // char uFragTime[] = "uFragTime";
    // BCShader->SetUniformVariable(uFragTime, ShaderFragTime);
    // // char uVertAnimSel[] = "uVertAnimSel";
    // // BCShader->SetUniformVariable(uVertAnimSel, VertShaderAnimSel);
    // char uFragColorSel[] = "uFragColorSel";
    // BCShader->SetUniformVariable(uFragColorSel, FragShaderColorSel);

    // // Draw BC0
    // BCShader->Use();
    //     for (struct beziercurve *curve = radial_curves; curve != &radial_curves[NUM_RAD_BC]; curve++)
    //         DrawBezierCurve(*curve);
    //     for (struct beziercurve *curve = fixed_curves; curve != &fixed_curves[NUM_FIX_BC]; curve++)
    //         DrawBezierCurve(*curve);
    //     // DrawBezierCurve(BC0);
    //     // DrawBezierCurve(BC1);
    //     // DrawBezierCurve(BC2);
    //     // DrawBezierCurve(BC3);
    // BCShader->UnUse();

    
    
    

    // // set material
    // float ambient[] = {0.2f, 0.2f, 0.2f, 1};
    // float diffuse[] = {0.8f, 0.8f, 0.8f, 1};
    // float specular[] = {1.0f, 1.0f, 1.0f, 1};
    // float shininess = 128;
    // glMaterialfv(GL_FRONT, GL_AMBIENT, ambient);
    // glMaterialfv(GL_FRONT, GL_DIFFUSE, diffuse);
    // glMaterialfv(GL_FRONT, GL_SPECULAR, specular);
    // glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    // float lineColor[] = {0.3f, 0.3f, 0.3f, 1};
    // glShadeModel(GL_FLAT);

    
    // Draw icospheres w/ shaders
    DrawIcoSphere(sphere, vboId, iboId, IcoShader, true);
    DrawIcoSphere(sphere2, vboId2, iboId2, IcoShader, false);

    // draw icosphere
    // sphere.draw();
    // // draw icosphere using VBO and GLSL
    // int stride = sphere.getInterleavedStride();     // should be 32 bytes
    // glVertexAttribPointer(attribVertex,   3, GL_FLOAT, false, stride, (void*)0);
    // glVertexAttribPointer(attribNormal,   3, GL_FLOAT, false, stride, (void*)(sizeof(float)*3));
    // glVertexAttribPointer(attribTexCoord, 2, GL_FLOAT, false, stride, (void*)(sizeof(float)*6));
    // glDrawElements(GL_TRIANGLES,
    //                 sphere.getIndexCount(),
    //                 GL_UNSIGNED_INT,
    //                 (void*)0);




    // Disable lighting
    glDisable(GL_LIGHTING);




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
                "Ctrls: Space, t, T, B, F, R, Q, E, V, /, ?, ., 0-3, [shift]wasd",
                (int)SphereVertSlices, (int)SphereArcSlices);
        DoRasterString(5., 5., 0., sliceStr);
    }
    if (DrawSlicesText) {
        glColor3f(1., 1., 1.);
        sprintf(sliceStr, "Vertical = %i", (int)SphereVertSlices);
        DoRasterString(5., 15., 0., sliceStr);
        sprintf(sliceStr, "Arc = %i", (int)SphereArcSlices);
        DoRasterString(60., 15., 0., sliceStr);
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
    Shader->Use();
        // // Draw w/o shaders
        // int drawMode = 2;
        // if(drawMode == 2)
        //     sphere.draw();
        // else
        //     sphere.drawWithLines(lineColor);

        // Draw w/ shaders
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
        // glUniformMatrix4fv(uniformMatrixModelView, 1, false, &matrixModelView[0][0]);
        // glUniformMatrix4fv(uniformMatrixModelViewProjection, 1, false, &matrixModelViewProjection[0][0]);
        // glUniformMatrix4fv(uniformMatrixNormal, 1, false, &matrixNormal[0][0]);

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

// Draws a bezier curve using the control points
void DrawBezierCurve(struct beziercurve BC)
{
    // Use vao
    glBindVertexArray(vao);

    // Apply object material
    char uKa[] = "uKa";
    char uKd[] = "uKd";
    char uKs[] = "uKs";
    char uShininess[] = "uShininess";
    char uColor[] = "uColor";
    char uSpecularColor[] = "uSpecularColor";
    char uColorPercent[] = "uColorPercent";
    char uFragColorSel[] = "uFragColorSel";
    BCShader->SetUniformVariable(uKa, 0);
    // BCShader->SetUniformVariable(uKa, 1);
    BCShader->SetUniformVariable(uKd, 1);
    BCShader->SetUniformVariable(uKs, 1);
    BCShader->SetUniformVariable(uShininess, 1);
    BCShader->SetUniformVariable(uColor, BC.color.r, BC.color.g, BC.color.b);
    BCShader->SetUniformVariable(uSpecularColor, 1., 1., 1.);
    BCShader->SetUniformVariable(uColorPercent, BC.colorPercent);
    BCShader->SetUniformVariable(uFragColorSel, FragShaderColorSel);
    
    // Initialize datastructures
    const int num_t = BC.t;
    float p_buf[12*num_t];
    // float t_buf[num_t];
    float t_buf[2*num_t];
    // float buf[13*num_t];
    float buf[16*num_t];

    // P values to use for each vertex
    float p_vals[12] = { BC.p0.x, BC.p0.y, BC.p0.z,
                         BC.p1.x, BC.p1.y, BC.p1.z,
                         BC.p2.x, BC.p2.y, BC.p2.z,
                         BC.p3.x, BC.p3.y, BC.p3.z };
    // float p_vals[24] = { BC.p0.x, BC.p0.y, BC.p0.z,
    //                      BC.p1.x, BC.p1.y, BC.p1.z,
    //                      BC.p2.x, BC.p2.y, BC.p2.z,
    //                      BC.p3.x, BC.p3.y, BC.p3.z,
    //                      BC.p0.x, BC.p0.y, BC.p0.z,
    //                      BC.p1.x, BC.p1.y, BC.p1.z,
    //                      BC.p2.x, BC.p2.y, BC.p2.z,
    //                      BC.p3.x, BC.p3.y, BC.p3.z };

    float p_vals4[32] ={ BC.p0.x, BC.p0.y, BC.p0.z, 0,
                         BC.p1.x, BC.p1.y, BC.p1.z, 0,
                         BC.p2.x, BC.p2.y, BC.p2.z, 0,
                         BC.p3.x, BC.p3.y, BC.p3.z, 0,
                         BC.p0.x, BC.p0.y, BC.p0.z, 0,
                         BC.p1.x, BC.p1.y, BC.p1.z, 0,
                         BC.p2.x, BC.p2.y, BC.p2.z, 0,
                         BC.p3.x, BC.p3.y, BC.p3.z, 0};


    glm::vec3 p0;
    glm::vec3 p1;
    glm::vec3 p2;
    glm::vec3 p3;
    glm::vec3 pos;
    float t;
    float pos_buf[4*BC.t];

    p0 = glm::vec3(BC.p0.x, BC.p0.y, BC.p0.z);
    p1 = glm::vec3(BC.p1.x, BC.p1.y, BC.p1.z);
    p2 = glm::vec3(BC.p2.x, BC.p2.y, BC.p2.z);
    p3 = glm::vec3(BC.p3.x, BC.p3.y, BC.p3.z);

    // Fill buffers
    for (int i = 0; i < BC.t; i++) {
        // // Copy p_vals into p_buf for this point
        // std::copy(p_vals, &p_vals[12], &p_buf[12*i]);
        // std::copy(p_vals + 3 * i, &p_vals[12 + 3 * i], &p_buf[12 * i]);

        // // Set t_val for this point
        // t_buf[i] = float(i)/float(BC.t-1);

        // // Set t_val for this point
        // t_buf[2*i] = float(i)/float(BC.t-1);
        // t_buf[2*i+1] = 1-(float(i)/float(BC.t-1));



        // // Copy in p and t values for combined buf
        // std::copy(p_vals + 3*i, &p_vals[12 + 3*i], &buf[13*i]);
        // buf[13*(i+1)-1] = float(i)/float(BC.t-1);

        // // Copy in p and t values for combined buf
        // buf[13*(i)] = float(i)/float(BC.t-1);
        // std::copy(p_vals + 3*i, &p_vals[12 + 3*i], &buf[13*i+1]);
        // buf[13 * i + 2] = float(i) / float(BC.t - 1); // put i in the y-coord

        // // Copy in p and t values for combined buf
        // std::copy(p_vals + 3*i, &p_vals[12 + 3*i], &buf[15*i]);
        // buf[15*(i+1)-3] = float(i)/float(BC.t-1);
        // buf[15*(i+1)-2] = 0;
        // buf[15*(i+1)-1] = 0;

        // Calculate this t value
        t = float(i)/float(BC.t-1);

        // Copy in p and t values for combined buf (vec4's)
        std::copy(p_vals4, &p_vals4[16], &buf[16*i]); // reuse points
        for (int j = 0; j < 4; j++)
            buf[16*i + 4*(j+1) - 1] = t;

        pos = powf(1-t, 3)*p0 + 3*t*powf(1-t,2)*p1 + 3*powf(t,2)*(1-t)*p2 + powf(t,3)*p3;
        pos_buf[0+4*i] = pos.x, pos_buf[1+4*i] = pos.y, pos_buf[2+4*i] = pos.z, pos_buf[3+4*i] = t;

        // std::cout << "pos=" << pos.x << ", " << pos.y << ", " << pos.z << " " << "; t=" << t << "; " << std::endl;
    }
    // std::cout << std::endl;
    // glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);

    // Bind vertex array
    glBindVertexArray(vao);

    // Draw Curves!
    glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    glBufferData(GL_ARRAY_BUFFER, BC.t*4*sizeof(float), pos_buf, GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINE_STRIP, 0, BC.t);

    // Draw Points and Lines!
    if (BezierPointsOn || BezierLinesOn) {
        glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
        glBufferData(GL_ARRAY_BUFFER, NUM_BC_POINTS*4*sizeof(float), p_vals4, GL_DYNAMIC_DRAW);
        if (BezierPointsOn) {
            BCShader->SetUniformVariable(uFragColorSel, WHITE_COLOR);
            glDrawArrays(GL_POINTS, 0, NUM_BC_POINTS);
        }
        if (BezierLinesOn) {
            BCShader->SetUniformVariable(uFragColorSel, GRAY_COLOR);
            glDrawArrays(GL_LINE_STRIP, 0, NUM_BC_POINTS);
        }
    }



    // // Bind these to the respective vbo's
    // glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    // glBufferData(GL_ARRAY_BUFFER, BC.t*sizeof(float), t_buf, GL_DYNAMIC_DRAW);
    // glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    // glBufferData(GL_ARRAY_BUFFER, BC.t*12*sizeof(float), p_buf, GL_DYNAMIC_DRAW);

    // // Bind these to the combined vbo
    // glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    // glBufferData(GL_ARRAY_BUFFER, 13*BC.t*sizeof(float), buf, GL_DYNAMIC_DRAW);

    // // Bind these to the new combined vbo
    // glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    // glBufferData(GL_ARRAY_BUFFER, 16*BC.t*sizeof(float), wow, GL_DYNAMIC_DRAW);
    // // glBufferData(GL_ARRAY_BUFFER, 16*BC.t*sizeof(float), buf, GL_DYNAMIC_DRAW);

    // // Draw!
    // glDrawArrays(GL_LINE_STRIP, 0, BC.t);
    // // glDrawArrays(GL_TRIANGLES, 0, BC.t);

    // float t2_buf[3] = {0, 1, 2};
    // float t2_buf[6] = {0, 1, 1, 1, 2, 2};

    // glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    // // glBufferData(GL_ARRAY_BUFFER, 2*BC.t*sizeof(float), t_buf, GL_DYNAMIC_DRAW);
    // // glBufferData(GL_ARRAY_BUFFER, 1*BC.t*sizeof(float), t_buf, GL_DYNAMIC_DRAW);
    // glBufferData(GL_ARRAY_BUFFER, 2*BC.t*sizeof(float), t2_buf, GL_DYNAMIC_DRAW);

    // glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    // glBufferData(GL_ARRAY_BUFFER, 12*BC.t*sizeof(float), p_buf, GL_DYNAMIC_DRAW);
    
    // glDrawArrays(GL_LINE_STRIP, 0, BC.t);
}

// Reset's a bezier curve's positions
void ResetBezierCurve(struct beziercurve *BC)
{
    BC->p0.x = BC->p0.x0, BC->p0.y = BC->p0.y0, BC->p0.z = BC->p0.z0;
    BC->p1.x = BC->p1.x0, BC->p1.y = BC->p1.y0, BC->p1.z = BC->p1.z0;
    BC->p2.x = BC->p2.x0, BC->p2.y = BC->p2.y0, BC->p2.z = BC->p2.z0;
    BC->p3.x = BC->p3.x0, BC->p3.y = BC->p3.y0, BC->p3.z = BC->p3.z0;
}

// Rotate bezier point a number of degrees around a center point
void RotateY( bezierpoint *p, float deg, float xc, float yc, float zc )
{
    float rad = deg * (M_PI/180.f); // deg -> radians
    float x = p->x0 - xc;
    float y = p->y0 - yc;
    float z = p->z0 - zc;

    float xp =  x*cos(rad) + z*sin(rad);
    float yp =  y;
    float zp = -x*sin(rad) + z*cos(rad);

    p->x = xp + xc;
    p->y = yp + yc;
    p->z = zp + zc;
}

// Draws an object's list using all transformations and materials, using Shaders
void DrawObjectShaded(struct object Obj,
                      GLSLProgram *Shader,
                      bool WithAxes)
{
    // Draw Axes if requested
    if (WithAxes) {
        // Apply to copy of curr matrix
        glPushMatrix();
            glTranslatef(Obj.X, Obj.Y, Obj.Z);
            glScalef(Obj.Scale, Obj.Scale, Obj.Scale);
            glColor3fv(&Colors[WhichColor][0]);
            glCallList(AxesList);
        glPopMatrix();
    }

    // Use the Shader shader for this object
    Shader->Use();
        // Apply object material
        char uKa[] = "uKa";
        char uKd[] = "uKd";
        char uKs[] = "uKs";
        char uShininess[] = "uShininess";
        char uColor[] = "uColor";
        char uSpecularColor[] = "uSpecularColor";
        Shader->SetUniformVariable(uKa, Obj.ambient);
        Shader->SetUniformVariable(uKd, Obj.diffuse);
        Shader->SetUniformVariable(uKs, Obj.specular);
        Shader->SetUniformVariable(uShininess, Obj.shiny);
        Shader->SetUniformVariable(uColor, Obj.R, Obj.G, Obj.B);
        Shader->SetUniformVariable(uSpecularColor, 1., 1., 1.);

        // Apply object transformations to a copy of curr matrix
        glPushMatrix();
            // Scale, rotate, and translate object1
            glRotatef((GLfloat)Obj.Xr, 1., 0., 0.);
            glRotatef((GLfloat)Obj.Yr, 0., 1., 0.);
            glRotatef((GLfloat)Obj.Zr, 0., 0., 1.);
            glTranslatef(Obj.X, Obj.Y, Obj.Z);
            glScalef(Obj.Scale, Obj.Scale, Obj.Scale);
            // Draw object
            glCallList(Obj.List);
        // Pop transformations
        glPopMatrix();
    Shader->UnUse();
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
    // Depending on old or new blades
    if (NewBladesOn) {
        // Draw the top blade
        glPushMatrix();
            // Transform (low poly)
            glTranslatef(0., 0.5, 6.+2.);
            // Rotate for correct directions
            glRotatef(-90., 0., 1., 0.);
            // Move up to correct spot
            glTranslatef(0.004 * 248.341, 0.004 * 624.294, 0);
            // glTranslatef(0.004 * 249.341, 0.004 * 624.294, 0.00425 * 16.8352);
            // Scale to a good size
            glScalef(0.004,0.004,0.00425);
            // Apply animation rotation
            glRotatef(TopBladeRot, 0., 1., 0.);

            // // Apply translation up to top of heli
            // glTranslatef(0., 2.9, -1.5+8.5+2.);
            // // glTranslatef(0., 2.9, -1.5);
            // // Apply scaling up to radius 5 (from 1)
            // glScalef(5., 5., 5.);
            // // Apply animation rotation
            // glRotatef(TopBladeRot, 0., 1., 0.);
            // // Apply rotation abt X
            // glRotatef(90., 1., 0., 0.);
            // Draw the blade
            glCallList(NewTopBladeList);
        // Pop top blade transormations
        glPopMatrix();

        // Draw the tail blade
        glPushMatrix();
            // Transform (low poly)
            glTranslatef(0., 0.5, 6.+2.);
            // // Move up to correct spot
            glTranslatef(0., 0.004 * 509.54, 0.00425 * 2700.57);
            // glTranslatef(0.004 * 2700.57, 0.004 * 509.54, 0);
            // glTranslatef(0.004 * 2700.57, 0.004 * 509.54, 0.00425 * 13.0899);
            // Scale to a good size
            glScalef(0.004,0.004,0.00425);
            // Apply animation rotation
            glRotatef(TailBladeRot, 1., 0., 0.);
            // Rotate for correct directions
            glRotatef(-90., 0., 0., 1.);


            // // Transform (low poly)
            // glTranslatef(0., 0.5, 6.+2.);
            // glScalef(0.004,0.004,0.00425);
            // glRotatef(-90., 0., 1., 0.);
            // // Apply animation rotation
            // glRotatef(TailBladeRot, 1., 0., 0.);

            // // Apply translation up to top of heli
            // glTranslatef(0.5, 2.5, 9.+8.5+2.);
            // // glTranslatef(0.5, 2.5, 9.);
            // // Apply scaling up to radius 3 (from 1)
            // glScalef(3., 3., 3.);
            // // Apply animation rotation
            // glRotatef(TailBladeRot, 1., 0., 0.);
            // // Apply rotation abt Y
            // glRotatef(90., 0., 1., 0.);
            // Draw the blade
            glCallList(NewTailBladeList);
        // Pop top blade transormations
        glPopMatrix();
    }
    else {
        // Draw the top blade
        glPushMatrix();
            // Apply translation up to top of heli
            glTranslatef(0., 2.9, -1.5+8.5+2.);
            // glTranslatef(0., 2.9, -1.5);
            // Apply scaling up to radius 5 (from 1)
            glScalef(5., 5., 5.);
            // Apply animation rotation
            glRotatef(TopBladeRot, 0., 1., 0.);
            // Apply rotation abt X
            glRotatef(90., 1., 0., 0.);
            // Draw the blade
            glCallList(OldBladeList);
        // Pop top blade transormations
        glPopMatrix();

        // Draw the tail blade
        glPushMatrix();
            // Apply translation up to top of heli
            glTranslatef(0.5, 2.5, 9.+8.5+2.);
            // glTranslatef(0.5, 2.5, 9.);
            // Apply scaling up to radius 3 (from 1)
            glScalef(3., 3., 3.);
            // Apply animation rotation
            glRotatef(TailBladeRot, 1., 0., 0.);
            // Apply rotation abt Y
            glRotatef(90., 0., 1., 0.);
            // Draw the blade
            glCallList(OldBladeList);
        // Pop top blade transormations
        glPopMatrix();
    }
}

float GenerateNewPlanet() {
    float aveNoise;
    if (!THREE_DEE) {
        // Setup and generate 2D noise for planet
        // int baseSeed = 1000;  // no
        int baseSeed = time(NULL) % 10000;   // this one
        const int numLayers = 1;  // no
        // const int numLayers = 9;  // this one
        float baseFreq = .01;
        float baseAmp = 0.5;  // no
        // float baseAmp = 1.4;  // this one
        // float baseAmp = 0.5;
        const int dim = 128;
        // float noiseData[dim*dim] = {0};
        float noiseData[dim*dim];
        std::fill_n(noiseData, dim*dim, 1);
        int index;
        float total = 0;


        // FastNoiseLite noise;
        // noise.SetSeed(baseSeed);
        // noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
        // noise.SetFrequency(baseFreq);
        //         noise.SetFractalLacunarity(2.f);
        //         noise.SetFractalGain(0.5f);
        //         noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
        // index = 0;
        // for (int y = 0; y < dim; y++)
        //     for (int x = 0; x < dim; x++) {
        //         // noiseData[index++] *= (1-float(i)/float(numLayers))*baseAmp*(0.5 + ( noise.GetNoise((float)x, (float)y) + 1 ) / 2.);
        //         noiseData[index++] += baseAmp * noise.GetNoise((float)x, (float)y);
        //         // noiseData[index++] += baseAmp * noise.GetNoise((float)x, (float)y);
        //     }


        FastNoiseLite noise_generators[numLayers];
        for (int i = 0; i < numLayers; i++) {
            FastNoiseLite noise = noise_generators[i];
            noise.SetSeed(baseSeed + (i+1)*100);
            noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            noise.SetFrequency((1+i*1.5)*baseFreq);
                    noise.SetFractalLacunarity(2.f);
                    // noise.SetFractalLacunarity(5.f);
                    // noise.SetFractalGain(0.1f);
                    // noise.SetFractalGain(1.f);
                    noise.SetFractalGain(0.5f);
                    noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
            index = 0;
            for (int y = 0; y < dim; y++)
                for (int x = 0; x < dim; x++) {
                    noiseData[index++] *= (1-float(i)/float(numLayers))*baseAmp*(0.5 + ( noise.GetNoise((float)x, (float)y) + 1 ) / 2.);
                    // noiseData[index++] += baseAmp * noise.GetNoise((float)x, (float)y);
                    if (i == numLayers-1) total += noiseData[index-1];
                }
        }

        aveNoise = total / float(dim*dim);
        sphere2_rad = aveNoise;

        // Use noise as texture for icosphere
        // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId); // bind

        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // or GL_CLAMP
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // or GL_CLAMP
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // or GL_CLAMP
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // or GL_LINEAR
        // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // or GL_LINEAR
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // or GL_REPLACE

        glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, dim, dim, 0,
                     GL_RED, GL_FLOAT, noiseData);
                    //  GL_DEPTH_COMPONENT, GL_FLOAT, noiseData);

        // glActiveTexture(0);
        glBindTexture(GL_TEXTURE_2D, 0); // unbind
    }

    else if (THREE_DEE) {
        const int dim3D = 60;
        float noiseData3D[dim3D*dim3D*dim3D];

        // Setup and generate 3D noise for planet
        // int baseSeed = 1000;  // no
        int baseSeed = time(NULL) % 10000;   // this one
        // const int numLayers = 2;  // no
        // const int numLayers = 1;  // no
        const int numLayers = 9;  // this one
        float baseFreq = .02;
        // float baseFreq = .01;
        // float baseAmp = 0.5;  // no
        // float baseAmp = 1.;  // no
        float baseAmp = 0.9;  // no
        // float baseAmp = 1.2;  // this one
        // float baseAmp = 0.5;
        // float noiseData3D[dim3D*dim3D] = {0};
        // float noiseData3D[dim3D*dim3D];
        // std::fill_n(noiseData3D, dim3D*dim3D*dim3D, 1);
        std::fill_n(noiseData3D, dim3D*dim3D*dim3D, 0);
        int index;
        float total = 0;
        float count = 0;

        // lambda used to filter the noise
        auto filter = [](float x) { return pow(1.-abs(sin(x)), 2); }; // peaks
        // auto filter = [](float x) { return x; }; // identity

        // Params & setup
        FastNoiseLite noise;
        float amplitude = baseAmp;
        float frequency = baseFreq;
        float roughness = 3.;
        float persistence = 0.5;
        float min = 0;
        float strength = 0.5;
        
        // Apply each layer
        for (int i = 0; i < numLayers; i++) {
            // FastNoiseLite noise = noise_generators[i];
            // noise.SetSeed(baseSeed + (i+1)*100);
            noise.SetSeed(baseSeed);
            noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2);
            noise.SetFrequency(frequency);
            // noise.SetFrequency((1+i*1.5)*baseFreq);
                    // noise.SetFractalLacunarity(2.f);
                    noise.SetFractalLacunarity(5.f);
                    // noise.SetFractalGain(0.1f);
                    // noise.SetFractalGain(1.f);
                    noise.SetFractalGain(0.5f);
                    noise.SetFractalType(FastNoiseLite::FractalType::FractalType_FBm);
            index = 0;
            for (int y = 0; y < dim3D; y++) {
                for (int x = 0; x < dim3D; x++) {
                    for (int z = 0; z < dim3D; z++) {
                        index++;
                        float v = noise.GetNoise((float)x, (float)y, (float)z); // v = [-1,1]
                        v = (v+1)*0.5f*amplitude; // v = [0, amplitude]
                        v = std::max(0.f, v-min); // v = [max(0, min), amplitude]
                        v *= strength; // [max(0, min*strength), amplitude*strength]

                        noiseData3D[index-1] += v; // store

                        
                        // noiseData3D[index++] += filter( (1-float(i)/float(numLayers))*baseAmp*(0.5 + ( noise.GetNoise((float)x, (float)y, (float)z) + 1 ) / 2.) );
                        // noiseData3D[index++] += filter( (1-float(i)/float(numLayers))*baseAmp*(0.5 + ( noise.GetNoise((float)x, (float)y, (float)z) + 1 ) / 2.) );
                        // noiseData3D[index++] *= filter( (1-float(i)/float(numLayers))*baseAmp*(0.5 + ( noise.GetNoise((float)x, (float)y, (float)z) + 1 ) / 2.) );
                        // noiseData3D[index++] *= (1-float(i)/float(numLayers))*baseAmp*(0.5 + ( noise.GetNoise((float)x, (float)y, (float)z) + 1 ) / 2.);
                        // noiseData3D[index++] += baseAmp * noise.GetNoise((float)x, (float)y);
                        if (i == numLayers-1) {
                            // if (noiseData3D[index-1] < min_found)
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
        // aveNoise = total / float(dim3D*dim3D*dim3D);
        sphere2_rad = oceanThresh+(aveNoise);

                        // float bound = 0.1;
                        // // skip if not near edge
                        // float sqdist = float(x*x+y*y+z*z) / float(dim3D*dim3D);
                        // if (sqdist < 1-bound)
                        //     continue;
                        // if (   (y > bound*(float)dim3D && y < (1-bound)*(float)dim3D )
                        //     && (x > bound*(float)dim3D && x < (1-bound)*(float)dim3D )
                        //     && (z > bound*(float)dim3D && z < (1-bound)*(float)dim3D ) )
                        //     continue;

        glBindTexture(GL_TEXTURE_3D, texId); // bind

        // select modulate to mix texture with color for shading
        glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // Setup mon and mag filter and mipmap stuff
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP, GL_TRUE);
        // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // or GL_LINEAR
        // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        // Setup wrap parameter
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // or GL_CLAMP
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);

        // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_BASE_LEVEL, 0);
        // glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAX_LEVEL, 0);
        // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // Copy 3D noise texture data
        glTexImage3D(GL_TEXTURE_3D, 0, GL_RED, dim3D, dim3D, dim3D, 0,
                     GL_RED, GL_FLOAT, noiseData3D);

        // Generate mipmap for 3d texture
        glGenerateMipmap(GL_TEXTURE_3D);
        // gluBuild3DMipmaps(GL_TEXTURE_3D, GL_RED, dim3D, dim3D, dim3D, GL_RED, GL_FLOAT, noiseData3D);
        // gluBuild3DMipmaps(GL_TEXTURE_3D, GLU_INTENSITY, dim3D, dim3D, dim3D, GLU_RED, )

        glBindTexture(GL_TEXTURE_3D, 0); // unbind



        // int width = dim3D;
        // int height = dim3D;
        // GLenum type = GL_FLOAT;
        // GLenum format = GL_RED;
        
        // // gen texture ID
        // GLuint texture;
        // glGenTextures(1, &texture);

        // // set active texture and configure it
        // glBindTexture(GL_TEXTURE_3D, texture);

        // // select modulate to mix texture with color for shading
        // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

        // glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        // glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // glTexParameteri(GL_TEXTURE_3D, GL_GENERATE_MIPMAP, GL_TRUE);

        // // if wrap is true, the texture wraps over at the edges (repeat)
        // //       ... false, the texture ends at the edges (clamp)
        // glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
        // glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
        // glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
        // // glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, wrap ? GL_REPEAT : GL_CLAMP);
        // // glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, wrap ? GL_REPEAT : GL_CLAMP);
        // // glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, wrap ? GL_REPEAT : GL_CLAMP);
        // //glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        // //glTexParameterf(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        // // copy texture data
        // glTexImage3D(GL_TEXTURE_3D, 0, format, width, height, width, 0, format, type, noiseData3D);
        // // glTexImage3D(GL_TEXTURE_3D, 0, format, width, height, 0, format, type, data);
        // glGenerateMipmap(GL_TEXTURE_3D);
    }

    return aveNoise;
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
        delete[] Texture;
        // clean up VBOs
        if(vboSupported)
        {
            glDeleteBuffers(1, &vboId);
            glDeleteBuffers(1, &iboId);
            vboId = iboId = 0;
        }
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


    bool valid;

    // // Setup shader 1
    // // char vert[] = "shaders/lighting.vert";
    // // char frag[] = "shaders/lighting.frag";
    // char vert1[] = "shaders/p5.vert";
    // char frag1[] = "shaders/p5.frag";

    // Pattern = new GLSLProgram();
    // valid = Pattern->Create(vert1, frag1);

    // if (valid) {
    //     fprintf(stderr, "Shader created.\n");
    // } else {
    //     fprintf(stderr, "Shader cannot be created!\n");
    //     DoMainMenu(QUIT);
    // }

    // // Setup shader 2
    // const int max_name_size = 20;
    // char vert2[] = "shaders/bezier.vert";
    // char frag2[] = "shaders/bezier.frag";
    // int attrib_idx[2] = {0, 1};
    // const char *attrib_names[2] = {"vT", "vP"};

    // BCShader = new GLSLProgram(2, attrib_idx, attrib_names);
    // valid = BCShader->Create(vert2, frag2);

    // if (valid) {
    //     fprintf(stderr, "Shader created.\n");
    // } else {
    //     fprintf(stderr, "Shader cannot be created!\n");
    //     DoMainMenu(QUIT);
    // }


    // // Generate VAO and VBO's
    // // Create and enable vao
    // glGenVertexArrays(1, &vao);
    // glBindVertexArray(vao);
    // glEnableVertexAttribArray(vao);

    // // Create and enable both vbo's
    // glGenBuffers(5, vbo);

    // // // Set that vbo[0] has 2 t's per entry
    // // GLuint t_loc = glGetAttribLocation(BCShader->Program, "vT");
    // // glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    // // size_t t_stride = 2*sizeof(float);
    // // glVertexAttribPointer(t_loc, 2, GL_FLOAT, GL_FALSE, t_stride, 0);

    // // // Set that vbo[0] has 1 t per entry
    // // GLuint t_loc = glGetAttribLocation(BCShader->Program, "vT");
    // // glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    // // size_t t_stride = sizeof(float);
    // // glVertexAttribPointer(t_loc, 1, GL_FLOAT, GL_FALSE, t_stride, 0);
    
    // // // Set that vbo[1] has 4 X,Y,Z values
    // // GLuint p_loc = glGetAttribLocation(BCShader->Program, "vP");
    // // glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    // // // glVertexAttribPointer(p_loc, 4, GL_FLOAT_VEC3, GL_FALSE, 0, 0);
    // // glVertexAttribPointer(p_loc, 3, GL_FLOAT, GL_FALSE, 12*sizeof(float), 0);

    // // // Set that vbo[1] has 4 X,Y,Z values to go into vP0, vP1, vP2, vP3
    // // GLuint p0_loc = glGetAttribLocation(BCShader->Program, "vP0");
    // // GLuint p1_loc = glGetAttribLocation(BCShader->Program, "vP1");
    // // GLuint p2_loc = glGetAttribLocation(BCShader->Program, "vP2");
    // // GLuint p3_loc = glGetAttribLocation(BCShader->Program, "vP3");
    // // glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    // // size_t p_stride = 12*sizeof(float);
    // // glVertexAttribPointer(p0_loc, 3, GL_FLOAT, GL_FALSE, p_stride, (GLvoid *)(0*p_stride));
    // // glVertexAttribPointer(p1_loc, 3, GL_FLOAT, GL_FALSE, p_stride, (GLvoid *)(1*p_stride));
    // // glVertexAttribPointer(p2_loc, 3, GL_FLOAT, GL_FALSE, p_stride, (GLvoid *)(2*p_stride));
    // // glVertexAttribPointer(p3_loc, 3, GL_FLOAT, GL_FALSE, p_stride, (GLvoid *)(3*p_stride));


    // // // Set that vbo[2] has 4 X,Y,Z values to go into vP0, vP1, vP2, vP3 and 1 vT
    // // glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    // // GLuint p0_loc = glGetAttribLocation(BCShader->Program, "vP0");
    // // GLuint p1_loc = glGetAttribLocation(BCShader->Program, "vP1");
    // // GLuint p2_loc = glGetAttribLocation(BCShader->Program, "vP2");
    // // GLuint p3_loc = glGetAttribLocation(BCShader->Program, "vP3");
    // // size_t p_stride = 12*sizeof(float);
    // // GLuint t_loc = glGetAttribLocation(BCShader->Program, "vT");
    // // size_t t_stride = sizeof(float);
    // // // size_t tot_stride = p_stride;
    // // size_t tot_stride = p_stride + t_stride;
    // // glVertexAttribPointer(p0_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(0*tot_stride));
    // // glVertexAttribPointer(p1_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(1*tot_stride));
    // // glVertexAttribPointer(p2_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(2*tot_stride));
    // // glVertexAttribPointer(p3_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(3*tot_stride));
    // // glVertexAttribPointer(t_loc, 1, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(1*p_stride));
    // // // glVertexAttribPointer(t_loc, 1, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(1*tot_stride-1));


    // // // Set that vbo[2] has 4 X,Y,Z values to go into vP0, vP1, vP2, vP3 and 1 vT
    // // glBindBuffer(GL_ARRAY_BUFFER, vbo[2]);
    // // GLuint p0_loc = glGetAttribLocation(BCShader->Program, "vP0");
    // // GLuint p1_loc = glGetAttribLocation(BCShader->Program, "vP1");
    // // GLuint p2_loc = glGetAttribLocation(BCShader->Program, "vP2");
    // // GLuint p3_loc = glGetAttribLocation(BCShader->Program, "vP3");
    // // size_t p_stride = 12*sizeof(float);
    // // GLuint t_loc = glGetAttribLocation(BCShader->Program, "vT");
    // // size_t t_stride = sizeof(float);
    // // size_t tot_stride = p_stride + t_stride;
    // // glVertexAttribPointer(t_loc, 1, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(0));
    // // glVertexAttribPointer(p0_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(t_stride));
    // // glVertexAttribPointer(p1_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(1*tot_stride+t_stride));
    // // glVertexAttribPointer(p2_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(2*tot_stride+t_stride));
    // // glVertexAttribPointer(p3_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(3*tot_stride+t_stride));
    // // // glVertexAttribPointer(t_loc, 1, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(1*p_stride));

    // // // Set that vbo[3] has 4 X,Y,Z values to go into vP0, vP1, vP2, vP3, and vP4
    // // glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    // // GLuint p0_loc = glGetAttribLocation(BCShader->Program, "vP0");
    // // GLuint p1_loc = glGetAttribLocation(BCShader->Program, "vP1");
    // // GLuint p2_loc = glGetAttribLocation(BCShader->Program, "vP2");
    // // GLuint p3_loc = glGetAttribLocation(BCShader->Program, "vP3");
    // // GLuint p4_loc = glGetAttribLocation(BCShader->Program, "vP4");
    // // size_t p_stride = 15*sizeof(float);
    // // size_t tot_stride = p_stride;
    // // glVertexAttribPointer(p0_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(0));
    // // glVertexAttribPointer(p1_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(1*tot_stride));
    // // glVertexAttribPointer(p2_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(2*tot_stride));
    // // glVertexAttribPointer(p3_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(3*tot_stride));
    // // glVertexAttribPointer(p4_loc, 3, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(4*tot_stride));

    // // // Set that vbo[3] has 4 X,Y,Z values to go into vP0, vP1, vP2, vP3, and vP4
    // // glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    // // GLuint p0_loc = glGetAttribLocation(BCShader->Program, "vP0");
    // // GLuint p1_loc = glGetAttribLocation(BCShader->Program, "vP1");
    // // GLuint p2_loc = glGetAttribLocation(BCShader->Program, "vP2");
    // // GLuint p3_loc = glGetAttribLocation(BCShader->Program, "vP3");
    // // size_t p_stride = 16*sizeof(float);
    // // size_t tot_stride = p_stride;
    // // glVertexAttribPointer(p0_loc, 4, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(0));
    // // glVertexAttribPointer(p1_loc, 4, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(1*tot_stride));
    // // glVertexAttribPointer(p2_loc, 4, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(2*tot_stride));
    // // glVertexAttribPointer(p3_loc, 4, GL_FLOAT, GL_FALSE, tot_stride, (GLvoid *)(3*tot_stride));
    // // // glEnableVertexAttribArray(p0_loc);
    // // // glEnableVertexAtt/ribArray(p1_loc);
    // // // glEnableVertexAttribArray(p2_loc);
    // // // glEnableVertexAttribArray(p3_loc);

    // // Set that vbo[3] has 1 X,Y,Z,T for each vertex
    // glBindBuffer(GL_ARRAY_BUFFER, vbo[3]);
    // // glBindBuffer(GL_ARRAY_BUFFER, vbo[4]);
    // size_t stride = 4*sizeof(GLfloat);
    // GLuint p_loc = glGetAttribLocation(BCShader->Program, "vP");
    // glVertexAttribPointer(p_loc, 4, GL_FLOAT, GL_FALSE, stride, (GLvoid *)(0));


    // // Unbind
    // glBindVertexArray(0);


    // const int num = 1;
    // GLuint arrays[num];
    // glGenVertexArrays(num, arrays);

    // for (int i=0; i<num; i++) {
    //     glBindVertexArray(i);
    //     glEnableVertexAttribArray(i);
    // }



    // // Bind attribute positions
    // glBindAttribLocation(Pattern->Program, POS_INDEX, "vertex");
    // glBindAttribLocation(Pattern->Program, TEX_INDEX, "texcoord");


    // // Get texture files from disk
    // Texture = BmpToTexture(TEXTURE_PATH, &TEXTURE_WIDTH, &TEXTURE_HEIGHT);
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // // Create texture objects
    // glGenTextures(1, &Tex0);

    // // Bind for first time
    // glBindTexture(GL_TEXTURE_2D, Tex0);

    // // Setup Texture
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // or GL_CLAMP
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // or GL_CLAMP
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // or GL_LINEAR
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // or GL_REPLACE
    // // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // or GL_MODULATE

    // // Rotate texture coord space
    // // glMatrixMode(GL_TEXTURE);
    // // glScalef(0.1, 0.15, 1.);
    // // glTranslatef(4.5, 3.1, 0.);
    // // glRotatef(15., 0., 0., 1.);
    // // glMatrixMode(GL_MODELVIEW);

    // // Backup
    // // // glRotatef(10., 0., 0., 1.);
    // // // glTranslatef(0.5, 0., 0.);
    // // glScalef(1., 0.15, 1.);
    // // // glScalef(1., 0.3, 1.);
    // // // glTranslatef(0.5, -1., 0.);
    // // glScalef(0.1, 1., 1.);
    // // // glScalef(0.08, 1., 1.);
    // // // glScalef(0.125, 1., 1.);
    // // // glTranslatef(4, -0.5, 0.);
    // // glTranslatef(4., 0.75, 0.);
    // // glTranslatef(0.5, 2.35, 0.);
    // // glRotatef(15., 0., 0., 1.);
    // // glMatrixMode(GL_MODELVIEW);

    // // Apply Texture
    // glTexImage2D(GL_TEXTURE_2D, 0, 4,
    //              TEXTURE_WIDTH, TEXTURE_HEIGHT,
    //              0, GL_RGB, GL_UNSIGNED_BYTE, Texture);


    // // Set Bezier Curve default points
    // BC0.p0.x0 = -0.5,   BC0.p0.y0 = -1.5,   BC0.p0.z0 = 0;
    // BC0.p1.x0 = -0.5,   BC0.p1.y0 = -0.5,   BC0.p1.z0 = 0;
    // BC0.p2.x0 = -0.5,   BC0.p2.y0 = 0.5,    BC0.p2.z0 = 0;
    // BC0.p3.x0 = -0.5,   BC0.p3.y0 = 1.5,    BC0.p3.z0 = 0;
    // BC0.color = glm::vec3(1,1,0);
    // BC0.t = 10;

    // BC1.p0.x0 = 0.5,    BC1.p0.y0 = -1.5,   BC1.p0.z0 = 0;
    // BC1.p1.x0 = 0.5,    BC1.p1.y0 = -0.5,   BC1.p1.z0 = 0;
    // BC1.p2.x0 = 0.5,    BC1.p2.y0 = 0.5,    BC1.p2.z0 = 0;
    // BC1.p3.x0 = 0.5,    BC1.p3.y0 = 1.5,    BC1.p3.z0 = 0;
    // BC1.color = glm::vec3(0,1,1);
    // BC1.t = 10;

    // BC2.p0.x0 = 0,      BC2.p0.y0 = -1.5,   BC2.p0.z0 = -0.5;
    // BC2.p1.x0 = 0,      BC2.p1.y0 = -0.5,   BC2.p1.z0 = -0.5;
    // BC2.p2.x0 = 0,      BC2.p2.y0 = 0.5,    BC2.p2.z0 = -0.5;
    // BC2.p3.x0 = 0,      BC2.p3.y0 = 1.5,    BC2.p3.z0 = -0.5;
    // BC2.color = glm::vec3(1,0,1);
    // BC2.t = 10;

    // BC3.p0.x0 = 0,      BC3.p0.y0 = -1.5,   BC3.p0.z0 = 0.5;
    // BC3.p1.x0 = 0,      BC3.p1.y0 = -0.5,   BC3.p1.z0 = 0.5;
    // BC3.p2.x0 = 0,      BC3.p2.y0 = 0.5,    BC3.p2.z0 = 0.5;
    // BC3.p3.x0 = 0,      BC3.p3.y0 = 1.5,    BC3.p3.z0 = 0.5;
    // BC3.color = glm::vec3(0.2,1,0.2);
    // BC3.t = 10;


    // // Enable point sizes
    // glEnable(GL_PROGRAM_POINT_SIZE);


    // // Set Bezier Curve default points (radially around origin)
    // for (int i = 0; i < NUM_RAD_BC; i++) {
    //     float x = BC_RAD*cos(2*M_PI*float(i)/float(NUM_RAD_BC));
    //     float z = BC_RAD*sin(2*M_PI*float(i)/float(NUM_RAD_BC));
    //     float g = cos(2*M_PI*float(i)/float(NUM_RAD_BC)+(M_PI/4.));
    //     radial_curves[i].p0.x0 = x, radial_curves[i].p0.y0 = -1.5, radial_curves[i].p0.z0 = z;
    //     radial_curves[i].p1.x0 = x, radial_curves[i].p1.y0 = -0.5, radial_curves[i].p1.z0 = z;
    //     radial_curves[i].p2.x0 = x, radial_curves[i].p2.y0 = 0.5,  radial_curves[i].p2.z0 = z;
    //     radial_curves[i].p3.x0 = x, radial_curves[i].p3.y0 = 1.5,  radial_curves[i].p3.z0 = z;
    //     radial_curves[i].color = glm::vec3(x+.5,g+.5,z+.5);
    //     radial_curves[i].colorPercent = 1;
    //     // radial_curves[i].t = 10;
    // }
    // // And one stationary Bezier Curve at origin
    // fixed_curves[0].p0.x0 = 0, fixed_curves[0].p0.y0 = 2*-1.5, fixed_curves[0].p0.z0 = 0;
    // fixed_curves[0].p1.x0 = 0, fixed_curves[0].p1.y0 = 2*-0.5, fixed_curves[0].p1.z0 = 0;
    // fixed_curves[0].p2.x0 = 0, fixed_curves[0].p2.y0 = 2*0.5,  fixed_curves[0].p2.z0 = 0;
    // fixed_curves[0].p3.x0 = 0, fixed_curves[0].p3.y0 = 2*1.5,  fixed_curves[0].p3.z0 = 0;
    // fixed_curves[0].color = glm::vec3(0.5,0.5,0.5);
    // fixed_curves[0].colorPercent = 0.5;
    // // fixed_curves[0].t = 10;

    // // icosphere setup
    // // copy interleaved vertex data (V/N/T) to VBO
    // glGenBuffers(1, &vboId);
    // glBindBuffer(GL_ARRAY_BUFFER, vboId);           // for vertex data
    // glBufferData(GL_ARRAY_BUFFER,                   // target
    //              sphere.getInterleavedVertexSize(), // data size, # of bytes
    //              sphere.getInterleavedVertices(),   // ptr to vertex data
    //              GL_STATIC_DRAW);                   // usage

    // // copy index data to VBO
    // glGenBuffers(1, &iboId);
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId); // for index data
    // glBufferData(GL_ELEMENT_ARRAY_BUFFER,         // target
    //              sphere.getIndexSize(),           // data size, # of bytes
    //              sphere.getIndices(),             // ptr to index data
    //              GL_STATIC_DRAW);                 // usage


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
    valid = IcoShader->Create(icovert, icofrag);

    if (valid) {
        fprintf(stderr, "Ico shader created.\n");
    } else {
        fprintf(stderr, "Ico shader cannot be created!\n");
        DoMainMenu(QUIT);
    }

    attribVertexPosition = glGetAttribLocation(IcoShader->Program, "vertexPosition");
    attribVertexNormal   = glGetAttribLocation(IcoShader->Program, "vertexNormal");
    attribVertexTexCoord = glGetAttribLocation(IcoShader->Program, "vertexTexCoord");




    // Copy interleaved vertex data from icosphere
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

    // Copy interleaved vertex data from icosphere
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




    // // Get icosphere texture from bmp file
    // IcoTexture = BmpToTexture("obj/chopper.bmp", &texW, &texH);
    // // IcoTexture = BmpToTexture("obj/icosahedron_template1.bmp", &texW, &texH);
    // glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // // Create texture objects
    // glGenTextures(1, &texId);

    // // Bind for first time
    // glBindTexture(GL_TEXTURE_2D, texId);

    // // Setup Texture
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // or GL_CLAMP
    // // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP); // or GL_CLAMP
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); // or GL_LINEAR
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // or GL_MODULATE

    // // glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE); // or GL_REPLACE

    // // Apply Texture
    // glTexImage2D(GL_TEXTURE_2D, 0, 4, texW, texH,
    //              0, GL_RGB, GL_UNSIGNED_BYTE, IcoTexture);

    // // Unbind Texture
    // glBindTexture(GL_TEXTURE_2D, 0);



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
        // IcoShader->SetUniformVariable("map0", 0);
        // IcoShader->SetUniformVariable("map0", (int)texId);

        // IcoShader->SetUniformVariable("lightAmbient", 0.2f, 0.2f, 0.2f, 1);
        // IcoShader->SetUniformVariable("materialSpecular", 1.0f, 1.0f, 1.0f, 1);
        // IcoShader->SetUniformVariable("materialShininess", 0.25f);
        // IcoShader->SetUniformVariable("materialShininess", 1.5f);
        // IcoShader->SetUniformVariable("materialShininess", 0.5f);
        // IcoShader->SetUniformVariable("materialShininess", 128);
    IcoShader->UnUse();


}

// initialize the display lists that will not change:
// (a display list is a way to store opengl commands in
//  memory so that they can be played back efficiently at a later time
//  with a call to glCallList( )
void InitLists()
{
    // Create the helicopter list
    CreateHeliList();

    // Create the blade lists
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
    DrawOsuSphereLit(1.f, 8, 8, Obj1);
    glEndList();

    // Create the object2 list
    glutSetWindow(MainWindow);
    Obj2.List = glGenLists(1);
    glNewList(Obj2.List, GL_COMPILE);
    DrawOsuSphereLit(1.f, 1000, 1000, Obj2);
    glEndList();

    // Create the object3 list
    glutSetWindow(MainWindow);
    Obj3.List = glGenLists(1);
    glNewList(Obj3.List, GL_COMPILE);
    DrawOsuSphereLit(1.f, 1000, 1000, Obj3);
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

    // Create the lightobject3 list
    glutSetWindow(MainWindow);
    LObj0.List = glGenLists(1);
    glNewList(LObj0.List, GL_COMPILE);
    DrawOsuSphereUnlit(0.1, 10, 10, LObj0);
    glEndList();

}

// Recalculates the mesh for an Icosphere using vboId, iboId
void RecalcSphereMesh(Icosphere sphere, GLuint &vboId, GLuint &iboId) {
    // Copy interleaved vertex data from icosphere
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
}


void CreateHeliList() {
    // Set to the main window
    glutSetWindow(MainWindow);
    
    // Start heli list
    HeliList = glGenLists(1);
    glNewList(HeliList, GL_COMPILE);

    // Draw heli from OBJ
    DrawHeliFromObj();

    // // Draw heli
    // DrawHeli();

    // End list
    glEndList();
}

void DrawHeliFromObj() {
    // Push current matrix
    glPushMatrix();

    // Low Poly
    glTranslatef(0., 0.5, 6.+2.);
    glScalef(0.004,0.004,0.00425);
    glRotatef(-90., 0., 1., 0.);

    // Original
    // glTranslatef(0., -1., 9.+2.);
    // // glTranslatef(0., -1., 0.5);
    // glRotatef(97., 0., 1., 0.);
    // glRotatef(-15., 0., 0., 1.);

    // Use object material
    UseObjectMaterial(HeliObject);

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

    // Load heli Obj file
    // char *file = "obj/heli.obj";
    char *file = (char *)"obj/heli_customized.obj";
    LoadObjFileFromBlender(file);

    // Finish up
    glDisable(GL_TEXTURE_2D);
    glPopMatrix();
}

void DrawHeli() {
    // Create locals
    int i;
    struct hEdge *ep;
    struct hPoint *p0, *p1, *p2;
    struct hTri *tp;
    float p01[3], p02[3], n[3];

    // Push current matrix
    glPushMatrix();
    glTranslatef(0., -1., 9.+2.);
    // glTranslatef(0., -1., 0.5);
    glRotatef(97., 0., 1., 0.);
    glRotatef(-15., 0., 0., 1.);

    // Use object material
    UseObjectMaterial(HeliObject);

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

        // // fake "lighting" from above:
        // p01[0] = p1->x - p0->x;
        // p01[1] = p1->y - p0->y;
        // p01[2] = p1->z - p0->z;
        // p02[0] = p2->x - p0->x;
        // p02[1] = p2->y - p0->y;
        // p02[2] = p2->z - p0->z;
        // Cross(p01, p02, n);
        // Unit(n, n);
        // n[1] = fabs(n[1]);
        // n[1] += .25;
        // if (n[1] > 1.)
        //     n[1] = 1.;
        // glColor3f(0.75, 0.75 + (n[1] / 4.), 1.);
        // glColor3f(0., n[1], 0.5);


        glColor3f(0.75, 0.75, 0.75);

        // calculate normal
        auto reverse = [](float v[3]) { v[0]=-v[0]; v[1]=-v[1]; v[2]=-v[2]; };
        float n[3], p4[3], p5[3];
        struct hPoint *p6;
        p4[0] = p1->x-p0->x, p4[1] = p1->y-p0->y, p4[2] = p1->z-p0->z;
        p5[0] = p2->x-p0->x, p5[2] = p2->y-p0->y, p5[3] = p2->z-p0->z;
        Cross(p4, p5, n);
        // if (p0->z < p1->z) { reverse(n); }

        // if (p1->z < p0->z) { p6 = p0; p0 = p1; p1 = p6; }
        // p6 = p0; p0 = p1; p1 = p6;

        // Get whether the x y and z signs of p0 and n match
        p4[0] = p0->x * n[0], p4[1] = p0->y * n[1], p4[2] = p0->z * n[2];
        if (p4[0] < 0 || p4[1] < 0 || p4[2] < 0) {
            // If they don't match, swap p0 and p1 to switch the normal
            p6 = p0; p0 = p1; p1 = p6;
            reverse(n);
        } else {
            p4[0] = p1->x * n[0], p4[1] = p1->y * n[1], p4[2] = p1->z * n[2];
            if (p4[0] < 0 || p4[1] < 0 || p4[2] < 0) {
                // If they don't match, swap p0 and p1 to switch the normal
                p6 = p0; p0 = p1; p1 = p6;
                reverse(n);
            } else {
                p4[0] = p2->x * n[0], p4[1] = p2->y * n[1], p4[2] = p2->z * n[2];
                if (p4[0] < 0 || p4[1] < 0 || p4[2] < 0) {
                    // If they don't match, swap p0 and p1 to switch the normal
                    p6 = p0; p0 = p1; p1 = p6;
                    reverse(n);
                }
            }
        }

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
        glNormal3fv(n);
        glTexCoord2f(s_off+sine*p0_s, t_off+cosine*p0_t);
        glVertex3f(p0->x, p0->y, p0->z);

        glNormal3fv(n);
        glTexCoord2f(s_off+sine*p1_s, t_off+cosine*p1_t);
        glVertex3f(p1->x, p1->y, p1->z);

        glNormal3fv(n);
        glTexCoord2f(s_off+sine*p2_s, t_off+cosine*p2_t);
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
    char *wow;

    // Start old blade list
    OldBladeList = glGenLists(1);
    glNewList(OldBladeList, GL_COMPILE);

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

    // Create the new top and tail blades from Obj files
    NewTopBladeList = glGenLists(1);
    glNewList(NewTopBladeList, GL_COMPILE);
        // // Transform (low poly)
        // glTranslatef(0., 0.5, 6.+2.);
        // glScalef(0.004,0.004,0.00425);
        // glRotatef(-90., 0., 1., 0.);
        // Draw
        wow = (char *)"obj/heli_top_blade.obj";
        DrawObjectFromObj(wow, HeliObject);
    glEndList();

    NewTailBladeList = glGenLists(1);
    glNewList(NewTailBladeList, GL_COMPILE);
        // // Transform (low poly)
        // glTranslatef(0., 0.5, 6.+2.);
        // glScalef(0.004,0.004,0.00425);
        // glRotatef(-90., 0., 1., 0.);
        // Draw
    wow = (char *)"obj/heli_tail_blade.obj";
    DrawObjectFromObj(wow, HeliObject);
    glEndList();
}

void DrawObjectFromObj(char *obj_path, struct object material) {
    // Push current matrix
    glPushMatrix();

    // Use object material
    UseObjectMaterial(material);

    // Load Obj file
    LoadObjFileFromBlender(obj_path);

    // Finish up
    glPopMatrix();
}

void UpdateSphereList() {
    // // Create the sphere list
    // SphereList = glGenLists(1);
    // glNewList(SphereList, GL_COMPILE);

    // // Draw the sphere
    // DrawSphere();

    // // End sphere list
    // glEndList();
}

void DrawSphere() {
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

    if (WireframeMode) {
        // Enable wireframe
        glPolygonMode(GL_FRONT, GL_LINE);
        glPolygonMode(GL_BACK, GL_LINE);
    }

    // Use object1 properties (will replace color per-vertex)
    UseObjectMaterial(Obj1);

    // Push matrix to apply rotation 
    glPushMatrix();
        // Start compiling geometry
        glBegin(GL_TRIANGLE_STRIP);

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
                    // // Change blue hue by arc progression, red by vertical
                    // glColor3f(float(i) / int(SphereVertSlices),
                    //         0.f,
                    //         float(j) / int(SphereArcSlices));

                    // // Red and green gradiants
                    // glColor3f(float(i) / int(SphereVertSlices),
                    //           float(j) / int(SphereArcSlices),
                    //           0.f);

                    // Set material properties for these colors
                    float rgb[4] = {float(i)/int(SphereVertSlices), 0.f, float(j)/int(SphereArcSlices), 1.f};
                    glMaterialfv(GL_FRONT, GL_AMBIENT, rgb);
                    glMaterialfv(GL_FRONT, GL_DIFFUSE, rgb);

                    // Set normals
                    float prev_lat = M_PI/2. - M_PI * (float)(i-1) / (float)(SphereVertSlices-1);
                    float lat =      M_PI/2. - M_PI * (float)i     / (float)(SphereVertSlices-1);
                    // float prev_lat = -M_PI/2. + M_PI * (float)(i-1) / (float)(SphereVertSlices-1);
                    // float lat =      -M_PI/2. + M_PI * (float)i     / (float)(SphereVertSlices-1);
                    float lng = -M_PI + M_PI/2. + 2*M_PI * (float)j / (float)(SphereArcSlices-1);
                    // float lng = -M_PI + 2*M_PI * (float)j / (float)(SphereArcSlices-1);
                    float prev_x = 1*cosf( prev_lat ) * cosf( lng );
                    float prev_y = 1*sinf( lat );
                    float prev_z = 1*-cosf( prev_lat ) * sinf( lng );
                    float x = 1*cosf( lat ) * cosf( lng );
                    float y = 1*sinf( lat );
                    float z = 1*-cosf( lat ) * sinf( lng );

                    // Always do prev layer as first vertex in triangle chain
                    glNormal3f(prev_x, prev_y, prev_z);
                    glVertex3f(prev_layer[j].x,
                            prev_layer[j].y,
                            prev_layer[j].z);
                    
                    // Then do the curr layer
                    glNormal3f(x, y, z);
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
        // End sphere geometry
        glEnd();
    // Pop matrix after creating geometry
    glPopMatrix();

    if (WireframeMode) {
        // Disable wireframe
        glPolygonMode(GL_FRONT, GL_FILL);
        glPolygonMode(GL_BACK, GL_FILL);
    }
}


// the keyboard callback:
void Keyboard(unsigned char c, int x, int y)
{
    int sub;
    float subf;
    float wow;

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
        if (sub > 7) {sub = 7;}
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


    // case 'v':
    // case 'V':
    //     BezierPointsOn = !BezierPointsOn;
    //     break;

    // case 'b':
    // case 'B':
    //     BezierLinesOn = !BezierLinesOn;
    //     break;

    // case 'a':
    //     // Update Variable Sphere Number of Slices
    //     SphereArcSlices -= 1;
    //     if (SphereArcSlices < SPHERE_MIN_ARC) {SphereArcSlices = SPHERE_MIN_ARC;}
    //     if (SphereArcSlices > SPHERE_MAX_ARC) {SphereArcSlices = SPHERE_MAX_ARC;}
    //     // UpdateSphereList();
    //     break;
    // case 'A':
    //     // Update Variable Sphere Number of Slices
    //     SphereArcSlices -= 10;
    //     if (SphereArcSlices < SPHERE_MIN_ARC) {SphereArcSlices = SPHERE_MIN_ARC;}
    //     if (SphereArcSlices > SPHERE_MAX_ARC) {SphereArcSlices = SPHERE_MAX_ARC;}
    //     // UpdateSphereList();
    //     break;
    // case 'd':
    //     // Update Variable Sphere Number of Slices
    //     SphereArcSlices += 1;
    //     if (SphereArcSlices < SPHERE_MIN_ARC) {SphereArcSlices = SPHERE_MIN_ARC;}
    //     if (SphereArcSlices > SPHERE_MAX_ARC) {SphereArcSlices = SPHERE_MAX_ARC;}
    //     // UpdateSphereList();
    //     break;
    // case 'D':
    //     // Update Variable Sphere Number of Slices
    //     SphereArcSlices += 10;
    //     if (SphereArcSlices < SPHERE_MIN_ARC) {SphereArcSlices = SPHERE_MIN_ARC;}
    //     if (SphereArcSlices > SPHERE_MAX_ARC) {SphereArcSlices = SPHERE_MAX_ARC;}
    //     // UpdateSphereList();
    //     break;

    case 'r':
    case 'R':
        Reset();
        break;

    // case ' ':
    //     SphereRotateOn = !SphereRotateOn;
    //     SphereSliceAnimateOn = !SphereSliceAnimateOn;
    //     BladesRotateOn = !BladesRotateOn;
    //     ObjAnimOn = !ObjAnimOn;
    //     BezierAnimOn = !BezierAnimOn;
    //     break;

    // case 'f':
    // case 'F':
    //     BladesRotateOn = !BladesRotateOn;
    //     ObjAnimOn = !ObjAnimOn;
    //     LightAnimOn = !LightAnimOn;
    //     SphereRotateOn = !SphereRotateOn;
    //     SphereSliceAnimateOn = !SphereSliceAnimateOn;
    //     BezierAnimOn = !BezierAnimOn;
    //     break;

    // case 'l':
    // case 'L':
    //     LightAnimOn = !LightAnimOn;
    //     break;

    // case 'o':
    // case 'O':
    //     ObjAnimOn = !ObjAnimOn;
    //     break;

    // case 'z':
    //     VertShaderAnimOn = !VertShaderAnimOn;
    //     break;
    // case 'Z':
    //     VertShaderAnimSel += 1;
    //     if (VertShaderAnimSel > 1)
    //         VertShaderAnimSel = 0;
    //     break;

    // case 'x':
    // case 'X':
    //     FragShaderAnimOn = !FragShaderAnimOn;
    //     BezierAnimOn = !BezierAnimOn;
    //     break;
        
    // // case 'c':
    // //     ShaderRainbowOn = !ShaderRainbowOn;
    // //     break;
    // case 'c':
    // case 'C':
    //     FragShaderColorSel += 1;
    //     if (FragShaderColorSel > 3)
    //         FragShaderColorSel = 0;
    //     break;
    

    // case 'e':
    // case 'E':
    //     WireframeMode = !WireframeMode;
    //     UpdateSphereList();
    //     break;

    // case '.':
    //     OriginAxesOn = !OriginAxesOn;
    //     FrontAxesOn = !FrontAxesOn;
    //     SphereAxesOn = !SphereAxesOn;
    //     break;

    case '?':
        DrawHelpText = !DrawHelpText;
        break;

    // case '/':
    //     DrawSlicesText = !DrawSlicesText;
    //     break;

    // case 't':
    //     // Cycle through texture options
    //     TexSel += 1;
    //     if (TexSel >= sizeof(TEX_DISPLAY_OPTIONS) / sizeof(char*))
    //         TexSel = 0;
    //     break;

    // case 'T':
    //     // Cycle through texture distortion options
    //     TexDistortSel += 1;
    //     if (TexDistortSel >= sizeof(TEX_DISTORT_OPTIONS) / sizeof(char*))
    //         TexDistortSel = 0;
    //     break;

    // case '1':
    //     L1_On = !L1_On;
    //     break;
    // case '2':
    //     L2_On = !L2_On;
    //     break;
    // case '3':
    //     L3_On = !L3_On;
    //     break;
    // case '4':
    // case '0':
    //     L0_On = !L0_On;
    //     break;

    // case 'b':
    // case 'B':
    //     NewBladesOn = !NewBladesOn;
    //     break;


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
    int dy = y - Ymouse;
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
    // ActiveButton = 0;
    // OriginAxesOn = 1;
    // FrontAxesOn = 0;
    // SphereAxesOn = 0;
    // // OriginAxesScale = 2.5;
    // OriginAxesScale = 1;
    // DebugOn = 0;
    // DepthBufferOn = 1;
    // DepthFightingOn = 0;
    // DepthCueOn = 0;
    // Scale = 1.5;
    // ShadowsOn = 0;
    // WhichColor = WHITE;
    // WhichProjection = PERSP; 
    // // Xrot = -10.;
    // // Yrot = -20.;
    // Zrot = 0.;
    // Xrot = 0;
    // Yrot = 0;
    // TopBladeRot, TailBladeRot = 0., 0.;
    // BladesRotateOn = true;
    // SphereVertSlices = SPHERE_DEF_VERT;
    // SphereArcSlices = SPHERE_DEF_ARC;
    // SphereRotateOn = true;
    // SphereSliceAnimateOn = true;
    // SphereArcSliceAnimateDir = 1; 
    // SphereVertSliceAnimateDir = 1;
    // WireframeMode = false;
    // DrawHelpText = true;
    // DrawSlicesText = false;
    // CurrView = OUTSIDE_VIEW;
    // DistortTexOn = true;
    // TexDistortSel = 0;
    // TexSel = 2;
    // DistortTexTheta = 0.;
    // NewBladesOn = false;
    L1_On = false;
    L2_On = false;
    L3_On = false;

    L0_On = false;
    // ObjAnimOn = false;
    // LightAnimOn = false;

    // ShaderVertTime = 0., ShaderFragTime = 0.;
    // VertShaderAnimOn = true, FragShaderAnimOn = false;
    // ShaderNumChecks = 40;
    // ShaderRainbowOn = false;

    // reset objects  
    Obj1.X = 0., Obj1.Y = 2., Obj1.Z = 0.;
    Obj1.Xr = 0., Obj1.Yr = 0., Obj1.Zr = 0.;
    Obj1.R = 1., Obj1.G = 1., Obj1.B = 0.;
    Obj1.ambient = 1., Obj1.diffuse = 1., Obj1.specular = 0.4, Obj1.shiny = 0.0;
    Obj1.Scale = 1.;

    Obj2.X = -2., Obj2.Y = 0.5, Obj2.Z = 0.;
    Obj2.Xr = 0., Obj2.Yr = 0., Obj2.Zr = 0.;
    Obj2.R = 0.2, Obj2.G = 0.3, Obj2.B = 0.1;
    Obj2.ambient = 1., Obj2.diffuse = 1., Obj2.specular = 0.4, Obj2.shiny = 6.0;
    Obj2.Scale = 1.;

    Obj3.X = 2., Obj3.Y = 0.5, Obj3.Z = 0.;
    Obj3.Xr = 0., Obj3.Yr = 0., Obj3.Zr = 0.;
    Obj3.R = 0.2, Obj3.G = 0.1, Obj3.B = 0.3;
    Obj3.ambient = 1., Obj3.diffuse = 1., Obj3.specular = 0.4, Obj3.shiny = 5.0;
    Obj3.Scale = 1.;

    // reset lights
    L1.X = -2., L1.Y = 2.5, L1.Z = 0.;
    L1.R = 1., L1.G = 0.25, L1.B = 0.;
    L1.ambient = 0., L1.diffuse = 1, L1.specular = 1;
    L1.num = GL_LIGHT1;

    L2.X = 0., L2.Y = -3, L2.Z = 0.;
    L2.R = .8, L2.G = .8, L2.B = .8;
    L2.ambient = 0., L2.diffuse = .9, L2.specular = .9;
    L2.num = GL_LIGHT2;

    L3.X = 2., L3.Y = 2.5, L3.Z = 0.;
    L3.R = 0., L3.G = 0.25, L3.B = 1.;
    L3.ambient = 0., L3.diffuse = 1., L3.specular = 1.;
    L3.num = GL_LIGHT3;

    // L0.X = 2., L0.Y = 5., L0.Z = 2.;
    // L0.X = 0.5, L0.Y = 6., L0.Z = -2.;
    L0.X = 2., L0.Y = 3., L0.Z = -3.;
    L0.R = 1., L0.G = 1., L0.B = 1.;
    L0.ambient = 0., L0.diffuse = 0.5, L0.specular = 0.5;
    L0.DX = -L0.X, L0.DY = -L0.Y, L0.DZ = -L0.Z;
    L0.exp = 1., L0.spread = 12.;
    // L0.exp = 10., L0.spread = 20.;
    L0.num = GL_LIGHT0;

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

    LObj0.X = L0.X, LObj0.Y = L0.Y, LObj0.Z = L0.Z;
    LObj0.Xr = 0., LObj0.Yr = 0., LObj0.Zr = 0.;
    LObj0.R = L0.R, LObj0.G = L0.G, LObj0.B = L0.B;
    LObj0.Scale = 1.0;

    // // reset heli object color
    // HeliObject.R = 1.0, HeliObject.G = 1.0, HeliObject.B = 1.0;
    
    // // reset object and light animation angle
    // ObjectRot = 0;
    // LightRot = 0;

    // UpdateSphereList();

    // reset general
    Xrot = 0;
    Yrot = 0;
    Zrot = 0.;
    ActiveButton = 0;
    OriginAxesOn = 1;
    WhichProjection = ORTHO;
    DrawHelpText = false;
    DrawSlicesText = false;
    CurrView = OUTSIDE_VIEW;
    Scale = 1.;
    DepthBufferOn = 1;
    DepthFightingOn = 0;
    DepthCueOn = 0;

    // reset bezier curves
    BezierAnimOn = false;
    BezierPointsOn = false;
    BezierLinesOn = false;
    for (struct beziercurve *curve = radial_curves; curve != &radial_curves[NUM_RAD_BC]; curve++)
        ResetBezierCurve(curve);
    for (struct beziercurve *curve = fixed_curves; curve != &fixed_curves[NUM_FIX_BC]; curve++)
        ResetBezierCurve(curve);


    // reset icospheres
    sphere.setRadius(1.f);
    sphere.setSubdivision(5);
    sphere.setSmooth(false);
    if (redisplay) RecalcSphereMesh(sphere, vboId, iboId);
    sphere2.setRadius(sphere2_rad);
    sphere2.setSubdivision(5);
    sphere2.setSmooth(false);
    if (redisplay) RecalcSphereMesh(sphere2, vboId2, iboId2);

    fixedLighting = true;
    oceanThresh = oceanThreshDefVal;
    SceneRotateOn = false;



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
    // establish colors
    float frontRGB[4] = {Obj.R, Obj.G, Obj.B, 1.};
    float backRGB[4] = {Obj.BR, Obj.BG, Obj.BB, 1.};

    // back-facing attributes
    glMaterialfv(GL_BACK, GL_EMISSION, Array3(0., 0., 0.));
    glMaterialfv(GL_BACK, GL_SPECULAR, Array3(0., 0., 0.));
    glMaterialfv(GL_BACK, GL_AMBIENT, MulArray3(Obj.ambient, White));
    glMaterialfv(GL_BACK, GL_DIFFUSE, MulArray3(Obj.diffuse, backRGB));
    glMaterialfv(GL_BACK, GL_SPECULAR, MulArray3(Obj.specular, backRGB));
    glMaterialf(GL_BACK, GL_SHININESS, 0.5*Obj.shiny);

    // front-facing attributes
    glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
    glMaterialfv(GL_FRONT, GL_AMBIENT, MulArray3(Obj.ambient, frontRGB));
    glMaterialfv(GL_FRONT, GL_DIFFUSE, MulArray3(Obj.diffuse, frontRGB));
    glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(Obj.specular, White));
    glMaterialf(GL_FRONT, GL_SHININESS, Obj.shiny);
    // glMaterialfv(GL_FRONT, GL_EMISSION, Array3(0., 0., 0.));
    // glMaterialfv(GL_FRONT, GL_SPECULAR, MulArray3(0.4, White));
    // glMaterialfv(GL_FRONT, GL_AMBIENT, frontRGB);
    // glMaterialfv(GL_FRONT, GL_DIFFUSE, frontRGB);
    // glMaterialf(GL_FRONT, GL_SHININESS, 1.f);
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

void EnableLight(int lightNum, struct spotlight l)
{
    // Enable all the general light stuff
    EnableLight(lightNum, (struct light) l);

    // Specify spotlight-specific stuff
    glLightfv(lightNum, GL_SPOT_DIRECTION, Array3(l.DX, l.DY, l.DZ));
    glLightf(lightNum, GL_SPOT_EXPONENT, l.exp);
    glLightf(lightNum, GL_SPOT_CUTOFF, l.spread);
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
