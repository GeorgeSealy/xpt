#include "xpt.h"
#include "GLUtils.h"
#include "Noise.h"
#include "MeshUtils.h"

const float planetRadius = 6000.0;

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

uint vertexShader = 0;
uint geometryShader = 0;
uint fragmentShader = 0;
uint shaderProgram = 0;

inline float normalizedHeightAboveSeaLevel(const vec3f &basePt) {
    vec3f pt = normalize(basePt);
    
    double n = noise(4.0 * pt.x(), 4.0 * pt.y(), 4.0 * pt.z());
    double n2 = noise(7.0 * pt.x(), 7.0 * pt.y(), 7.0 * pt.z());
    
    float height = n + 0.5 * n2;
    
    return height;
}

Mesh buildGridMesh(int squaresPerSide, vec3f startPt, vec3f acrossDir, vec3f upDir) {
    
    float scaleFactor = 1.0 / squaresPerSide;
    
    vec3f uStep = scaleFactor * acrossDir;
    vec3f vStep = scaleFactor * upDir;
    
    int numVerts = (squaresPerSide + 1) * (squaresPerSide + 1);

    vec3f *verts = (vec3f *)malloc(sizeof(vec3f) * numVerts);
    vec3f *norms = (vec3f *)malloc(sizeof(vec3f) * numVerts);
    vec4f *cols = (vec4f *)malloc(sizeof(vec4f) * numVerts);

    int vertIndex = 0;
    for (int j = 0; j <= squaresPerSide; ++j) {
        
        for (int i = 0; i <= squaresPerSide; ++i) {
            
            vec3f &pt = verts[vertIndex];
            
            vec3f u = i * uStep;
            vec3f v = j * vStep;

            pt = startPt + u + v;

//            // TODO: (George) Move the normalisation / height code out of here into a separate function
//            // TODO: (George) Will require an "update mesh" function and stored arrays in mesh struct
            pt = normalize(pt);
            
            float height = normalizedHeightAboveSeaLevel(pt);
            
            height += 0.1;
            
            double multiplier = 0.025;
            float distFromCentre = planetRadius * (1.0 + height * multiplier);
            
            if (height < 0.0) {
                distFromCentre = planetRadius;
            }
            
            pt = distFromCentre * pt;
            
            if (height < 0.0) {
                cols[vertIndex] = vec4f(0.2, 0.2 - height * 0.5, 1.0, 1.0);
            } else if (height < 0.8) {
                cols[vertIndex] = vec4f(0.2 + 0.4 * height, 0.5, 0.1 + 0.4 * height, 1.0);
            } else {
                cols[vertIndex] = vec4f(0.9, 0.9, 0.9, 1.0);
            }
            
            vertIndex += 1;
        }
    }
    
    int numIndices = (2 * (squaresPerSide + 1)) * squaresPerSide;
    ushort *indices = (ushort *)malloc(sizeof(ushort) * numIndices);

    int index = 0;
    
    for (int j = 0; j < squaresPerSide; ++j) {
        for (int i = 0; i <= squaresPerSide; ++i) {
            
            if ((j % 2) == 0) {
                
                ushort baseIndex = j * (squaresPerSide + 1) + i;
                
                indices[index + 0] = baseIndex;
                indices[index + 1] = baseIndex + (squaresPerSide + 1);
                
            } else {
                
                ushort baseIndex = j * (squaresPerSide + 1) + (squaresPerSide - i);
                
                indices[index + 0] = baseIndex + (squaresPerSide + 1);
                indices[index + 1] = baseIndex;
                
            }
            
            index += 2;
        }
    }
    
    Mesh result;
    
    result.setup(numVerts, verts, norms, cols, numIndices, indices);
    
    free(verts);
    free(cols);
    free(indices);
    
    return result;
}

