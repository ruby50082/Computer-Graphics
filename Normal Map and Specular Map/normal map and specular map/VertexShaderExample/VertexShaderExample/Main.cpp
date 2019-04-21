#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <cmath>
#include <iostream>
#define PI 3.14159265359

using namespace std;

struct VertexAttribute {
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};

int windowSize[2] = { 600, 500 };

GLuint program;
GLuint vboName;

GLuint textObj[3];
int slices = 360, stacks = 180;
float X = 1.0;
float rotat = 0, rotat_2 = 0;
GLfloat turn[3] = { 1.0, 1.0, 1.0 };
bool paused = false;

VertexAttribute *Sphere(int slice, int stack);
void RotationMatrix(GLfloat *m, float angle, float x, float y, float z);
void display();
void reshape(GLsizei w, GLsizei h);
void init();
void init_texture();
void idle();
void keyboard(unsigned char key, int x, int y);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("hw3");
	glewInit();

	init();
	glGenTextures(3, textObj);
	init_texture();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();

	return 0;
}

void init() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint geom = createShader("Shaders/example.geom", "geometry");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, geom, frag);

	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	VertexAttribute *vertices;
	vertices = Sphere(slices, stacks);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * slices * stacks * 6, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void display()
{
	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);
	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	//lights();
	gluLookAt(0.0f, 0.0f, 3.0f,	// eye
			  0.0f, 0.0f, 0.0f,	// center
			  0.0f, 1.0f, 0.0f);// up

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	GLfloat rotmtx1[16];
	GLfloat rotmtx2[16];
	GLfloat rotmtx3[16];
	GLfloat lightPos[3] = { -3.0, 0.0, 0.0 };
	GLfloat viewPos[3] = { 0.0, 0.0, 3.0 };
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	RotationMatrix(rotmtx1, -rotat * PI / 180, 0, 1, 0);
	RotationMatrix(rotmtx2, 23.5 * PI / 180, 0, 0, 1);
	RotationMatrix(rotmtx3, -rotat_2 * PI / 180, 0, 1, 0); //light

	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint rotmat1Loc = glGetUniformLocation(program, "Rotation1");
	GLint rotmat2Loc = glGetUniformLocation(program, "Rotation2");
	GLint rotmat3Loc = glGetUniformLocation(program, "Rotation3");
	GLint lightPosLoc = glGetUniformLocation(program, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(program, "viewPos");
	GLint texLoc1 = glGetUniformLocation(program, "Texture1");
	GLint texLoc2 = glGetUniformLocation(program, "Texture2");
	GLint texLoc3 = glGetUniformLocation(program, "Texture3");
	GLint turnLoc = glGetUniformLocation(program, "turn");

	glUseProgram(program);

	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	glUniformMatrix4fv(rotmat1Loc, 1, GL_FALSE, rotmtx1);
	glUniformMatrix4fv(rotmat2Loc, 1, GL_FALSE, rotmtx2);
	glUniformMatrix4fv(rotmat3Loc, 1, GL_FALSE, rotmtx3);
	glUniform3fv(lightPosLoc, 1, lightPos);
	glUniform3fv(viewPosLoc, 1, viewPos);
	glUniform3fv(turnLoc, 1, turn);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textObj[0]);
	glUniform1i(texLoc1, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textObj[1]);
	glUniform1i(texLoc2, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textObj[2]);
	glUniform1i(texLoc3, 2);

	glDrawArrays(GL_TRIANGLES, 0, slices * stacks * 6);
	glBindTexture(GL_TEXTURE_2D, NULL);

	glUseProgram(0);

	glPopMatrix();
	glutSwapBuffers();

}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

