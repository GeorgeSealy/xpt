#ifndef _GLUtils_h_
#define _GLUtils_h_

#include <assert.h>

#ifdef DEBUG

#define CHECK_GL_ERRORS()                               \
do {                                                    \
GLenum error = glGetError();                            \
if(error != GL_NO_ERROR) {                          \
printf("OpenGL: %s [error %d]\n", __FUNCTION__, (int)error);					\
assert(0); \
} \
} while(false)

#else

#define CHECK_GL_ERRORS()

#endif


typedef enum {
    ShaderTypeVertex,
    ShaderTypeFragment,
    ShaderTypeGeometry,
} ShaderType;

extern uint compileShader(const char *shaderFilename, ShaderType shaderType);
extern uint linkShaders(uint vertexShader, uint geometryShader, uint fragmentShader);

// ----------------------------------------------------
// MATRIX STUFF
//

typedef float Mat4x4[16];

// sets the square matrix mat to the identity matrix,
// size refers to the number of rows (or columns)
extern void setIdentityMatrix(Mat4x4 &mat);

//
// a = a * b;
//
extern void multMatrix(Mat4x4 &a, Mat4x4 b);

// Defines a transformation matrix mat with a translation
extern void setTranslationMatrix(Mat4x4 &mat, float x, float y, float z);

// ----------------------------------------------------
// Projection Matrix
//

extern void buildProjectionMatrix(Mat4x4 &projMatrix, float fov, float ratio, float nearP, float farP);

// ----------------------------------------------------
// View Matrix
//
// note: it assumes the camera is not tilted,
// i.e. a vertical up vector (remmeber gluLookAt?)
//

extern void setCamera(Mat4x4 &viewMatrix, vec3f pos, vec3f lookAt);

#endif // _GLUtils_h_