Mesh buildAtmosphereMesh(int squaresPerSide, vec3f startPt, vec3f acrossDir, vec3f upDir) {
    
    float scaleFactor = 1.0 / squaresPerSide;
    
    vec3f uStep = scaleFactor * acrossDir;
    vec3f vStep = scaleFactor * upDir;
    
    int numVerts = (squaresPerSide + 1) * (squaresPerSide + 1);
    
    vec3f *verts = (vec3f *)malloc(sizeof(vec3f) * numVerts);
    vec3f *norms = (vec3f *)malloc(sizeof(vec3f) * numVerts);
    vec4f *cols = (vec4f *)malloc(sizeof(vec4f) * numVerts);
    
    int vertIndex = 0;
    for (int j = 0; j <= squaresPerSide; ++j) {
        
        for (int i = 0; i <= squaresPerSide; ++i) {
            
            vec3f &pt = verts[vertIndex];
            
            vec3f u = i * uStep;
            vec3f v = j * vStep;
            
            pt = startPt + u + v;
            
            //            // TODO: (George) Move the normalisation / height code out of here into a separate function
            //            // TODO: (George) Will require an "update mesh" function and stored arrays in mesh struct
            pt = normalize(pt);
            
            float height = 5.0;
            
            double multiplier = 0.025;
            float distFromCentre = planetRadius * (1.0 + height * multiplier);
            
            pt = distFromCentre * pt;
            cols[vertIndex] = vec4f(0.7, 0.75, 0.9, 0.3);
            
            vertIndex += 1;
        }
    }
    
    int numIndices = (2 * (squaresPerSide + 1)) * squaresPerSide;
    ushort *indices = (ushort *)malloc(sizeof(ushort) * numIndices);
    
    int index = 0;
    
    for (int j = 0; j < squaresPerSide; ++j) {
        for (int i = 0; i <= squaresPerSide; ++i) {
            
            if ((j % 2) == 0) {
                
                ushort baseIndex = j * (squaresPerSide + 1) + i;
                
                indices[index + 0] = baseIndex;
                indices[index + 1] = baseIndex + (squaresPerSide + 1);
                
            } else {
                
                ushort baseIndex = j * (squaresPerSide + 1) + (squaresPerSide - i);
                
                indices[index + 0] = baseIndex + (squaresPerSide + 1);
                indices[index + 1] = baseIndex;
                
            }
            
            index += 2;
        }
    }
    
    Mesh result;
    
    result.setup(numVerts, verts, norms, cols, numIndices, indices);
    
    free(verts);
    free(cols);
    free(indices);
    
    return result;
}

struct Model {
    static const int maxMeshes = 64;
    
    int numMeshes;
    Mesh meshes[maxMeshes];
    
    void render() {
        for (int i = 0; i < numMeshes; ++i) {
            meshes[i].draw();
        }
    }
};

Model planet;
Model atmosphere;

