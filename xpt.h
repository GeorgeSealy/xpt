#ifndef _xpt_h_
#define _xpt_h_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* If using gl3.h */
/* Ensure we are using opengl's core profile only */
#define GL3_PROTOTYPES 1

#if APPLE

#include "SDL2/SDL.h"
// #include "SDL2/SDL_opengl.h"
#include <OpenGL/OpenGL.h>
#include <OpenGL/gl3.h>

#elif WINDOWS

#define GLEW_STATIC
#include <glew.h>
#include <GL/glu.h>
#include <GL/gl.h>

#include "SDL.h"
#include "SDL_opengl.h"

#endif

#include "vectorial/vectorial.h"
using namespace vectorial;

typedef uint16_t ushort;
typedef uint32_t uint;

#include "System.h"

#endif // _xpt_h_
