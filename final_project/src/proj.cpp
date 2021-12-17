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
    elapsedMS = ms - msOfPriorAnimate;
    msOfPriorAnimate = ms;

    // Handle resizeTimer
    if (resizeTimer > 0) {
        resizeTimer -= elapsedMS; // if timer on, decrement by anim time

        if (resizeTimer <= 0) {
            // if decrement finished it off, trigger resize Stuff
            DepthTexture = setDepthTexture();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
            SetShaderUniforms(IcoShader, false);
            SetShaderUniforms(AtmShader, false);
            SetShaderUniforms(DepthShader, false);
        }
    }

    // Handle MouseProcessingTimer
    if (MouseProcessingTimer > 0) {
        MouseProcessingTimer -= elapsedMS; // if timer on, decrement by anim time
    }
    // Handle MouseTrapTimer
    if (MouseTrapTimer > 0) {
        MouseTrapTimer -= elapsedMS; // if timer on, decrement by anim time
    }

    // Handle scene rotation if enabled
    if (SceneRotateOn) {
        // Yrot += 180.f * ((float)elapsedMS) / SCENE_ROTATE_TIME; // rotate view
        TOYrot += SceneRotateSpeed * 180.f * ((float)elapsedMS) / SCENE_ROTATE_TIME; // rotate object
        // OYrot += SceneRotateSpeed * 180.f * ((float)elapsedMS) / SCENE_ROTATE_TIME; // rotate object
        // TYrot += SceneRotateSpeed * 180.f * ((float)elapsedMS) / SCENE_ROTATE_TIME; // rotate object
    }

    // If not at target Xrot, Yrot then interp toward them
    float eps = 0.1;
    float rat = 0.2;
    float dX = TXrot - Xrot;
    float dY = TYrot - Yrot;
    if (abs(dX) > eps || abs(dY) > eps) {
        Xrot += rat * dX;
        Yrot += rat * dY;
    }
    // if (abs(dX) > eps)
    //     Xrot += rat * dX;
    // if (abs(dY) > eps)
    //     Yrot += rat * dY;

    // If not at target OXrot, OYrot then interp toward them
    float Oeps = 0.1;
    float Orat = 0.1;
    float dOX = TOXrot - OXrot;
    float dOY = TOYrot - OYrot;
    if (abs(dOX) > Oeps || abs(dOY) > Oeps) {
        OXrot += Orat * dOX;
        OYrot += Orat * dOY;
    }
    // if (abs(dOX) > Oeps)
    //     OXrot += Orat * dOX;
    // if (abs(dOY) > Oeps)
    //     OYrot += Orat * dOY;

    // If not at target camTPitch, camTYaw then interp toward them
    float camEps = 0.05;
    float camRat = 0.4;
    float dPi = camTPitch - camPitch;
    float dYa = camTYaw - camYaw;
    if (abs(dPi) > camEps || abs(dYa) > camEps) {
        camPitch += camRat * dPi;
        camYaw += camRat * dYa;
    }
    // if (abs(dPi) > camEps)
    //     camPitch += camRat * dPi;
    // if (abs(dYa) > camEps)
    //     camYaw += camRat * dYa;
    UpdateCamDirs();

    // Movement
    glm::vec3 newTPos(camTX, camTY, camTZ);
    newTPos +=   (    (float)camFwdMove*camMFwd
                    + (float)camRghtMove*camMRght
                    + (float)camUpMove*camMUp )
               * (CAM_WASD_SPEED*elapsedMS/1000.f);
    camTX = newTPos.x, camTY = newTPos.y, camTZ = newTPos.z;
    float camMEps = 0.05;
    float camMRat = 0.2;
    float dXCam = camTX - camX;
    float dYCam = camTY - camY;
    float dZCam = camTZ - camZ;
    if (       abs(dXCam) > camMEps
            || abs(dYCam) > camMEps
            || abs(dZCam) > camMEps ) {
        camX += camMRat * dXCam;
        camY += camMRat * dYCam;
        camZ += camMRat * dZCam;
    }
    // if (abs(dXCam) > camMEps)
    //     camX += camMRat * dXCam;
    // if (abs(dYCam) > camMEps)
    //     camY += camMRat * dYCam;
    // if (abs(dZCam) > camMEps)
    //     camZ += camMRat * dZCam;

    // force a call to Display( ) next time it is convenient:
    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

void AnimateTimer(int wowsohelpful) {
    glutTimerFunc(ANIMATE_TIMER, AnimateTimer, 0);
    Animate();
}