void setupGL() {
    
    int numSquaresPerSide = 128;
    
    planet.numMeshes = 6;
//    planet.meshes[0] = buildGridMesh(numSquaresPerSide, vec3f(1.0, -1.0, -1.0), vec3f(0.0, 2.0, 0.0), vec3f(0.0, 0.0, 2.0));
//    planet.meshes[1] = buildGridMesh(numSquaresPerSide, vec3f(-1.0, 1.0, -1.0), vec3f(0.0, -2.0, 0.0), vec3f(0.0, 0.0, 2.0));
//    planet.meshes[2] = buildGridMesh(numSquaresPerSide, vec3f(1.0, 1.0, -1.0), vec3f(-2.0, 0.0, 0.0), vec3f(0.0, 0.0, 2.0));
//    planet.meshes[3] = buildGridMesh(numSquaresPerSide, vec3f(-1.0, -1.0, -1.0), vec3f(2.0, 0.0, 0.0), vec3f(0.0, 0.0, 2.0));
//    planet.meshes[4] = buildGridMesh(numSquaresPerSide, vec3f(-1.0, -1.0, 1.0), vec3f(2.0, 0.0, 0.0), vec3f(0.0, 2.0, 0.0));
//    planet.meshes[5] = buildGridMesh(numSquaresPerSide, vec3f(-1.0, -1.0, -1.0), vec3f(2.0, 0.0, 0.0), vec3f(0.0, 2.0, 0.0));

    planet.meshes[0] = buildGridMesh(numSquaresPerSide, vec3f(1.0, -1.0, 1.0), vec3f(0.0, 2.0, 0.0), vec3f(0.0, 0.0, -2.0));
    planet.meshes[1] = buildGridMesh(numSquaresPerSide, vec3f(-1.0, 1.0, 1.0), vec3f(0.0, -2.0, 0.0), vec3f(0.0, 0.0, -2.0));
    planet.meshes[2] = buildGridMesh(numSquaresPerSide, vec3f(1.0, 1.0, 1.0), vec3f(-2.0, 0.0, 0.0), vec3f(0.0, 0.0, -2.0));
    planet.meshes[3] = buildGridMesh(numSquaresPerSide, vec3f(-1.0, -1.0, 1.0), vec3f(2.0, 0.0, 0.0), vec3f(0.0, 0.0, -2.0));
    planet.meshes[4] = buildGridMesh(numSquaresPerSide, vec3f(-1.0, 1.0, 1.0), vec3f(2.0, 0.0, 0.0), vec3f(0.0, -2.0, 0.0));
    planet.meshes[5] = buildGridMesh(numSquaresPerSide, vec3f(-1.0, -1.0, -1.0), vec3f(2.0, 0.0, 0.0), vec3f(0.0, 2.0, 0.0));
    
    numSquaresPerSide = 64;
    atmosphere.numMeshes = 6;
    atmosphere.meshes[0] = buildAtmosphereMesh(numSquaresPerSide, vec3f(1.0, -1.0, 1.0), vec3f(0.0, 2.0, 0.0), vec3f(0.0, 0.0, -2.0));
    atmosphere.meshes[1] = buildAtmosphereMesh(numSquaresPerSide, vec3f(-1.0, 1.0, 1.0), vec3f(0.0, -2.0, 0.0), vec3f(0.0, 0.0, -2.0));
    atmosphere.meshes[2] = buildAtmosphereMesh(numSquaresPerSide, vec3f(1.0, 1.0, 1.0), vec3f(-2.0, 0.0, 0.0), vec3f(0.0, 0.0, -2.0));
    atmosphere.meshes[3] = buildAtmosphereMesh(numSquaresPerSide, vec3f(-1.0, -1.0, 1.0), vec3f(2.0, 0.0, 0.0), vec3f(0.0, 0.0, -2.0));
    atmosphere.meshes[4] = buildAtmosphereMesh(numSquaresPerSide, vec3f(-1.0, 1.0, 1.0), vec3f(2.0, 0.0, 0.0), vec3f(0.0, -2.0, 0.0));
    atmosphere.meshes[5] = buildAtmosphereMesh(numSquaresPerSide, vec3f(-1.0, -1.0, -1.0), vec3f(2.0, 0.0, 0.0), vec3f(0.0, 2.0, 0.0));


    vertexShader = compileShader("Assets/Shaders/SimpleCameraVertex.glsl", ShaderTypeVertex);
//    geometryShader = compileShader("Assets/Shaders/SimpleCameraGeometry.glsl", ShaderTypeGeometry);
    fragmentShader = compileShader("Assets/Shaders/SimpleCameraFragment.glsl", ShaderTypeFragment);
    
    printf("Shaders: %u, %u, %u\n", vertexShader, geometryShader, fragmentShader);
    
    shaderProgram = linkShaders(vertexShader, geometryShader, fragmentShader);
    printf("Program: %u\n", shaderProgram);
    
    glUseProgram(shaderProgram);
    
}

struct PointOfView {

    vec3f position;
    vec3f direction, right, up;
    