VertexAttribute *Sphere(int slice, int stack) {
	
	VertexAttribute *vertices;
	vertices = new VertexAttribute[slice * stack * 6];
	VertexAttribute v[4];
	int direction[4][2] = { { 0, 0 },{ 1, 0 },{ 0, 1 },{ 1, 1 } };
	int index[6] = {0, 1, 2, 1, 3, 2 };
	float slice_step = 2 * PI / slice, stack_step = PI / stack;
	int n = 0;
	for (int i = 0; i < slice; i++) {
		for (int j = 0; j < stack; j++) {
			for (int k = 0; k < 4; k++) {
				v[k].position[0] = v[k].normal[0] = sin((j + direction[k][1]) * stack_step) * cos((i + direction[k][0]) * slice_step);
				v[k].position[1] = v[k].normal[1] = cos((j + direction[k][1]) * stack_step);
				v[k].position[2] = v[k].normal[2] = sin((j + direction[k][1]) * stack_step) * sin((i + direction[k][0]) * slice_step);
				v[k].texcoord[0] = 1 - float(i + direction[k][0]) / slice;
				v[k].texcoord[1] = 1 - float(j + direction[k][1]) / stack;
			}
			for (int k = 0; k < 6; k++) {
				vertices[n].position[0] = vertices[n].normal[0] = v[index[k]].position[0];
				vertices[n].position[1] = vertices[n].normal[1] = v[index[k]].position[1];
				vertices[n].position[2] = vertices[n].normal[2] = v[index[k]].position[2];
				vertices[n].texcoord[0] = v[index[k]].texcoord[0];
				vertices[n].texcoord[1] = v[index[k]].texcoord[1];
				vertices[n].texcoord[2] = v[index[k]].texcoord[2];
				n++;
			}
		}
	}

	return vertices;
}

void init_texture()
{
	FIBITMAP* pImage_E = FreeImage_Load(FreeImage_GetFileType("earth_texture_map.jpg", 0), "earth_texture_map.jpg");
	FIBITMAP* p32BitsImage_E = FreeImage_ConvertTo32Bits(pImage_E);
	int iWidth_E = FreeImage_GetWidth(p32BitsImage_E);
	int iHeight_E = FreeImage_GetHeight(p32BitsImage_E);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textObj[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth_E, iHeight_E, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage_E));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	pImage_E = FreeImage_Load(FreeImage_GetFileType("earth_normal_map.tif", 0), "earth_normal_map.tif");
	p32BitsImage_E = FreeImage_ConvertTo32Bits(pImage_E);
	iWidth_E = FreeImage_GetWidth(p32BitsImage_E);
	iHeight_E = FreeImage_GetHeight(p32BitsImage_E);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textObj[1]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth_E, iHeight_E, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage_E));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	pImage_E = FreeImage_Load(FreeImage_GetFileType("earth_specular_map.tif", 0), "earth_specular_map.tif");
	p32BitsImage_E = FreeImage_ConvertTo32Bits(pImage_E);
	iWidth_E = FreeImage_GetWidth(p32BitsImage_E);
	iHeight_E = FreeImage_GetHeight(p32BitsImage_E);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textObj[2]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth_E, iHeight_E, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage_E));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
}

void RotationMatrix(GLfloat *m, float angle, float x, float y, float z) {

	if (x == 1) {
		m[0] = 1.0f; m[4] = 0.0f;		 m[8] = 0.0f;		  m[12] = 0.0f;
		m[1] = 0.0f; m[5] = cosf(angle); m[9] = -sinf(angle); m[13] = 0.0f;
		m[2] = 0.0f; m[6] = sinf(angle); m[10] = cosf(angle); m[14] = 0.0f;
		m[3] = 0.0f; m[7] = 0.0f;		 m[11] = 0.0f;		  m[15] = 1.0f;
	}
	else if (y == 1) {
		m[0] = cosf(angle); m[4] = 0.0f; m[8] = -sinf(angle); m[12] = 0.0f;
		m[1] = 0.0f;		m[5] = 1.0f; m[9] = 0.0f;		  m[13] = 0.0f;
		m[2] = sinf(angle); m[6] = 0.0f; m[10] = cosf(angle); m[14] = 0.0f;
		m[3] = 0.0f;		m[7] = 0.0f; m[11] = 0.0f;		  m[15] = 1.0f;
	}
	else if (z == 1) {
		m[0] = cosf(angle);	m[4] = -sinf(angle); m[8] = 0.0f;  m[12] = 0.0f;
		m[1] = sinf(angle); m[5] = cosf(angle);	 m[9] = 0.0f;  m[13] = 0.0f;
		m[2] = 0.0f;		m[6] = 0.0f;		 m[10] = 1.0f; m[14] = 0.0f;
		m[3] = 0.0f;		m[7] = 0.0f;		 m[11] = 0.0f; m[15] = 1.0f;
	}
}

void idle() {
	if (!paused) {
		rotat = fmod((rotat + X), 360.0f);
		rotat_2 = fmod((rotat_2 + X/10.0f), 360.0f);
	}
	glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y) {
	switch (key) {
	case '1':
		turn[0] = !turn[0];
		break;

	case '2':
		turn[1] = !turn[1];
		break;

	case '3':
		turn[2] = !turn[2];
		break;

	case 'P':case 'p':
		paused = !paused;
		break;
	}
}
