#ifndef _MeshUtils_h_
#define _MeshUtils_h_

struct Mesh {
	GLuint vao = 0;
	GLuint vertexBuffer[3] = { 0, 0, 0 };
	GLuint indexBuffer = 0;
	int numVertices = 0;
	int numIndices = 0;

	void setup(int numVerts, vec3f *verts, vec3f *norms, vec4f *colors, int numInds, ushort *indices);
	void draw();
};

#endif // _MeshUtils_h_