    vec3f rotatePointAboutLine(vec3f p, double theta, vec3f p1, vec3f p2)
    {
        vec3f u,q1,q2;
        double d;
        
        /* Step 1 */
		q1 = p - p1;
        //q1.x = p.x - p1.x;
        //q1.y = p.y - p1.y;
        //q1.z = p.z - p1.z;

		u = p2 - p1;
        //u.x = p2.x - p1.x;
        //u.y = p2.y - p1.y;
        //u.z = p2.z - p1.z;
        
        u = normalize(u);
//        Normalise(&u);
        d = sqrt(u.y()*u.y() + u.z()*u.z());
        
        /* Step 2 */
        if (d != 0) {
            q2 = vec3f(q1.x(), q1.y() * u.z() / d - q1.z() * u.y() / d, q1.y() * u.y() / d + q1.z() * u.z() / d);
        } else {
            q2 = q1;
        }
        
        /* Step 3 */
        q1 = vec3f(q2.x() * d - q2.z() * u.x(), q2.y(), q2.x() * u.x() + q2.z() * d);
        
        /* Step 4 */
        q2 = vec3f(q1.x() * cos(theta) - q1.y() * sin(theta), q1.x() * sin(theta) + q1.y() * cos(theta), q1.z());
        
        /* Inverse of step 3 */
        q1 = vec3f(q2.x() * d + q2.z() * u.x(), q2.y(), - q2.x() * u.x() + q2.z() * d);
        
        /* Inverse of step 2 */
        if (d != 0) {
            q2 = vec3f(q1.x(), q1.y() * u.z() / d + q1.z() * u.y() / d, - q1.y() * u.y() / d + q1.z() * u.z() / d);
        } else {
            q2 = q1;
        }
        
        /* Inverse of step 1 */
		q1 = q2 + p1;
        //q1.x = q2.x + p1.x;
        //q1.y = q2.y + p1.y;
        //q1.z = q2.z + p1.z;
        return(q1);
    }
    
    void roll(double angle) {
        direction = normalize(direction);
        vec3f newUp = rotatePointAboutLine(up, angle, vec3f(0.0, 0.0, 0.0), direction);
        
        updateForUpVector(newUp);
    }
    
    void pitch(double angle) {
        right = normalize(right);
        
        direction = rotatePointAboutLine(direction, angle, vec3f(0.0, 0.0, 0.0), right);
        vec3f newUp = cross(right, direction);
        
        updateForUpVector(newUp);
    }
    
    void yaw(double angle) {
        up = normalize(up);
        
        direction = rotatePointAboutLine(direction, angle, vec3f(0.0, 0.0, 0.0), up);
        
        updateForUpVector(up);
    }
    
    void move(double distance) {
        direction = normalize(direction);

        vec3f offset = distance * direction;
        
        position = position + offset;
    }
    
    void updateForUpVector(vec3f referenceUp) {
        direction = normalize(direction);
        
        right = cross(direction, referenceUp);
        right = normalize(right);
        
        up = cross(right, direction);
        up = normalize(up);

    }
    
    void getCameraMatrix(Mat4x4 &viewMatrix) {
        
        viewMatrix[0]  = right.x();
        viewMatrix[4]  = right.y();
        viewMatrix[8]  = right.z();
        viewMatrix[12] = 0.0f;
        
        viewMatrix[1]  = up.x();
        viewMatrix[5]  = up.y();
        viewMatrix[9]  = up.z();
        viewMatrix[13] = 0.0f;
        
        viewMatrix[2]  = -direction.x();
        viewMatrix[6]  = -direction.y();
        viewMatrix[10] = -direction.z();
        viewMatrix[14] =  0.0f;
        
        viewMatrix[3]  = 0.0f;
        viewMatrix[7]  = 0.0f;
        viewMatrix[11] = 0.0f;
        viewMatrix[15] = 1.0f;
        
        Mat4x4 aux;
        setTranslationMatrix(aux, -position.x(), -position.y(), -position.z());
        
        multMatrix(viewMatrix, aux);
    }
};


struct InputState {
    InputState() {
        forwardActive = false;
        backActive = false;
        leftActive = false;
        rightActive = false;
        yawLeftActive = false;
        yawRightActive = false;
        upActive = false;
        downActive = false;
        shouldQuit = false;
    }
    
