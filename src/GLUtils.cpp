#include "xpt.h"
#include "GLUtils.h"

uint compileShader(const char *shaderFilename, ShaderType shaderType) {
    GLchar *shaderSource;
    GLuint shader;
    
    shaderSource = fileToCharArray(shaderFilename);
    
    switch (shaderType) {
        case ShaderTypeVertex:
        shader = glCreateShader(GL_VERTEX_SHADER);
        break;
        
        case ShaderTypeFragment:
        shader = glCreateShader(GL_FRAGMENT_SHADER);
        break;
        
        case ShaderTypeGeometry:
        shader = glCreateShader(GL_GEOMETRY_SHADER);
        break;
        
        default:
        return 0;
    }
    CHECK_GL_ERRORS();
    
    glShaderSource(shader, 1, (const GLchar**)&shaderSource, 0);
    CHECK_GL_ERRORS();
    glCompileShader(shader);
    CHECK_GL_ERRORS();
    
    free(shaderSource);
    
    int isShaderCompiled;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &isShaderCompiled);
    
    if (isShaderCompiled == 0) {
        int maxLength;
        
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
        char *infoLog = (char *)malloc(maxLength);
        
        glGetShaderInfoLog(shader, maxLength, &maxLength, infoLog);
        
        printf("Error compiling shader: %s\n", shaderFilename);
        printf("Error: %s\n", infoLog);
        free(infoLog);
        return 0;
    }
    
    return shader;
}

uint linkShaders(uint vertexShader, uint geometryShader, uint fragmentShader) {
    GLuint shaderProgram = glCreateProgram();
    
    /* Attach our shaders to our program */
    glAttachShader(shaderProgram, vertexShader);
    CHECK_GL_ERRORS();
    //    glAttachShader(shaderProgram, geometryShader);
    glAttachShader(shaderProgram, fragmentShader);
    CHECK_GL_ERRORS();
    
    // TODO: (George) Handle this in a more re-usable way
    glBindAttribLocation(shaderProgram, 0, "in_Position");
    CHECK_GL_ERRORS();
    glBindAttribLocation(shaderProgram, 1, "in_Normal");
    CHECK_GL_ERRORS();
    glBindAttribLocation(shaderProgram, 2, "in_Color");
    CHECK_GL_ERRORS();
    
    glLinkProgram(shaderProgram);
    CHECK_GL_ERRORS();

    int isLinked;
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &isLinked);
    if (isLinked == 0) {
        int maxLength;
        glGetProgramiv(shaderProgram, GL_INFO_LOG_LENGTH, &maxLength);
        
        char *infoLog = (char *)malloc(maxLength);
        
        glGetProgramInfoLog(shaderProgram, maxLength, &maxLength, infoLog);
        
        free(infoLog);
        
        return 0;
    }
    
    return shaderProgram;
}

// ----------------------------------------------------
// MATRIX STUFF
//

// sets the square matrix mat to the identity matrix,
// size refers to the number of rows (or columns)
void setIdentityMatrix(Mat4x4 &mat) {

	int size = 4;

	// fill matrix with 0s
	for (int i = 0; i < size * size; ++i)
		mat[i] = 0.0f;

	// fill diagonal with 1s
	for (int i = 0; i < size; ++i)
		mat[i + i * size] = 1.0f;
}

//
// a = a * b;
//
void multMatrix(Mat4x4 &a, Mat4x4 b) {

	Mat4x4 res;

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			res[j * 4 + i] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				res[j * 4 + i] += a[k * 4 + i] * b[j * 4 + k];
			}
		}
	}
	memcpy(a, res, sizeof(Mat4x4));

}

// Defines a transformation matrix mat with a translation
void setTranslationMatrix(Mat4x4 &mat, float x, float y, float z) {

	setIdentityMatrix(mat);
	mat[12] = x;
	mat[13] = y;
	mat[14] = z;
}

//float projMatrix[16];
//float viewMatrix[16];

// ----------------------------------------------------
// Projection Matrix
//

void buildProjectionMatrix(Mat4x4 &projMatrix, float fov, float ratio, float nearP, float farP) {

	float f = 1.0f / (float)(tan(fov * (M_PI / 360.0)));

	setIdentityMatrix(projMatrix);

	projMatrix[0] = f / ratio;
	projMatrix[1 * 4 + 1] = f;
	projMatrix[2 * 4 + 2] = (farP + nearP) / (nearP - farP);
	projMatrix[3 * 4 + 2] = (2.0f * farP * nearP) / (nearP - farP);
	projMatrix[2 * 4 + 3] = -1.0f;
	projMatrix[3 * 4 + 3] = 0.0f;
}

// ----------------------------------------------------
// View Matrix
//
// note: it assumes the camera is not tilted,
// i.e. a vertical up vector (remmeber gluLookAt?)
//

void setCamera(Mat4x4 &viewMatrix, vec3f pos, vec3f lookAt) {

	vec3f dir, up(0.0, 0.0, 1.0);

	dir = lookAt - pos;
	//    dir[0] =  (lookAt[0] - pos[0]);
	//    dir[1] =  (lookAt[1] - pos[1]);
	//    dir[2] =  (lookAt[2] - pos[2]);
	dir = normalize(dir);

	vec3f right = cross(dir, up);
	right = normalize(right);

	up = cross(right, dir);
	up = normalize(up);

	float aux[16];

	//    setIdentityMatrix(viewMatrix, 4);
	viewMatrix[0] = right.x();
	viewMatrix[4] = right.y();
	viewMatrix[8] = right.z();
	viewMatrix[12] = 0.0f;

	viewMatrix[1] = up.x();
	viewMatrix[5] = up.y();
	viewMatrix[9] = up.z();
	viewMatrix[13] = 0.0f;

	viewMatrix[2] = -dir.x();
	viewMatrix[6] = -dir.y();
	viewMatrix[10] = -dir.z();
	viewMatrix[14] = 0.0f;

	viewMatrix[3] = 0.0f;
	viewMatrix[7] = 0.0f;
	viewMatrix[11] = 0.0f;
	viewMatrix[15] = 1.0f;

	setTranslationMatrix(aux, -pos.x(), -pos.y(), -pos.z());

	multMatrix(viewMatrix, aux);
}