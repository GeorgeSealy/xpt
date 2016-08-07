#include <stdio.h>
#include "SDL.h"

#if APPLE

#include "SDL2/SDL.h"
#include "SDL2/SDL_opengl.h"

#elif WINDOWS

#include "SDL.h"
#include "SDL_opengl.h"

#endif

#include "vectorial/vectorial.h"
using namespace vectorial;

void killApp(const char *msg)
{
    printf("SDL Error: %s: %s\n", msg, SDL_GetError());
    SDL_Quit();
    exit(1);
}


void checkSDLError(int line = -1)
{
#ifdef DEBUG
    const char *error = SDL_GetError();
    if (*error != '\0')
    {
        printf("SDL Error: %s\n", error);
        if (line != -1)
            printf(" + line: %i\n", line);
        SDL_ClearError();
    }
#endif
}

struct InputState {
    InputState() {
        shouldQuit = false;
    }
    
    bool shouldQuit;
};

InputState updateInputs(InputState &inputs) {
    SDL_Event event;
    
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                inputs.shouldQuit = true;
                break;
                
            case SDL_KEYDOWN:
                switch( event.key.keysym.sym ) {
                    case SDLK_ESCAPE:
                        inputs.shouldQuit = true;
                        break;
                    default:
                        break;
                }
                break;
                
            case SDL_KEYUP:
                /* Check the SDLKey values and move change the coords */
                switch( event.key.keysym.sym ) {
				case SDLK_ESCAPE:
					inputs.shouldQuit = true;
					break;
				default:
                        break;
                }
                break;
                
            default:
                break;
        }
    }
    
    return inputs;
}

void initWindowAndContext(const char *windowName, SDL_Window **window, SDL_GLContext *context) {
    
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        killApp("Unable to initialize SDL");
    
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_FLAGS, SDL_GL_CONTEXT_FORWARD_COMPATIBLE_FLAG);
    SDL_GL_SetAttribute (SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
    
    *window = SDL_CreateWindow(windowName, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                               640, 480, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
    if (!*window) {
        killApp("Unable to create window");
    }
    
    checkSDLError(__LINE__);
    
    *context = SDL_GL_CreateContext(*window);
    checkSDLError(__LINE__);
    
    SDL_GL_SetSwapInterval(1);
}

void deleteWindowAndContext(SDL_Window *window, SDL_GLContext &context) {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
}

void runMainLoop(SDL_Window *window) {
    
//    setupGL();
    
//    double t = 0.0;
//    const double dt = 1.0 / 60.0;
//    
//    double currentTime = timer.seconds();
//    double accumulator = 0.0;
    
    InputState inputs;
    
    // Basic run loop from http://gafferongames.com/game-physics/fix-your-timestep/
    // TODO: (George) Final interpolation between states for super smoothness
    
    float y = 3.0;
    vec4f v = normalize( vec4f(1,2,3,4) + y );
    float z = v.z();
    
    printf("Z = %1.2f\n", z);
    
    while (!inputs.shouldQuit)
    {
//        double newTime = timer.seconds();
//        double frameTime = newTime - currentTime;
//        currentTime = newTime;
//        
        // GATHER USER INPUT (PLUS NETWORK INPUT?) - START
        inputs = updateInputs(inputs);
        
        // GATHER USER INPUT (PLUS NETWORK INPUT?) - END
//
//        accumulator += frameTime;
//        
//        while ( accumulator >= dt )
//        {
//            
//            // GAME STATE UPDATE - START
//            //            integrate( state, t, dt );
//            
//            ship.moveShip(inputs, dt, v3(0.0, 0.0, 0.0), planetRadius);
//            
//            // GAME STATE UPDATE - END
//            
//            accumulator -= dt;
//            t += dt;
//        }
        
        // GAME STATE RENDER - START
        
        
        glClearColor ( 0.05, 0.0, 0.1, 1.0 );
        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        
//        glEnable(GL_DEPTH_TEST);
//        glDepthFunc(GL_LESS);
//        CHECK_GL_ERRORS();
//        
//        Mat4x4 projectionMatrix;
//        Mat4x4 viewMatrix;
//        
//        buildProjectionMatrix(projectionMatrix, 45.0, 4.0 / 3.0, 10.0, 50000.0);
//        
//        ship.view.getCameraMatrix(viewMatrix);
//        
//        multMatrix(projectionMatrix, viewMatrix);
//        
//        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvpmatrix"), 1, GL_FALSE, projectionMatrix);
//        CHECK_GL_ERRORS();
//        
//        glUseProgram(shaderProgram);
//        CHECK_GL_ERRORS();
//        
//        glEnable(GL_CULL_FACE);
//        glCullFace(GL_BACK);
//        
//        glDisable(GL_BLEND);
//        glEnable(GL_DEPTH_TEST);
//        planet.render();
//        
//        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//        glEnable(GL_BLEND);
//        
//        glCullFace(GL_FRONT);
//        atmosphere.render();
//        
//        // TODO: (George) Only if we're outside the atmosphere...
//        glCullFace(GL_BACK);
//        atmosphere.render();
        
        SDL_GL_SwapWindow(window);
//        CHECK_GL_ERRORS();
        
        // GAME STATE RENDER - END
    }
    
    // Clean up GL
//    glUseProgram(0);
//    glDisableVertexAttribArray(0);
//    glDisableVertexAttribArray(1);
    
}

int main( int argc, char * argv[] ) {
    // // (void)argc;
    // // (void)argv;
    // printf( "\n\nYou can't connect a client like this in secure mode. See connect.cpp and matcher.go instead\n" );

    printf("Args:\n");
    
    for (int i = 0; i < argc; ++i) {
        printf("%s ", argv[i]);
    }
    
    printf("\n");
    
    SDL_Window *mainWindow;
    SDL_GLContext mainGLContext;
    
    initWindowAndContext("Boo", &mainWindow, &mainGLContext);
    runMainLoop(mainWindow);
    deleteWindowAndContext(mainWindow, mainGLContext);
    
    SDL_Quit();

    return 0;
}