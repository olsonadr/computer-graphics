#ifndef OPENGL_INCLUDES_HPP
#define OPENGL_INCLUDES_HPP

#define GL_GLEXT_PROTOTYPES
#include "gl_ext/glew.h"
#include <GL/gl.h>
#include <GL/glu.h>
#include "gl_ext/glut.h"

#define GLM_ENABLE_EXPERIMENTAL
#define GLM_FORCE_RADIANS
#include "gl_ext/glm/glm.hpp"
#include "gl_ext/glm/gtc/matrix_transform.hpp"
#include "gl_ext/glm/gtc/type_ptr.hpp"
#include "gl_ext/glm/gtx/string_cast.hpp"

#endif // #ifndef OPENGL_INCLUDES_HPP