    bool forwardActive;
    bool backActive;
    bool leftActive;
    bool rightActive;
    bool yawLeftActive;
    bool yawRightActive;
    bool upActive;
    bool downActive;
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
                    case SDLK_w:
                        inputs.upActive = true;
                        break;
                    case SDLK_s:
                        inputs.downActive = true;
                        break;
                    case SDLK_a:
                        inputs.leftActive = true;
                        break;
                    case SDLK_d:
                        inputs.rightActive = true;
                        break;
                    case SDLK_UP:
                        inputs.forwardActive = true;
                        break;
                    case SDLK_DOWN:
                        inputs.backActive = true;
                        break;
                    case SDLK_LEFT:
                        inputs.yawLeftActive = true;
                        break;
                    case SDLK_RIGHT:
                        inputs.yawRightActive = true;
                        break;
                    default:
                        break;
                }
                break;
                
            case SDL_KEYUP:
                /* Check the SDLKey values and move change the coords */
                switch( event.key.keysym.sym ) {
                    case SDLK_w:
                        inputs.upActive = false;
                        break;
                    case SDLK_s:
                        inputs.downActive = false;
                        break;
                    case SDLK_a:
                        inputs.leftActive = false;
                        break;
                    case SDLK_d:
                        inputs.rightActive = false;
                        break;
                    case SDLK_UP:
                        inputs.forwardActive = false;
                        break;
                    case SDLK_DOWN:
                        inputs.backActive = false;
                        break;
                    case SDLK_LEFT:
                        inputs.yawLeftActive = false;
                        break;
                    case SDLK_RIGHT:
                        inputs.yawRightActive = false;
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

struct Ship {
    double rollAcceleration;
    double rollVelocity;
    double rollMultiplier;
    
    double pitchAcceleration;
    double pitchVelocity;
    double pitchMultiplier;
    
    double yawAcceleration;
    double yawVelocity;
    double yawMultiplier;
    
    double acceleration;
    double velocity;
    double velocityMultiplier;
    
    PointOfView view;
    
    Ship(const PointOfView &_view) {
        
        view = _view;
        
        rollAcceleration = 0.5;
        rollVelocity = 0.0;
        rollMultiplier = 0.3;
        
        pitchAcceleration = 0.1;
        pitchVelocity = 0.0;
        pitchMultiplier = 0.2;
        
        yawAcceleration = 0.1;
        yawVelocity = 0.0;
        yawMultiplier = 0.2;
        
        acceleration = 0.1;
        velocity = 0.0;
        velocityMultiplier = 1000.0;
    }
    
    void moveShip(const InputState &inputs, double dt, vec3f planetPos, double planetRad) {
        
        if (inputs.leftActive) {
            rollVelocity -= rollAcceleration * dt;
        }
        
        if (inputs.rightActive) {
            rollVelocity += rollAcceleration * dt;
        }
        
        if (!inputs.leftActive && !inputs.rightActive) {
            rollVelocity *= 0.95;
            
            if (fabs(rollVelocity) < 0.05) {
                rollVelocity = 0.0;
            }
        }
        
        if (rollVelocity < -1.0) {
            rollVelocity = -1.0;
        } else if (rollVelocity > 1.0) {
            rollVelocity = 1.0;
        }
        
        if (inputs.upActive) {
            pitchVelocity -= pitchAcceleration * dt;
        }
        
        if (inputs.downActive) {
            pitchVelocity += pitchAcceleration * dt;
        }
        
        if (!inputs.upActive && !inputs.downActive) {
            pitchVelocity *= 0.9;
        }
        
        if (pitchVelocity < -1.0) {
            pitchVelocity = -1.0;
        } else if (pitchVelocity > 1.0) {
            pitchVelocity = 1.0;
        }
        
        if (inputs.yawLeftActive) {
            yawVelocity += yawAcceleration * dt;
        }
        
        if (inputs.yawRightActive) {
            yawVelocity -= yawAcceleration * dt;
        }
        
        if (!inputs.yawLeftActive && !inputs.yawRightActive) {
            yawVelocity *= 0.9;
        }
        
        if (yawVelocity < -1.0) {
            yawVelocity = -1.0;
        } else if (yawVelocity > 1.0) {
            yawVelocity = 1.0;
        }
        
        if (inputs.forwardActive) {
            velocity += acceleration * dt;
        }
        
        if (inputs.backActive) {
            velocity -= acceleration * dt;
        }
        
        if (!inputs.forwardActive && !inputs.backActive) {
            velocity *= 0.95;
        }
        
        if (velocity < -1.0) {
            velocity = -1.0;
        } else if (velocity > 1.0) {
            velocity = 1.0;
        }
        
        view.roll(rollVelocity * rollMultiplier);
        view.pitch(pitchVelocity * pitchMultiplier);
        view.yaw(yawVelocity * yawMultiplier);
        
        velocityMultiplier = length(view.position - planetPos) - planetRad;
        
        if (velocityMultiplier < 10.0) {
            velocityMultiplier = 10.0;
        }
        
        view.move(velocity * dt * velocityMultiplier);
    }
};


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
    
#if WINDOWS
	glewInit();
#endif

    SDL_GL_SetSwapInterval(1);
}