// draw the complete scene:
void Display()
{
    // float wow[2];
    // glGetFloatv(GL_DEPTH_RANGE, wow);
    // std::cout << wow[0] << "; " << wow[1] << std::endl;

    // set which window we want to do the graphics into:
    glutSetWindow(MainWindow);

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

    // GLM style recording proj
    double P_d[16];
    glGetDoublev(GL_PROJECTION_MATRIX, P_d);
    glm::mat4 P = glm::make_mat4(P_d);

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // since we are using glScalef( ), be sure normals get unitized:
    glEnable(GL_NORMALIZE);

    // GLM style recording M, V, MV, WV
    glm::mat4 M(1);
    glm::mat4 V(1);
    glm::mat4 MV(1);
    glm::mat4 MVP(1);
    glm::mat4 W(1);
    glm::mat4 WV(1);

    // handle camera placement
    if (orbitCam) {
        // set the eye position, look-at position, and up-vector
        gluLookAt(eyeX/Scale, eyeY/Scale, eyeZ/Scale, 0., 0., 0., 0., 1., 0.);
        // gluLookAt(eyeX, eyeY, eyeZ, 0., 0., 0., 0., 1., 0.);
        V = glm::translate(V, -glm::vec3(eyeX, eyeY, eyeZ)); // at eyeXYZ, looking at 000, up 010
    } else {
        // do the camera transforamtion manually by camera pos and rot
        glRotatef(camPitch, camVRght.x, camVRght.y, camVRght.z);
        glRotatef(camYaw, camVUp.x, camVUp.y, camVUp.z);
        glTranslatef(-camX, -camY, -camZ);
        V = glm::rotate(V, (float)glm::radians(camPitch), camVRght);
        V = glm::rotate(V, (float)glm::radians(camYaw), camVUp);
        V = glm::translate(V, -glm::vec3(camX, camY, camZ));
    }

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDepthFunc(GL_LEQUAL);

    glPushMatrix();
        if (orbitCam) {
            // uniformly rotate and scale the scene
            glRotatef((GLfloat)Xrot, 1., 0., 0.);
            glRotatef((GLfloat)Yrot, 0., 1., 0.);
            if (Scale < MINSCALE)
                Scale = MINSCALE;
            // glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
        }

        // Use no color attachment
        // glEnable(GL_DEPTH_TEST);
        // glDepthMask(GL_TRUE);
        // glDepthFunc(GL_LEQUAL);
        // glDepthFunc(GL_ALWAYS);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // bind to custom framebuffer
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glClearDepth(1.);
        glClear(GL_DEPTH_BUFFER_BIT);

        // glEnable(GL_ALPHA_TEST);
        // glDisable(GL_ALPHA_TEST);

        // using depth fbo shader
        // DepthShader->Use();
            // Draw icospheres w/ shaders
            DrawIcoSphere(sphere, vboId, iboId, DepthShader, true, FBO);
            DrawIcoSphere(sphere2, vboId2, iboId2, DepthShader, false, FBO);
            // DrawIcoSphere(sphere, vboId, iboId, IcoShader, true, ICO);
            // DrawIcoSphere(sphere2, vboId2, iboId2, IcoShader, false, ICO);
        // DepthShader->UnUse();

        // unbind fbo
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glPopMatrix();


    // render framebuffer, erase background
    glDrawBuffer(GL_BACK);
    glClearColor(BACKCOLOR[0], BACKCOLOR[1], BACKCOLOR[2], BACKCOLOR[3]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glShadeModel(GL_FLAT);
    // glEnable(GL_DEPTH_TEST);
    // glDepthMask(GL_TRUE);
    // glDepthFunc(GL_LEQUAL);

    // draw skybox sphere w/ shader before scaling or rotating
    DrawSkyBox();

    if (orbitCam) {
        // uniformly rotate and scale the scene
        glRotatef((GLfloat)Xrot, 1., 0., 0.);
        glRotatef((GLfloat)Yrot, 0., 1., 0.);
        if (Scale < MINSCALE)
            Scale = MINSCALE;
        // glScalef((GLfloat)Scale, (GLfloat)Scale, (GLfloat)Scale);
        M = glm::rotate(M, (float)glm::radians(Xrot), glm::vec3(1,0,0));
        M = glm::rotate(M, (float)glm::radians(Yrot), glm::vec3(0,1,0));
        M = glm::scale(M, glm::vec3(Scale));
    }
    
    // Establish GLM combinations for icospheres before rotating model away from world
    WV = V*M;

    // Draw atmosphere icosphere w/ it's shader and same transformations
    DrawAtmSpherePostGLM(M, V*M, P*V*M, WV, P);

    // Rotate model away from world matrices
    M = glm::rotate(M, (float)glm::radians(OXrot), glm::vec3(1,0,0));
    M = glm::rotate(M, (float)glm::radians(OYrot), glm::vec3(0,1,0));
    MV = V*M;
    MVP = P*MV;

    // Draw icospheres w/ shaders
    DrawIcoSpherePostGLM(sphere, vboId, iboId, IcoShader, M, MV, MVP, WV, true, ICO);
    DrawIcoSpherePostGLM(sphere2, vboId2, iboId2, IcoShader, M, MV, MVP, WV, false, ICO);
    // DrawIcoSphere(sphere, vboId, iboId, IcoShader, true, ICO);
    // DrawIcoSphere(sphere2, vboId2, iboId2, IcoShader, false, ICO);


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
    // glScalef(.4, .4, .4);

    // Draw test quad
    bool drawTestQuad = false;
    if (drawTestQuad) {
        // DrawTestQuad(DepthTextureIdx, DepthTexture, true);
        DrawTestQuad(OpticalDepthTextureIdx, OpticalDepthTexture, false);
    }

    char sliceStr[50];
    if (DrawHelpText) {
        glColor3f(1., 1., 1.);
        sprintf(sliceStr,
                "Ctrls: Space, Enter, WASD, L, M, O, P, R, Q, ?");
        DoRasterString(5., 5., 0., sliceStr);
    }
    if (DrawFPS) {
        glColor3f(1., 1., 1.);
        sprintf(sliceStr,
                "%d", (int)(1000.f/(float)elapsedMS));
                // "%d", 1000/elapsedMS);
        DoRasterString(95., 95., 0., sliceStr);
        // DoRasterString(5., -5., 0., sliceStr);
    }


    // swap the double-buffered framebuffers:
    glutSwapBuffers();

    // be sure the graphics buffer has been sent:
    // note: be sure to use glFlush( ) here, not glFinish( ) !
    glFlush();
}

void DrawTestQuad(int TextureIdx, GLuint Texture, bool blend = false) {
    GLint OldBlendOn;
    GLint OldBlendAlpSrc;
    GLint OldBlendAlpDest;
    GLint OldBlendRGBSrc;
    GLint OldBlendRGBDest;
    if (blend) {
        glGetIntegerv(GL_BLEND, &OldBlendOn);
        glGetIntegerv(GL_BLEND_SRC_ALPHA, &OldBlendAlpSrc);
        glGetIntegerv(GL_BLEND_DST_ALPHA, &OldBlendAlpDest);
        glGetIntegerv(GL_BLEND_SRC_RGB, &OldBlendRGBSrc);
        glGetIntegerv(GL_BLEND_DST_RGB, &OldBlendRGBDest);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    const int n_v = 4;
    glm::vec2 v[n_v] = { glm::vec2(-1,-1), glm::vec2(-1,1),
                        glm::vec2(1,1), glm::vec2(1,-1)  };

    glActiveTexture(GL_TEXTURE0 + TextureIdx);
    glBindTexture(GL_TEXTURE_2D, Texture);
    TestQuadShader->SetUniformVariable("texture", (int)TextureIdx);
    TestQuadShader->Use();
        glEnableVertexAttribArray(testQuadAttribPosIdx);
        glBegin(GL_QUADS);
            for (int i = 0; i < n_v; i++)
                glVertex2f(v[i].x, v[i].y);
        glEnd();
    TestQuadShader->UnUse();
    glBindTexture(GL_TEXTURE_2D, 0);

    if (blend) {
        if (OldBlendOn) glEnable(GL_BLEND);
        else glDisable(GL_BLEND);
        glBlendFuncSeparate(OldBlendRGBSrc, OldBlendRGBDest, OldBlendAlpSrc, OldBlendAlpDest);
    }
}



void DrawIcoSphere(Icosphere sphere,
                   GLuint &vbo, GLuint &ibo,
                   GLSLProgram *Shader,
                   bool useNoise = true,
                   ShaderSel sel = ICO) {
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

        // Shader->SetUniformVariableMat4("matrixModel", &matrixModel[0][0]);
        Shader->SetUniformVariableMat4("matrixModelView", &matrixModelView[0][0]);
        Shader->SetUniformVariableMat4("matrixWorldView", &matrixWorldView[0][0]);
        Shader->SetUniformVariableMat4("matrixModelViewProjection", &matrixModelViewProjection[0][0]);
        Shader->SetUniformVariableMat4("matrixNormal", &matrixNormal[0][0]);
        Shader->SetUniformVariable("useNoise", useNoise);
        Shader->SetUniformVariable("fixedLighting", fixedLighting);

        // glm::mat4 matrixModelViewInv = glm::inverse(matrixModelView);
        // Shader->SetUniformVariable("matrixInvView", &matrixModelViewInv[0][0]);
                

        // bind VBOs
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        // activate attribs
        if (sel == ICO) {
            glEnableVertexAttribArray(attribVertexPosition);
            glEnableVertexAttribArray(attribVertexNormal);
            glEnableVertexAttribArray(attribVertexTexCoord);
        } else if (sel == ATM) {
            glEnableVertexAttribArray(attribAtmVertexPosition);
            glEnableVertexAttribArray(attribAtmVertexNormal);
            glEnableVertexAttribArray(attribAtmVertexTexCoord);
        } else if (sel == FBO ) {
            glEnableVertexAttribArray(attribFBOVertexPosition);
            glEnableVertexAttribArray(attribFBOVertexNormal);
            glEnableVertexAttribArray(attribFBOVertexTexCoord);
        } else {
            DoMainMenu(QUIT);
        }
            // std::cout << glGetError() << std::endl;

        // set attrib arrays using glVertexAttribPointer()
        int stride = sphere.getInterleavedStride();
        if (sel == ICO) {
            glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
            glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
            glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
        } else if (sel == ATM) {
            glVertexAttribPointer(attribAtmVertexPosition, 3, GL_FLOAT, false, stride, 0);
            glVertexAttribPointer(attribAtmVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
            glVertexAttribPointer(attribAtmVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
        } else if (sel == FBO ) {
            glVertexAttribPointer(attribFBOVertexPosition, 3, GL_FLOAT, false, stride, 0);
            glVertexAttribPointer(attribFBOVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
            glVertexAttribPointer(attribFBOVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
        } else {
            DoMainMenu(QUIT);
        }
        
        // Bind noise texture
        glActiveTexture(GL_TEXTURE0 + NoiseTextureIdx);
        glBindTexture(GL_TEXTURE_3D, texId);
        Shader->SetUniformVariable("useSSAO", false);

        // Use Identity so that the uniforms are used in-shader and the positions are unit
        glLoadIdentity();
        
        // Draw the sphere!
        glDrawElements(GL_TRIANGLES,            // primitive type
                       sphere.getIndexCount(),  // # of indices
                       GL_UNSIGNED_INT,         // data type
                       (void*)0);               // ptr to indices

        glBindTexture(GL_TEXTURE_3D, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (sel == ICO) {
            glDisableVertexAttribArray(attribVertexPosition);
            glDisableVertexAttribArray(attribVertexNormal);
            glDisableVertexAttribArray(attribVertexTexCoord);
        } else if (sel == ATM) {
            glDisableVertexAttribArray(attribAtmVertexPosition);
            glDisableVertexAttribArray(attribAtmVertexNormal);
            glDisableVertexAttribArray(attribAtmVertexTexCoord);
        } else if (sel == FBO ) {
            glDisableVertexAttribArray(attribFBOVertexPosition);
            glDisableVertexAttribArray(attribFBOVertexNormal);
            glDisableVertexAttribArray(attribFBOVertexTexCoord);
        } else {
            DoMainMenu(QUIT);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Shader->UnUse();

    glPopMatrix();
}

void DrawIcoSpherePreGLM(Icosphere sphere,
                      GLuint &vbo, GLuint &ibo,
                      GLSLProgram *Shader,
                      glm::mat4 worldMat,
                      glm::mat4 modelMat,
                      glm::mat4 viewMat,
                      glm::mat4 projMat,
                      bool useNoise = true,
                      ShaderSel sel = ICO) {
    // Use copies of matrices
    glm::mat4 localWorldMat(worldMat);
    glm::mat4 localModelMat(modelMat);
    glm::mat4 localViewMat(viewMat);
    glm::mat4 localProjMat(projMat);

    // Draw w/ shaders
    Shader->Use();
        // set matrix uniforms every frame
        glm::mat4 localWorldViewMat(localViewMat * localWorldMat);
        glm::mat4 localModelViewMat(localViewMat * localModelMat);
        glm::mat4 localModelViewProj(localProjMat * localModelViewMat);
        glm::mat4 localNormalMat = localModelViewMat;
        localNormalMat[3] = glm::vec4(0,0,0,1);

        // Set calculated matrix uniforms
        Shader->SetUniformVariableMat4("matrixModel", &localModelMat[0][0]);
        Shader->SetUniformVariableMat4("matrixModelView", &localModelViewMat[0][0]);
        Shader->SetUniformVariableMat4("localWorldViewMat", &localWorldViewMat[0][0]);
        Shader->SetUniformVariableMat4("matrixModelViewProjection", &localModelViewProj[0][0]);
        Shader->SetUniformVariableMat4("matrixNormal", &localNormalMat[0][0]);
        Shader->SetUniformVariable("useNoise", useNoise);
        Shader->SetUniformVariable("fixedLighting", fixedLighting);

        // bind VBOs
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        // activate attribs
        if (sel == ICO) {
            glEnableVertexAttribArray(attribVertexPosition);
            glEnableVertexAttribArray(attribVertexNormal);
            glEnableVertexAttribArray(attribVertexTexCoord);
        } else if (sel == ATM) {
            glEnableVertexAttribArray(attribAtmVertexPosition);
            glEnableVertexAttribArray(attribAtmVertexNormal);
            glEnableVertexAttribArray(attribAtmVertexTexCoord);
        } else if (sel == FBO ) {
            glEnableVertexAttribArray(attribFBOVertexPosition);
            glEnableVertexAttribArray(attribFBOVertexNormal);
            glEnableVertexAttribArray(attribFBOVertexTexCoord);
        } else {
            DoMainMenu(QUIT);
        }
            // std::cout << glGetError() << std::endl;

        // set attrib arrays using glVertexAttribPointer()
        int stride = sphere.getInterleavedStride();
        if (sel == ICO) {
            glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
            glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
            glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
        } else if (sel == ATM) {
            glVertexAttribPointer(attribAtmVertexPosition, 3, GL_FLOAT, false, stride, 0);
            glVertexAttribPointer(attribAtmVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
            glVertexAttribPointer(attribAtmVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
        } else if (sel == FBO ) {
            glVertexAttribPointer(attribFBOVertexPosition, 3, GL_FLOAT, false, stride, 0);
            glVertexAttribPointer(attribFBOVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
            glVertexAttribPointer(attribFBOVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
        } else {
            DoMainMenu(QUIT);
        }
        
        // Bind noise texture
        glActiveTexture(GL_TEXTURE0 + NoiseTextureIdx);
        glBindTexture(GL_TEXTURE_3D, texId);

        // Bind depth texture
        glActiveTexture(GL_TEXTURE0 + DepthTextureIdx);
        glBindTexture(GL_TEXTURE_2D, DepthTexture);
        Shader->SetUniformVariable("depthTex", (int)(DepthTextureIdx));
        Shader->SetUniformVariable("useSSAO", useSSAO);
        
        // Draw the sphere!
        glDrawElements(GL_TRIANGLES,            // primitive type
                       sphere.getIndexCount(),  // # of indices
                       GL_UNSIGNED_INT,         // data type
                       (void*)0);               // ptr to indices

        glBindTexture(GL_TEXTURE_3D, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (sel == ICO) {
            glDisableVertexAttribArray(attribVertexPosition);
            glDisableVertexAttribArray(attribVertexNormal);
            glDisableVertexAttribArray(attribVertexTexCoord);
        } else if (sel == ATM) {
            glDisableVertexAttribArray(attribAtmVertexPosition);
            glDisableVertexAttribArray(attribAtmVertexNormal);
            glDisableVertexAttribArray(attribAtmVertexTexCoord);
        } else if (sel == FBO ) {
            glDisableVertexAttribArray(attribFBOVertexPosition);
            glDisableVertexAttribArray(attribFBOVertexNormal);
            glDisableVertexAttribArray(attribFBOVertexTexCoord);
        } else {
            DoMainMenu(QUIT);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Shader->UnUse();
}

void DrawIcoSpherePostGLM(Icosphere sphere,
                          GLuint &vbo, GLuint &ibo,
                          GLSLProgram *Shader,
                          glm::mat4 M,
                          glm::mat4 MV,
                          glm::mat4 MVP,
                          glm::mat4 WV,
                          bool useNoise = true,
                          ShaderSel sel = ICO) {
    // Use copies of matrices
    glm::mat4 lM(M);
    glm::mat4 lMV(MV);
    glm::mat4 lMVP(MVP);
    glm::mat4 lWV(WV);

    // Use identity instead of fixed-pipeline stuff
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    // Draw w/ shaders
    Shader->Use();
        // set matrix uniforms every frame
        glm::mat4 lN = lMV;
        lN[3] = glm::vec4(0,0,0,1);

        // Set calculated matrix uniforms
        Shader->SetUniformVariableMat4("matrixModel", &lM[0][0]);
        Shader->SetUniformVariableMat4("matrixModelView", &lMV[0][0]);
        Shader->SetUniformVariableMat4("matrixWorldView", &lWV[0][0]);
        Shader->SetUniformVariableMat4("matrixModelViewProjection", &lMVP[0][0]);
        Shader->SetUniformVariableMat4("matrixNormal", &lN[0][0]);
        Shader->SetUniformVariable("useNoise", useNoise);
        Shader->SetUniformVariable("fixedLighting", fixedLighting);

        // bind VBOs
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);

        // activate attribs
        if (sel == ICO) {
            glEnableVertexAttribArray(attribVertexPosition);
            glEnableVertexAttribArray(attribVertexNormal);
            glEnableVertexAttribArray(attribVertexTexCoord);
        } else if (sel == ATM) {
            glEnableVertexAttribArray(attribAtmVertexPosition);
            glEnableVertexAttribArray(attribAtmVertexNormal);
            glEnableVertexAttribArray(attribAtmVertexTexCoord);
        } else if (sel == FBO ) {
            glEnableVertexAttribArray(attribFBOVertexPosition);
            glEnableVertexAttribArray(attribFBOVertexNormal);
            glEnableVertexAttribArray(attribFBOVertexTexCoord);
        } else {
            DoMainMenu(QUIT);
        }

        // set attrib arrays using glVertexAttribPointer()
        int stride = sphere.getInterleavedStride();
        if (sel == ICO) {
            glVertexAttribPointer(attribVertexPosition, 3, GL_FLOAT, false, stride, 0);
            glVertexAttribPointer(attribVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
            glVertexAttribPointer(attribVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
        } else if (sel == ATM) {
            glVertexAttribPointer(attribAtmVertexPosition, 3, GL_FLOAT, false, stride, 0);
            glVertexAttribPointer(attribAtmVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
            glVertexAttribPointer(attribAtmVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
        } else if (sel == FBO ) {
            glVertexAttribPointer(attribFBOVertexPosition, 3, GL_FLOAT, false, stride, 0);
            glVertexAttribPointer(attribFBOVertexNormal, 3, GL_FLOAT, false, stride, (void*)(3 * sizeof(float)));
            glVertexAttribPointer(attribFBOVertexTexCoord, 2, GL_FLOAT, false, stride, (void*)(6 * sizeof(float)));
        } else {
            DoMainMenu(QUIT);
        }
        
        // Bind noise texture
        glActiveTexture(GL_TEXTURE0 + NoiseTextureIdx);
        glBindTexture(GL_TEXTURE_3D, texId);

        // Bind depth texture
        glActiveTexture(GL_TEXTURE0 + DepthTextureIdx);
        glBindTexture(GL_TEXTURE_2D, DepthTexture);
        Shader->SetUniformVariable("depthTex", (int)(DepthTextureIdx));
        Shader->SetUniformVariable("useSSAO", useSSAO);

        // Draw the sphere!
        glDrawElements(GL_TRIANGLES,            // primitive type
                       sphere.getIndexCount(),  // # of indices
                       GL_UNSIGNED_INT,         // data type
                       (void*)0);               // ptr to indices

        glBindTexture(GL_TEXTURE_3D, 0);
        glBindTexture(GL_TEXTURE_2D, 0);

        if (sel == ICO) {
            glDisableVertexAttribArray(attribVertexPosition);
            glDisableVertexAttribArray(attribVertexNormal);
            glDisableVertexAttribArray(attribVertexTexCoord);
        } else if (sel == ATM) {
            glDisableVertexAttribArray(attribAtmVertexPosition);
            glDisableVertexAttribArray(attribAtmVertexNormal);
            glDisableVertexAttribArray(attribAtmVertexTexCoord);
        } else if (sel == FBO ) {
            glDisableVertexAttribArray(attribFBOVertexPosition);
            glDisableVertexAttribArray(attribFBOVertexNormal);
            glDisableVertexAttribArray(attribFBOVertexTexCoord);
        } else {
            DoMainMenu(QUIT);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    Shader->UnUse();

    // Re-establish fixed pipeline stuff
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
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
    // float min = 0.4;
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
    // sphere3_rad = oceanThresh + (aveNoise)+1;
    // sphere3_rad = 1.2 * (1+aveNoise);
    // sphere3_rad = 1.7 * (1+maxNoise);
    // sphere3_rad = 1.2 * (1+maxNoise);
    sphere3_rad = 1.3 * (1+maxNoise);
    // sphere3_rad = 1.3 * (1+maxNoise);
    // sphere3_rad = 1.5 * (1+maxNoise);
    // sphere3_rad = 1.4 * (1+maxNoise);
    // sphere3_rad = 1.05 * (1+maxNoise);

    // std::cout << minNoise << " " << maxNoise << " " << aveNoise << " " << 0.99 * (maxNoise - minNoise) + minNoise << std::endl;

    // Update texture in texId with this new data
    glActiveTexture(GL_TEXTURE0 + NoiseTextureIdx);
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
    GLint OldCullFaceOn;
    glGetIntegerv(GL_CULL_FACE, &OldCullFaceOn);
    GLint OldDepthFuncMode;
    glGetIntegerv(GL_DEPTH_FUNC, &OldDepthFuncMode);
    GLboolean OldDepthMask;
    glGetBooleanv(GL_DEPTH_WRITEMASK, &OldDepthMask);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_FRONT);
    glDepthFunc(GL_LEQUAL);
    glDepthMask(GL_FALSE);

    SkyShader->Use();
        // Translate origin back to eye
        if (orbitCam) {
            glTranslatef(eyeX/Scale, eyeY/Scale, eyeZ/Scale);
            // glTranslatef(eyeX, eyeY, eyeZ);
        } else {
            glTranslatef(camX, camY, camZ);
        }
        

        if (orbitCam)
        {
            const float skyScale = 5;
            glScalef(skyScale, skyScale, skyScale);
            glRotatef((GLfloat)Xrot, 1., 0., 0.);
            glRotatef((GLfloat)Yrot, 0., 1., 0.);
            if (Scale < MINSCALE)
                Scale = MINSCALE;
        }

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
        glBindBuffer(GL_ARRAY_BUFFER, vboIdSky);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIdSky);

        // activate attrib
        glEnableVertexAttribArray(attribPosition);

        // set attrib arrays using glVertexAttribPointer()
        int stride = skySphere.getInterleavedStride();
        glVertexAttribPointer(attribPosition, 3, GL_FLOAT, false, stride, 0);

        // bind texture
        skyTexArr[CurrSky]->Bind(GL_TEXTURE0 + SkyTextureIdx);
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
    if (OldCullFaceOn) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);

    glPopMatrix();
}


void DrawAtmSphere()
{
    // Use identity instead of fixed-pipeline stuff
    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    GLint OldCullFaceOn;
    glGetIntegerv(GL_CULL_FACE, &OldCullFaceOn);
    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldBlendOn;
    glGetIntegerv(GL_BLEND, &OldBlendOn);
    GLint OldBlendAlpSrc;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &OldBlendAlpSrc);
    GLint OldBlendAlpDest;
    glGetIntegerv(GL_BLEND_DST_ALPHA, &OldBlendAlpDest);
    GLint OldBlendRGBSrc;
    glGetIntegerv(GL_BLEND_SRC_RGB, &OldBlendRGBSrc);
    GLint OldBlendRGBDest;
    glGetIntegerv(GL_BLEND_DST_RGB, &OldBlendRGBDest);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE); // with manual culling in fragment shader, disable here
    // glDisable(GL_DEPTH_TEST); // disable depth if using back faces (render back of atm)


    glActiveTexture(GL_TEXTURE0 + DepthTextureIdx);
    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    AtmShader->SetUniformVariable("depthTex", (int)(DepthTextureIdx));
    // AtmShader->SetUniformVariable("depthTex", (int)(GL_TEXTURE0+DepthTextureIdx));
    float w = glutGet(GLUT_WINDOW_WIDTH);
    float h = glutGet(GLUT_WINDOW_HEIGHT);
    // float w = glutGet(GLUT_SCREEN_WIDTH);
    // float h = glutGet(GLUT_SCREEN_HEIGHT);
    // AtmShader->SetUniformVariable("screenWidth", (float)glutGet(GLUT_WINDOW_WIDTH));
    // AtmShader->SetUniformVariable("screenHeight", (float)glutGet(GLUT_WINDOW_HEIGHT));

    
    bool usingScaleInFreeCam = false;
    if (orbitCam) {
        // unproject to get modelspace eye position
        int viewport[4];
        double matModelView[16], matProjection[16];
        double dX, dY, dZ;
        glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
        glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
        glGetIntegerv(GL_VIEWPORT, viewport);
        gluUnProject((viewport[2] - viewport[0]) / 2, (viewport[3] - viewport[1]) / 2,
                     0.0, matModelView, matProjection, viewport,
                     &dX, &dY, &dZ);
        AtmShader->SetUniformVariable("eyePos", glm::vec4(dX, dY, dZ, 1));
        // AtmShader->SetUniformVariable("eyePos", glm::vec4(eyeX, eyeY, eyeZ, 1));
    } else if (!usingScaleInFreeCam) {
        AtmShader->SetUniformVariable("eyePos", glm::vec4(camX, camY, camZ, 1));
    }

    DrawIcoSphere(sphere3, vboId3, iboId3, AtmShader, true, ATM);


    // glEnable(GL_DEPTH_TEST); // enable depth again if disabled earlier for back face rendering


    if (OldCullFaceOn) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    glCullFace(OldCullFaceMode);
    if (OldBlendOn) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    glBlendFuncSeparate(OldBlendRGBSrc, OldBlendRGBDest, OldBlendAlpSrc, OldBlendAlpDest);

    // Re-establish fixed pipeline stuff
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);
    glPopMatrix();
}

void DrawAtmSpherePostGLM(glm::mat4 M, glm::mat4 MV, glm::mat4 MVP, glm::mat4 WV, glm::mat4 P)
{
    // Use copies of matrices
    glm::mat4 lM(M);
    glm::mat4 lMV(MV);
    glm::mat4 lMVP(MVP);
    glm::mat4 lWV(WV);
    glm::mat4 lP(P);

    GLint OldCullFaceOn;
    glGetIntegerv(GL_CULL_FACE, &OldCullFaceOn);
    GLint OldCullFaceMode;
    glGetIntegerv(GL_CULL_FACE_MODE, &OldCullFaceMode);
    GLint OldBlendOn;
    glGetIntegerv(GL_BLEND, &OldBlendOn);
    GLint OldBlendAlpSrc;
    glGetIntegerv(GL_BLEND_SRC_ALPHA, &OldBlendAlpSrc);
    GLint OldBlendAlpDest;
    glGetIntegerv(GL_BLEND_DST_ALPHA, &OldBlendAlpDest);
    GLint OldBlendRGBSrc;
    glGetIntegerv(GL_BLEND_SRC_RGB, &OldBlendRGBSrc);
    GLint OldBlendRGBDest;
    glGetIntegerv(GL_BLEND_DST_RGB, &OldBlendRGBDest);

    // glEnable(GL_CULL_FACE);
    // glCullFace(GL_FRONT);
    // glCullFace(GL_BACK);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDisable(GL_CULL_FACE); // with manual culling in fragment shader, disable here
    glDisable(GL_DEPTH_TEST); // disable depth if using back faces (render back of atm)


    glActiveTexture(GL_TEXTURE0 + DepthTextureIdx);
    glBindTexture(GL_TEXTURE_2D, DepthTexture);
    AtmShader->SetUniformVariable("depthTex", (int)(DepthTextureIdx));
    float w = glutGet(GLUT_WINDOW_WIDTH);
    float h = glutGet(GLUT_WINDOW_HEIGHT);
    // float w = glutGet(GLUT_SCREEN_WIDTH);
    // float h = glutGet(GLUT_SCREEN_HEIGHT);
    AtmShader->SetUniformVariable("screenWidth", (float)glutGet(GLUT_WINDOW_WIDTH));
    AtmShader->SetUniformVariable("screenHeight", (float)glutGet(GLUT_WINDOW_HEIGHT));

    AtmShader->SetUniformVariable("useModelOverWorld", orbitCam);
    AtmShader->SetUniformVariableMat4("matrixWorldViewProjection", &(lP*lWV)[0][0]);

    glActiveTexture(GL_TEXTURE0 + OpticalDepthTextureIdx);
    glBindTexture(GL_TEXTURE_2D, OpticalDepthTexture);
    AtmShader->SetUniformVariable("opticalDepthTex", (int)(OpticalDepthTextureIdx));
    AtmShader->SetUniformVariable("useBakedOpticalDepth", useBakedOpticalDepth);

    bool usingScaleInFreeCam = false;
    if (orbitCam) {
        // Get lMV and lP as doubles for gluUnProject
        double lMV_d[16];
        double lP_d[16];
        for (int i = 0; i < 16; i++) {
            lMV_d[i] = (double) (&lMV[0][0])[i];
            lP_d[i] = (double) (&lP[0][0])[i];
        }

        // unproject to get modelspace eye position
        int viewport[4];
        double matModelView[16], matProjection[16];
        double dX, dY, dZ;
        glGetDoublev(GL_MODELVIEW_MATRIX, matModelView);
        glGetDoublev(GL_PROJECTION_MATRIX, matProjection);
        glGetIntegerv(GL_VIEWPORT, viewport);
        gluUnProject((viewport[2] - viewport[0]) / 2, (viewport[3] - viewport[1]) / 2,
                     0.0, lMV_d, lP_d, viewport, &dX, &dY, &dZ);
        // gluUnProject((viewport[2] - viewport[0]) / 2, (viewport[3] - viewport[1]) / 2,
        //              0.0, matModelView, matProjection, viewport,
        //              &dX, &dY, &dZ);
        AtmShader->SetUniformVariable("eyePos", glm::vec4(dX, dY, dZ, 1));
        // AtmShader->SetUniformVariable("eyePos", glm::vec4(eyeX, eyeY, eyeZ, 1));
    } else if (!usingScaleInFreeCam) {
        AtmShader->SetUniformVariable("eyePos", glm::vec4(camX, camY, camZ, 1));
    }

    DrawIcoSpherePostGLM(sphere3, vboId3, iboId3, AtmShader, M, MV, MVP, WV, true, ATM);


    glEnable(GL_DEPTH_TEST); // enable depth again if disabled earlier for back face rendering


    if (OldCullFaceOn) glEnable(GL_CULL_FACE);
    else glDisable(GL_CULL_FACE);
    glCullFace(OldCullFaceMode);
    if (OldBlendOn) glEnable(GL_BLEND);
    else glDisable(GL_BLEND);
    glBlendFuncSeparate(OldBlendRGBSrc, OldBlendRGBDest, OldBlendAlpSrc, OldBlendAlpDest);
}

void SetShaderUniforms(GLSLProgram *Shader, bool lite) {
    // Setup shader uniforms
    Shader->Use();
        glm::vec3 landCol1 = glm::vec3(0.3, 0.7, 0.2);
        // glm::vec3 landCol2 = glm::vec3(0.2, 0.15, 0.07); // brown mountains
        glm::vec3 landCol2 = glm::vec3(0.3, 0.3, 0.3); // grey mountains
        glm::vec3 oceanCol = glm::vec3(0.1, 0.3, 0.7);  // blue oceans
        const float lAmb = 0.6, lDif = 0.8, lSpec = 0.6;
        const float oAmbLand1 = 0.5, oDifLand1 = 0.7, oSpecLand1 = 0.2, oShinLand1 = 0.5;
        const float oAmbLand2 = 0.5, oDifLand2 = 0.7, oSpecLand2 = 0.2, oShinLand2 = 0.5;
        const float oAmbOcean = 0.5, oDifOcean = 0.8, oSpecOcean = 0.7, oShinOcean = 50;

        Shader->SetUniformVariable("lightPosition", glm::vec4(SkyLightPos[CurrSky], 0));
        Shader->SetUniformVariable("lightAmbient", lAmb*SkyAmbCol[CurrSky]);
        Shader->SetUniformVariable("lightDiffuse", lDif*SkyLightCol[CurrSky]);
        Shader->SetUniformVariable("lightSpecular", lSpec*SkyLightCol[CurrSky]);
        Shader->SetUniformVariable("lightCol", SkyLightCol[CurrSky]);


        if (!lite) {
            // Do light scattering coefficients
            // float scatteringStrength = 50;
            // float scatteringStrength = 20;
            // float scatteringStrength = 30;
            float scatteringStrength = 5;
            // float scatteringStrength = 4;
            // float scatteringStrength = 2;
            // float scatteringStrength = 3;
            // float scatteringStrength = 6;
            // float scatteringStrength = 5;
            // float scatteringStrength = 30;
            // float scatteringStrength = 15;
            // float scatteringStrength = 15;
            // float scatteringStrength = 1;
            // glm::vec3 wavelengths(700, 530, 700);
            // glm::vec3 wavelengths(1000, 2000, 100);
            // glm::vec3 wavelengths(300.f, 100.f, 200.f);
            // glm::vec3 wavelengths(550, 530, 500);
            // glm::vec3 wavelengths(600, 530, 300);
            // glm::vec3 wavelengths(800, 530, 220);
            // glm::vec3 wavelengths(600, 530, 440);
            // glm::vec3 wavelengths(630.f, 530.f, 600.f);
            glm::vec3 wavelengths(700.f, 530.f, 470.f);
            // glm::vec3 wavelengths(700.f, 530.f, 440.f);
            // float scaler = 350.f;
            // float scaler = 275.f;
            // float scaler = 50.f;
            float scaler = 400.f;
            // float power = 2;
            float power = 4;
            float scatterR = pow(scaler/wavelengths.r, power) * scatteringStrength;
            float scatterG = pow(scaler/wavelengths.g, power) * scatteringStrength;
            float scatterB = pow(scaler/wavelengths.b, power) * scatteringStrength;
            float scatterA = 1;
            // float scatterR = pow(scaler/wavelengths.r, power) * scatteringStrength;
            // float scatterG = 0.9*pow(scaler/wavelengths.g, power) * scatteringStrength;
            // float scatterB = 1.2*pow(scaler/wavelengths.b, power) * scatteringStrength;
            // float scatterA = 1;
            // float scatterA = 3;
            // float scatterA = 1.5;
            // float scatterA = 0.25 * scatteringStrength;
            glm::vec4 scattering(scatterR, scatterG, scatterB, scatterA);
            Shader->SetUniformVariable("scatterCoef", scattering);

            Shader->SetUniformVariable("materialAmbientLand1", glm::vec4(oAmbLand1*landCol1, 1));
            Shader->SetUniformVariable("materialDiffuseLand1", glm::vec4(oDifLand1*landCol1, 1));
            Shader->SetUniformVariable("materialSpecularLand1", glm::vec4(oSpecLand1*landCol1, 1));
            Shader->SetUniformVariable("materialShininessLand1", oShinLand1);
            Shader->SetUniformVariable("materialAmbientLand2", glm::vec4(oAmbLand2*landCol2, 1));
            Shader->SetUniformVariable("materialDiffuseLand2", glm::vec4(oDifLand2*landCol2, 1));
            Shader->SetUniformVariable("materialSpecularLand2", glm::vec4(oSpecLand2*landCol2, 1));
            Shader->SetUniformVariable("materialShininessLand2", oShinLand2);
            Shader->SetUniformVariable("materialAmbientOcean", glm::vec4(oAmbOcean*oceanCol, 1));
            Shader->SetUniformVariable("materialDiffuseOcean", glm::vec4(oDifOcean*oceanCol, 1));
            Shader->SetUniformVariable("materialSpecularOcean", glm::vec4(oSpecOcean*oceanCol, 1));
            Shader->SetUniformVariable("materialShininessOcean", oShinOcean);
            Shader->SetUniformVariable("oceanThresh", oceanThresh);
            Shader->SetUniformVariable("aveNoise", aveNoise);
            Shader->SetUniformVariable("minNoise", minNoise);
            Shader->SetUniformVariable("maxNoise", maxNoise);
            Shader->SetUniformVariable("useNoise", 1);
            Shader->SetUniformVariable("map0", (int)NoiseTextureIdx);
            // Shader->SetUniformVariable("map0", 0);
            Shader->SetUniformVariable("oceanRad", sphere2_rad);
            Shader->SetUniformVariable("atmRad", sphere3_rad);
            Shader->SetUniformVariable("depthMin", depthMin);
            Shader->SetUniformVariable("depthMax", depthMax);
            Shader->SetUniformVariable("scale", Scale);
            Shader->SetUniformVariable("screenWidth", (float)glutGet(GLUT_WINDOW_WIDTH));
            Shader->SetUniformVariable("screenHeight", (float)glutGet(GLUT_WINDOW_HEIGHT));
        }
    Shader->UnUse();
}




GLuint setDepthTexture()
{
    // if (DepthTextureSet) {
    //     glDeleteTextures(1, &DepthTexture);
    // }

    // Only generate new texture first time around
    if (!DepthTextureSet)
        glGenTextures(1, &DepthTexture);

    glActiveTexture(GL_TEXTURE0 + DepthTextureIdx);
    glBindTexture(GL_TEXTURE_2D, DepthTexture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, glutGet(GLUT_WINDOW_WIDTH),
                 glutGet(GLUT_WINDOW_HEIGHT), 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri (GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_NONE);

    DepthTextureSet = true;

    return DepthTexture;
}



// Required camVFwd to be initialized
void UpdateCamDirs() {

    // Flip movement mat signs depending on angle
    int pr = 100;
    int p = ((((int)(pr*camYaw)-(pr*90)) % (pr*360)) + (pr*360)) % (pr*360);
    int pitchSign = p > pr*180 ? -1 : 1;  // if yaw more than 90 away from 0,360,etc, then swap sign
    int y = ((((int)(pr*camYaw)) % (pr*360)) + (pr*360)) % (pr*360);
    int yawSign = y < p*180 ? -1 : 1;  // if yaw more than 90 away from 0,360,etc, then swap sign
    // int y = ((((int)(pr*camPitch)-(pr*90)) % (pr*360)) + (pr*360)) % (pr*360);
    // int yawSign = y < p*90 ? -1 : 1;  // if yaw more than 90 away from 0,360,etc, then swap sign
    // int yawSign = y < p*180 ? -1 : 1;  // if yaw more than 90 away from 0,360,etc, then swap sign


    // Rotate using pitch (about rght)
    glm::mat4 rotVMat = glm::rotate(glm::mat4(1), -glm::radians(camPitch), camRghtInit);
    glm::mat4 rotMMat = glm::rotate(glm::mat4(1), pitchSign*yawSign*glm::radians(camPitch), camRghtInit);

    // Rotate using yaw (about up)
    glm::vec3 yawVAxis = camUpInit * glm::mat3(rotVMat);
    rotVMat = glm::rotate(rotVMat, -glm::radians(camYaw), yawVAxis);
    glm::vec3 yawMAxis = camUpInit * glm::mat3(rotMMat);
    rotMMat = glm::rotate(rotMMat, glm::radians(camYaw), yawMAxis);

    // Set camFwd, camUp, and camRght
    camVFwd = camFwdInit * glm::mat3(rotVMat);
    camVRght = camRghtInit * glm::mat3(rotVMat);
    camVUp = camUpInit * glm::mat3(rotVMat);
    camMFwd = camFwdInit * glm::mat3(rotMMat);
    camMRght = camRghtInit * glm::mat3(rotMMat);
    camMUp = camUpInit * glm::mat3(rotMMat);

    if (DrawFPS)
        int wow = 1;

    // Fix signs
    // if (camVRght.x+camVRght.y+camVRght.z < 0) camVRght *= -1;
    // if (camVUp.x+camVUp.y+camVUp.z < 0) camVUp *= -1;


    // if (DrawFPS)
    //     int wow = 1;
    
    // // Local fwd
    // glm::vec4 lFwd(camFwdInit, 1);
    // glm::vec4 lRght(camRghtInit, 1);
    // glm::vec4 lUp(camUpInit, 1);

    // lFwd = lFwd * rotMat;
    // glm::vec3 yawAxis = glm::normalize(glm::cross(camRghtInit, camFwdInit*glm::mat3(rotMat)));
    // rotMat = glm::rotate(rotMat, glm::radians(camYaw), yawAxis);

    // // // First rot
    // // glm::vec3 rotAxis1(0, 1, 0); // around y
    // // glm::mat4 rotMat = glm::rotate(glm::mat4(1), glm::radians(camYaw), rotAxis1);
    // // // glm::vec3 rotAxis1(1,0,0); // around x
    // // // glm::mat4 rotMat = glm::rotate(glm::mat4(1), glm::radians(camPitch), rotAxis1);



    // glm::vec3 rotFwd = lFwd*glm::mat3(rotMat);


    // // Second rot by vec perp to forward and x
    // // glm::vec3 rotAxis2 = glm::vec3(1, 0, 0);
    // // glm::vec3 rotAxis2 = glm::vec3(0, 1, 0);
    // glm::vec3 rotAxis2 = glm::normalize(glm::cross(rotAxis1, rotFwd));
    // // glm::vec3 rotAxis2 = glm::normalize(glm::cross(rotAxis1, glm::vec4(camVFwd,1)*rotMat));
    // // glm::vec3 rotAxis2 = glm::normalize(glm::cross(rotAxis1, camVFwd));

    // int camPitchSign = rotAxis2.x + rotAxis2.y + rotAxis2.z < 0 ? -1 : 1;
    // rotMat = glm::rotate(rotMat, glm::radians(camPitchSign*camPitch), rotAxis2);
    // // rotMat = glm::rotate(rotMat, glm::radians(camYaw), rotAxis2);

    // glm::vec3 rotFwd2 = glm::vec3(0,0,-1)*glm::mat3(rotMat);

    // if (DrawFPS)
    //     int wow = 1;

    // Apply rot to get new camVFwd
    // camVFwd = glm::vec4(camVFwd, 1) * rotMat;
    // camVFwd = glm::vec4(0, 0, -1, 1) * rotMat;
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

        // Cleanup frame buffer stuff
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &DepthTexture);
        glDeleteTextures(1, &OpticalDepthTexture);

        // Quit
        exit(0);
        break;

    default:
        fprintf(stderr, "Don't know what to do with Main Menu ID %d\n", id);
    }

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

float WrapVal(float x, float min, float max) {
    return x - (max-min) * floor(x / (max-min)) + min;
}

// use glut to display a string of characters using a raster font:
void DoRasterString(float x, float y, float z, char *s)
{
    // If negative, wrap around
    // int w = glutGet(GLUT_WINDOW_WIDTH), h = glutGet(GLUT_WINDOW_HEIGHT);
    // x = WrapVal(x, 0, w);
    // y = WrapVal(y, 0, h);

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

    // int w = glutGet(GLUT_SCREEN_WIDTH);

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
    glutKeyboardUpFunc(KeyboardUp);
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
    // glutTimerFunc(-1, NULL, 0);
    // glutIdleFunc(Animate);
    // glutTimerFunc(ANIMATE_TIMER, Reset, 0);
    glutTimerFunc(ANIMATE_TIMER, AnimateTimer, 0);

    // Disable key repeating
    glutSetKeyRepeat(GLUT_KEY_REPEAT_OFF);


    // init glew (a window must be open to do this):
    GLenum err = glewInit();
    if (err != GLEW_OK)
        fprintf(stderr, "glewInit Error\n");
    else
        fprintf(stderr, "GLEW initialized OK\n");
        
    fprintf(stderr, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));

    bool valid;

    // Icosphere shader setup
    char icovert[50];
    char icofrag[50];
    strcpy(icovert, "shaders/icosphere3D.vert");
    strcpy(icofrag, "shaders/icosphere3D.frag");

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

    // Get attrib locations
    attribVertexPosition = glGetAttribLocation(IcoShader->Program, "vertexPosition");
    attribVertexNormal   = glGetAttribLocation(IcoShader->Program, "vertexNormal");
    attribVertexTexCoord = glGetAttribLocation(IcoShader->Program, "vertexTexCoord");

    // Atmosphere shader setup
    char atmvert[50];
    char atmfrag[50];
    strcpy(atmvert, "shaders/atmosphere.vert");
    strcpy(atmfrag, "shaders/atmosphere.frag");

    int atm_attrib_idx[3] = {0, 1, 2};
    const char *atm_attrib_names[3] = {"vertexPosition", "vertexNormal", "vertexTexCoord"};

    AtmShader = new GLSLProgram(3, atm_attrib_idx, atm_attrib_names);
    valid = AtmShader->Create(atmvert, atmfrag);

    if (valid) {
        fprintf(stderr, "Atm shader created.\n");
    } else {
        fprintf(stderr, "Atm shader cannot be created!\n");
        DoMainMenu(QUIT);
    }

    // Get attrib locations
    attribAtmVertexPosition = glGetAttribLocation(AtmShader->Program, "vertexPosition");
    attribAtmVertexNormal   = glGetAttribLocation(AtmShader->Program, "vertexNormal");
    attribAtmVertexTexCoord = glGetAttribLocation(AtmShader->Program, "vertexTexCoord");


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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId2);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sphere2.getIndexSize(),
                    sphere2.getIndices(),
                    GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // Copy interleaved vertex data from atmosphere icosphere
    glGenBuffers(1, &vboId3);
    glBindBuffer(GL_ARRAY_BUFFER, vboId3);
    glBufferData(GL_ARRAY_BUFFER,                      // target
                    sphere3.getInterleavedVertexSize(), // # of bytes
                    sphere3.getInterleavedVertices(),   // ptr to vertices
                    GL_STATIC_DRAW);                   // usage
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &iboId3);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboId3);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                    sphere3.getIndexSize(),
                    sphere3.getIndices(),
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
        SkyShader->SetUniformVariable("gCubemapTexture", (int)SkyTextureIdx);
        // SkyShader->SetUniformVariable("gCubemapTexture", 0);
    SkyShader->UnUse();

    // Setup skySphere vbo and ibo
    glGenBuffers(1, &vboIdSky);
    glBindBuffer(GL_ARRAY_BUFFER, vboIdSky);
    glBufferData(GL_ARRAY_BUFFER,                      // target
                 skySphere.getInterleavedVertexSize(), // # of bytes
                 skySphere.getInterleavedVertices(),   // ptr to vertices
                 GL_STATIC_DRAW);                      // usage
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &iboIdSky);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, iboIdSky);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                 skySphere.getIndexSize(),
                 skySphere.getIndices(),
                 GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    // create framebuffer shader
    char fbovert[50];
    char fbofrag[50];
    strcpy(fbovert, "shaders/icosphere3D.vert");
    // strcpy(fbovert, "shaders/screenidentity.vert");
    strcpy(fbofrag, "shaders/screengetdepth.frag");

    // const int fbo_n = 1;
    // int fbo_attrib_idx[fbo_n] = {0};
    // const char *fbo_attrib_names[fbo_n] = {"vp"};
    // DepthShader = new GLSLProgram(fbo_n, fbo_attrib_idx, fbo_attrib_names);
    // valid = DepthShader->Create(fbovert, fbofrag);

    const int fbo_n = 3;
    int fbo_attrib_idx[fbo_n] = {0, 1, 2};
    const char *fbo_attrib_names[fbo_n] = {"vertexPosition", "vertexNormal", "vertexTexCoord"};
    DepthShader = new GLSLProgram(fbo_n, fbo_attrib_idx, fbo_attrib_names);
    valid = DepthShader->Create(fbovert, fbofrag);

    if (valid) {
        fprintf(stderr, "FBO shader created.\n");
    } else {
        fprintf(stderr, "FBO shader cannot be created!\n");
        DoMainMenu(QUIT);
    }

    // Get attrib positions
    attribFBOVertexPosition = glGetAttribLocation(DepthShader->Program, "vertexPosition");
    attribFBOVertexNormal   = glGetAttribLocation(DepthShader->Program, "vertexNormal");
    attribFBOVertexTexCoord = glGetAttribLocation(DepthShader->Program, "vertexTexCoord");

    // set framebuffer
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    // set texture
    DepthTexture = setDepthTexture();

    // attach texture to framebuffer
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, DepthTexture, 0);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Failed to bind framebuffer" << std::endl;
        DoMainMenu(QUIT);
    }

    // Test quad shader setup
    char tqvert[50];
    char tqfrag[50];
    strcpy(tqvert, "shaders/testquad.vert");
    strcpy(tqfrag, "shaders/testquad.frag");

    const int tq_num_attrib = 1;
    int tq_attrib_idx[tq_num_attrib] = {0};
    const char *tq_attrib_names[tq_num_attrib] = {"vertexIn"};

    TestQuadShader = new GLSLProgram(tq_num_attrib, tq_attrib_idx, tq_attrib_names);
    valid = TestQuadShader->Create(tqvert, tqfrag);

    if (valid) {
        fprintf(stderr, "Test Quad shader created.\n");
    } else {
        fprintf(stderr, "Test Quad shader cannot be created!\n");
        DoMainMenu(QUIT);
    }

    // Get attrib locations
    testQuadAttribPosIdx = glGetAttribLocation(TestQuadShader->Program, "vertexIn");
    
    // Setup ico and atm shader uniforms
    SetShaderUniforms(IcoShader, false);
    SetShaderUniforms(AtmShader, false);
    SetShaderUniforms(DepthShader, false);

    // Generate baked optical depth texture
    GenOpticalDepthTex();
}


void GenOpticalDepthTex() {
    // Generate data for lookup (bake the depths)
    const int dim = 512;
    // const int dim = 256;
    float data[dim*dim];
    for (int x = 0; x < dim; x++)
        for (int y = 0; y < dim; y++)
            data[y*dim + x] = BakedOpticalDepth(x,y,dim);

    // Only generate new texture first time around
    if (!OpticalDepthTextureSet)
        glGenTextures(1, &OpticalDepthTexture);
    
    // Update texture in tex with this new data
    glActiveTexture(GL_TEXTURE0 + OpticalDepthTextureIdx); // activate unit
    glBindTexture(GL_TEXTURE_2D, OpticalDepthTexture); // bind texture

    // select modulate to mix texture with color for shading
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // Setup min and mag filter and mipmap stuff
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // Setup wrap parameter
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT); // or GL_CLAMP
    // glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // or GL_CLAMP
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Copy 2D noise texture data
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, dim, dim, 0,
                 GL_RED, GL_FLOAT, data);

    // Generate mipmap for 2D texture
    glGenerateMipmap(GL_TEXTURE_2D);

    // Unbind tex
    glBindTexture(GL_TEXTURE_2D, 0);
}


// get what the optical depth should be at a given x,y out of dim
float BakedOpticalDepth(int x, int y, int dim) {
    float angleIdx = (float) x / (float) dim; // 0 up, 1 down
    float heightIdx = (float) y / (float) dim;

    glm::vec3 planetCenter(0,0,0);
    float planetRad = sphere2_rad; // at ocean height
    float atmosphereRad = sphere3_rad; // at atm height

    // (1,0,0) is up, (-1,0,0) is down, (0,1,0) is halfway
    float pitch = M_PI * (angleIdx); // 0 up, M_PI down
    glm::vec3 rayDir(cos(pitch), sin(pitch), 0);

    float height = heightIdx * (atmosphereRad-planetRad) + planetRad;
    float numSamplePoints = 10;

    // how far through the atmosphere to check
    float len = height + atmosphereRad; // max dist to other side of atm
    // float len = height + planetRad; // max dist to other side of planet

    glm::vec3 samplePoint = planetCenter + height*glm::vec3(1,0,0);
    float stepSize = len / (numSamplePoints - 1);
    float opticalDepth = 0;

    for (int i = 0; i < numSamplePoints; i++) {
        // if (!inSphere(samplePoint, planetCenter, planetRad))
        float localDensity = BakedDensityAtPoint(samplePoint, planetCenter, planetRad, atmosphereRad);
        opticalDepth += localDensity * stepSize;
        samplePoint += rayDir * stepSize;
    }

    // // Clamp to [0,1]
    // if (opticalDepth < 0) opticalDepth = 0;
    // else if (opticalDepth > 1) opticalDepth = 1;

    return opticalDepth;
}

bool inSphere(glm::vec3 point, glm::vec3 center, float rad) {
    return glm::length2(point-center) <= rad*rad;
}


// gives density at point for purposes of baking into texture
float BakedDensityAtPoint(glm::vec3 samplePoint, glm::vec3 planetCenter, float planetRad, float atmosphereRad) {
    float densityFalloff = 1.5;
    float heightAboveSurface = glm::length(samplePoint - planetCenter) - planetRad;
    float height01 = heightAboveSurface / (atmosphereRad - planetRad);
    float localDensity = exp(-height01 * densityFalloff) * (1-height01);
    return localDensity;
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

// std::map<int, bool> keys;
// void pressKey(int key, int xx, int yy)
// {
//     keys[key] = true;
// }
// void relaseKey(int key, int x, int y)
// {
//     keys[key] = false;
// }

// the keyboard callback:
void Keyboard(unsigned char c, int x, int y)
{
    int sub;
    float subf;

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

    case 'x':
        // Update ico subdivision
        sub = sphere.getSubdivision() - 1;
        if (sub < 0) {sub = 0;}
        else { sphere.setSubdivision(sub);
               RecalcSphereMesh(sphere, vboId, iboId);
               sphere2.setSubdivision(sub);
               RecalcSphereMesh(sphere2, vboId2, iboId2); }
        break;
    case 'X':
        // Update ico subdivision
        sub = sphere.getSubdivision() + 1;
        if (sub > 6) {sub = 6;}
        else { sphere.setSubdivision(sub);
               RecalcSphereMesh(sphere, vboId, iboId);
               sphere2.setSubdivision(sub);
               RecalcSphereMesh(sphere2, vboId2, iboId2); }
        break;

    case 'c':
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
            DepthShader->Use();
                DepthShader->SetUniformVariable("oceanThresh", oceanThresh);
            DepthShader->UnUse();
        }
        break;
    case 'C':
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
            DepthShader->Use();
                DepthShader->SetUniformVariable("oceanThresh", oceanThresh);
            DepthShader->UnUse();
        }
        break;

    case 'M':
    case 'm':
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

    case 'f':
    case ' ':
        // Toggle scene rotation
        SceneRotateOn = !SceneRotateOn;
        break;

    case char(13): // enter
        // Generate new planet
        GenerateNewPlanet();
        
        // New radii for ocean and atmosphere
        sphere2.setRadius(sphere2_rad);
        sphere3.setRadius(sphere3_rad);
        RecalcSphereMesh(sphere2, vboId2, iboId2);
        RecalcSphereMesh(sphere3, vboId3, iboId3);
        break;


    case '[':
        CurrSky = static_cast<Skies>(static_cast<int>(CurrSky) - 1);
        if (CurrSky < 0)
            CurrSky = static_cast<Skies>(0);
        SetShaderUniforms(IcoShader, true);
        SetShaderUniforms(AtmShader, true);
        SetShaderUniforms(DepthShader, false);
        break;
    case ']':
        CurrSky = static_cast<Skies>(static_cast<int>(CurrSky) + 1);
        if (CurrSky >= NumSkies)
            CurrSky = static_cast<Skies>(NumSkies - 1);
        SetShaderUniforms(IcoShader, true);
        SetShaderUniforms(AtmShader, true);
        SetShaderUniforms(DepthShader, false);
        break;


    case 'z':
        SceneRotateSpeed *= .9;
        if (SceneRotateSpeed < MinSceneRotateSpeed)
            SceneRotateSpeed = MinSceneRotateSpeed; 
        break;
    case 'Z':
        SceneRotateSpeed *= 1.1;
        if (SceneRotateSpeed > MaxSceneRotateSpeed)
            SceneRotateSpeed = MaxSceneRotateSpeed;
        break;

    case 'v':
        orbitCam = !orbitCam;
        break;
    case 'V':
        trapMouse = !trapMouse;
        MouseProcessingTimer = 100;
        if (trapMouse) {
            float cx = glutGet(GLUT_WINDOW_WIDTH) / 2.f;
            float cy = glutGet(GLUT_WINDOW_HEIGHT) / 2.f;
            Xmouse = cx; Ymouse = cy;

            glutSetCursor(GLUT_CURSOR_NONE);
            glutWarpPointer(cx, cy);
            // glfwSetMousePos((float)glutGet(GLUT_WINDOW_WIDTH)/2.f, (float)glutGet(GLUT_WINDOW_HEIGHT)/2.f);
        } else {
            glutSetCursor(GLUT_CURSOR_INHERIT);
        }
        break;

    // Baked or non-baked atmosphere
    case 'b':
    case 'B':
        useBakedOpticalDepth = !useBakedOpticalDepth;
        break;

    // Movement keys
    case 'w': case 'W': camFwdMove = 1;     break;
    case 's': case 'S': camFwdMove = -1;    break;
    case 'd': case 'D': camRghtMove = 1;    break;
    case 'a': case 'A': camRghtMove = -1;   break;
    case 'e': case 'E': camUpMove = 1;      break;
    case 'q': case 'Q': camUpMove = -1;     break;


    case 'r':
    case 'R':
        Reset(true);
        break;


    case '?':
        DrawHelpText = !DrawHelpText;
        break;
    case '/':
        DrawFPS = !DrawFPS;
        break;


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

// the keyboard up callback:
void KeyboardUp(unsigned char c, int x, int y)
{
    switch (c)
    { 
        // Movement keys
        case 'w': case 'W': case 's': case 'S': camFwdMove = 0;    break;
        case 'd': case 'D': case 'a': case 'A': camRghtMove = 0;   break;
        case 'e': case 'E': case 'q': case 'Q': camUpMove = 0;     break;
    }
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
        if (orbitCam) {
            Scale += SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
        } else {
            glm::vec3 newPos(camTX, camTY, camTZ);
            newPos += 1.5f * camMFwd * SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
            camTX = newPos.x, camTY = newPos.y, camTZ = newPos.z;
            // glm::vec3 newPos(camX, camY, camZ);
            // newPos += 1.5f * camMFwd * SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
            // camX = newPos.x, camY = newPos.y, camZ = newPos.z;
        }
        // keep object from turning inside-out or disappearing:
        if (Scale < MINSCALE)
            Scale = MINSCALE;
        break;

    case SCROLL_WHEEL_DOWN:
        if (orbitCam) {
            Scale -= SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
        } else {
            glm::vec3 newPos(camTX, camTY, camTZ);
            newPos -= 1.5f * camMFwd * SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
            camTX = newPos.x, camTY = newPos.y, camTZ = newPos.z;
            // glm::vec3 newPos(camX, camY, camZ);
            // newPos -= 1.5f * camMFwd * SCLFACT * SCROLL_WHEEL_CLICK_FACTOR;
            // camX = newPos.x, camY = newPos.y, camZ = newPos.z;
        }
        // keep object from turning inside-out or disappearing:
        if (Scale < MINSCALE)
            Scale = MINSCALE;
        break;

    default:
        b = 0;
        fprintf(stderr, "Unknown mouse button: %d\n", button);
    }

    // button down sets the bit, up clears the bit:
    if (state == GLUT_DOWN) {
        Xmouse = x;
        Ymouse = y;
        ActiveButton |= b; // set the proper bit
    } else {
        ActiveButton &= ~b; // clear the proper bit
    }

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}

// called when the mouse moves while a button is down:
void MouseMotion(int x, int y)
{
    // Skip processing if in timeout
    float trapScale = trapMouse ? 0.5 : 1;
    if (MouseProcessingTimer <= 0)
    {
        int dx = x - Xmouse; // change in mouse coords
        int dy = y - Ymouse;

        // if ((ActiveButton & LEFT) != 0)
        if ((ActiveButton & LEFT) != 0 || trapMouse)
        {
            if (orbitCam) {
                TXrot += trapScale*(ANGFACT * dy);
                TYrot += trapScale*(ANGFACT * dx);
                // Xrot += (ANGFACT * dy);
                // Yrot += (ANGFACT * dx);
            } else {
                float fpsSens = 0.5;
                float newTP = camTPitch + fpsSens * trapScale*(ANGFACT * dy);
                if (newTP < 90 && newTP > -90) camTPitch = newTP;
                camTYaw += fpsSens * trapScale * (ANGFACT * dx);
                // float newP = camPitch + fpsSens * trapScale*(ANGFACT * dy);
                // if (newP < 90 && newP > -90) camPitch = newP;
                // camYaw += fpsSens * trapScale*(ANGFACT * dx);
                // UpdateCamDirs();
            }
        }

        if ((ActiveButton & MIDDLE) != 0)
        {
            if (orbitCam) {
                Scale += SCLFACT * (float)(dx - dy);
            } else {
                glm::vec3 newPos(camTX, camTY, camTZ);
                newPos += 3.f * camMFwd * SCLFACT * (float)(dx - dy);
                camTX = newPos.x, camTY = newPos.y, camTZ = newPos.z;
                // glm::vec3 newPos(camX, camY, camZ);
                // newPos += 3.f * camMFwd * SCLFACT * (float)(dx - dy);
                // camX = newPos.x, camY = newPos.y, camZ = newPos.z;
            }

            // keep object from turning inside-out or disappearing:
            if (Scale < MINSCALE)
                Scale = MINSCALE;
        }

        Xmouse = x; // new current position
        Ymouse = y;

        // Slow down mouse motion
        MouseProcessingTimer = 10;
    }

    // Trap to center of screen if enabled
    if (trapMouse && MouseTrapTimer <= 0) {
        glm::vec2 center((float)glutGet(GLUT_WINDOW_WIDTH)/2.f,
                         (float)glutGet(GLUT_WINDOW_HEIGHT)/2.f);
        glm::vec2 diff = glm::vec2(x,y) - center;
        glm::vec2 absDiff = glm::abs(diff);
        if (absDiff.x > trapMouseRad || absDiff.y > trapMouseRad)
        {
            glutWarpPointer(center.x, center.y);
            Xmouse = center.x; // new current position
            Ymouse = center.y;
        }

        MouseTrapTimer = 50;
        // trapScale = 100;
    }

    glutSetWindow(MainWindow);
    glutPostRedisplay();
}


// reset the transformations and the colors:
// this only sets the global variables --
// the glut main loop is responsible for redrawing the scene
void Reset(bool redisplay)
{
    // reset general
    Xrot = 0, Yrot = 0;
    TXrot = 0, TYrot = 0;
    OXrot = 0, OYrot = 0;
    TOXrot = 0, TOYrot = 0;
    ActiveButton = 0;
    WhichProjection = PERSP;
    DrawHelpText = false;
    DrawFPS = false;
    Scale = 1;
    // Scale = 0.7;
    fixedLighting = false;
    oceanThresh = oceanThreshDefVal;
    SceneRotateOn = false;
    SceneRotateSpeed = 1;
    useSSAO = true;
    camFwdMove = 0, camRghtMove = 0, camUpMove = 0;
    useBakedOpticalDepth = false;

    // reset icospheres
    sphere.setRadius(1.f);
    sphere.setSubdivision(5);
    sphere.setSmooth(true);
    sphere2.setRadius(sphere2_rad);
    sphere2.setSubdivision(5);
    sphere2.setSmooth(true);
    sphere3.setRadius(sphere3_rad);
    sphere3.setSubdivision(5);
    sphere3.setSmooth(true);
    if (redisplay) RecalcSphereMesh(sphere, vboId, iboId);
    if (redisplay) RecalcSphereMesh(sphere2, vboId2, iboId2);
    if (redisplay) RecalcSphereMesh(sphere3, vboId3, iboId3);
    if (redisplay) RecalcSphereMesh(skySphere, vboIdSky, iboIdSky);
    
    // reset CurrSky
    CurrSky = NORMAL;
    
    // reset cam position
    orbitCam = true;
    eyeX = 0.f, eyeY = 0.f, eyeZ = 5.f;
    camX = 0.f, camY = 0.f, camZ = 5.f;
    camTX = 0.f, camTY = 0.f, camTZ = 5.f;
    camPitch = 0.f, camYaw = 0.f;
    camTPitch = 0.f, camTYaw = 0.f;
    UpdateCamDirs();
    trapMouse = false;
    MouseProcessingTimer = 0;
    MouseTrapTimer = 0;
    trapMouseRad = 0.03*std::min(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

    // redisplay if requested (only true after everything init'd)
    if (redisplay) {
        if (trapMouse)  glutSetCursor(GLUT_CURSOR_NONE);
        else            glutSetCursor(GLUT_CURSOR_INHERIT);

        SetShaderUniforms(IcoShader, false);
        SetShaderUniforms(AtmShader, false);
        SetShaderUniforms(DepthShader, false);

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

    resizeTimer = RESIZE_TIMER_VAL;

    SetShaderUniforms(IcoShader, false);
    SetShaderUniforms(AtmShader, false);
    SetShaderUniforms(DepthShader, false);

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