void deleteWindowAndContext(SDL_Window *window, SDL_GLContext &context) {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(window);
}

void runMainLoop(SDL_Window *window) {
    
    setupGL();
    
   double t = 0.0;
   const double dt = 1.0 / 60.0;
   
   double currentTime = systemTimeInSeconds();
   double accumulator = 0.0;
    
   PointOfView view;

   vec3f from(5.0, 0.0, 0.2);
   vec3f dir = -from;// (-from.x, -from.y, -from.z);
   vec3f up(0.0, 0.0, 1.0);

   from = planetRadius * from;
   view.position = from;
   view.direction = dir;
   view.updateForUpVector(up);

   Ship ship(view);

   InputState inputs;
    
    // Basic run loop from http://gafferongames.com/game-physics/fix-your-timestep/
    // TODO: (George) Final interpolation between states for super smoothness
    
    float y = 3.0;
    vec4f v = normalize( vec4f(1,2,3,4) + y );
    float z = v.z();
    
    printf("Z = %1.2f\n", z);
    
    while (!inputs.shouldQuit)
    {
       double newTime = systemTimeInSeconds();
       double frameTime = newTime - currentTime;
       currentTime = newTime;
       
        // GATHER USER INPUT (PLUS NETWORK INPUT?) - START
        inputs = updateInputs(inputs);
        
        // GATHER USER INPUT (PLUS NETWORK INPUT?) - END

       accumulator += frameTime;
       
       while ( accumulator >= dt )
       {
           
           // GAME STATE UPDATE - START
           //            integrate( state, t, dt );
           
           ship.moveShip(inputs, dt, vec3f(0.0, 0.0, 0.0), planetRadius);
           
           // GAME STATE UPDATE - END
           
           accumulator -= dt;
           t += dt;
       }
        
        // GAME STATE RENDER - START
        

	   glClearColor(0.05, 0.0, 0.1, 1.0);
	   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

       glEnable(GL_DEPTH_TEST);
       glDepthFunc(GL_LESS);
       CHECK_GL_ERRORS();
       
       Mat4x4 projectionMatrix;
       Mat4x4 viewMatrix;
       
       buildProjectionMatrix(projectionMatrix, 45.0, 4.0 / 3.0, 10.0, 50000.0);
       
       ship.view.getCameraMatrix(viewMatrix);
       
       multMatrix(projectionMatrix, viewMatrix);
       
       glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "mvpmatrix"), 1, GL_FALSE, projectionMatrix);
       CHECK_GL_ERRORS();
       
       glUseProgram(shaderProgram);
       CHECK_GL_ERRORS();
       
       glEnable(GL_CULL_FACE);
       glCullFace(GL_BACK);
       
       glDisable(GL_BLEND);
       glEnable(GL_DEPTH_TEST);
       planet.render();
       
       //glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
       //glEnable(GL_BLEND);
       //
       //glCullFace(GL_FRONT);
       //atmosphere.render();
       //
       //// TODO: (George) Only if we're outside the atmosphere...
       //glCullFace(GL_BACK);
       //atmosphere.render();
        
        SDL_GL_SwapWindow(window);
       CHECK_GL_ERRORS();
        
        // GAME STATE RENDER - END
    }
    
    // Clean up GL
   glUseProgram(0);
   glDisableVertexAttribArray(0);
   glDisableVertexAttribArray(1);
    
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