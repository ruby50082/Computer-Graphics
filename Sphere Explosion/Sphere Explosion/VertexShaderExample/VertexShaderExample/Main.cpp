#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <cmath>
#include <iostream>
#include <stdlib.h>
#include <time.h>
#define PI 3.14159265359

using namespace std;

struct VertexAttribute {
	GLfloat position[3];
	GLfloat normal[3];
	GLfloat texcoord[2];
};

int windowSize[2] = { 1000, 1000 };

GLuint program, program2;
GLuint vboName, vboName2, vao, vao2;

GLuint textObj[4];
int slices = 36, stacks = 18;
float X = 15;
float rotat = 0, rotat_2 = 0, revol = 0, offset = 0, delta = 0.05;
GLfloat turn[3] = { 1.0, 1.0, 1.0 };
bool paused = false;
int colli=0;
float tt = 0.0;

VertexAttribute *Sphere(int slice, int slice_b, int slice_e, int stack, int stack_b, int stack_e);
void RotationMatrix(GLfloat *m, float angle, float x, float y, float z);
void TranslationMatrix(GLfloat *m, float x, float y, float z);
void display();
void reshape(GLsizei w, GLsizei h);
void init();
void init_texture();
void idle();
void keyboard(unsigned char key, int x, int y);
void Multiply(GLfloat M[16], GLfloat N[16], GLfloat Result[16]);
void Multiply2(GLfloat M[16], GLfloat N[4], GLfloat Result[4]);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("hw4");
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

	GLuint vert2 = createShader("Shaders/comet.vert", "vertex");
	GLuint geom2 = createShader("Shaders/comet.geom", "geometry");
	GLuint frag2 = createShader("Shaders/comet.frag", "fragment");
	program2 = createProgram(vert2, geom2, frag2);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	VertexAttribute *vertices;
	vertices = Sphere(slices, 0, slices, stacks, 0, stacks);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * slices * stacks * 6, vertices, GL_DYNAMIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	glGenVertexArrays(1, &vao2);
	glBindVertexArray(vao2);

	glGenBuffers(1, &vboName2);
	glBindBuffer(GL_ARRAY_BUFFER, vboName2);
	VertexAttribute *vertices2;
	vertices2 = Sphere(slices,0,slices, stacks, 0,stacks);
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * slices * stacks * 6, vertices2, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);
}

void display()
{
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glViewport(0, 0, windowSize[0], windowSize[1]);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 20.0f, 20.0f,	// eye
		0.0f, 0.0f, 0.0f,	// center
		0.0f, 1.0f, 0.0f);// up

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	GLfloat rotmtx[16];
	GLfloat sltmtx[16];
	GLfloat rotmtx3[16];
	GLfloat revmtx[16];
	GLfloat transmtx_e[16];
	GLfloat transmtx_c[16];
	GLfloat identmtx[16];
	GLfloat lightPos[3] = { 5.0, 30.0, 30.0 };
	GLfloat viewPos[3] = { 0.0, 0.0, 20.0 };
	GLfloat t[3] = { tt, 0, 0 };
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx);
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx);
	RotationMatrix(rotmtx, -rotat * PI / 180, 0, 1, 0);
	RotationMatrix(sltmtx, 23.5 * PI / 180, 0, 0, 1);
	RotationMatrix(rotmtx3, -rotat_2 * PI / 180, 0, 1, 0); //light
	RotationMatrix(revmtx, -revol * PI / 180, 0, 1, 0);
	TranslationMatrix(transmtx_e, -3, 0, 0);
	TranslationMatrix(transmtx_c, 14 - offset, 0, 0);
	TranslationMatrix(identmtx, 0, 0, 0);

	GLfloat center_e[4], center_c[4], tmppp[4];
	center_e[0] = 0; center_e[1] = 0; center_e[2] = 0; center_e[3] = 1;
	center_c[0] = 14 - offset; center_c[1] = 0; center_c[2] = 0, center_c[3] = 1;
	GLfloat tmp[16], tmp2[16], center_e_tran[16], center_c_tran[16];


	GLfloat dx = center_e[0] - center_c[0];
	GLfloat dy = center_e[1] - center_c[1];
	GLfloat dz = center_e[2] - center_c[2];
	GLfloat d = sqrt(dx * dx + dy * dy + dz * dz);
	GLfloat r = (1 + 1);
	if (d < r) {
		//cout << "collision!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		delta = -0.03;
		colli = 1;
	}


	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint rotmatLoc = glGetUniformLocation(program, "Rotation");
	GLint sltmatLoc = glGetUniformLocation(program, "Slant");
	GLint rotmat3Loc = glGetUniformLocation(program, "Rotation3");
	GLint revmatLoc = glGetUniformLocation(program, "Revolution");
	GLint transmatLoc = glGetUniformLocation(program, "Translation");
	GLint lightPosLoc = glGetUniformLocation(program, "lightPos");
	GLint viewPosLoc = glGetUniformLocation(program, "viewPos");
	GLint texLoc1 = glGetUniformLocation(program, "Texture1");
	GLint texLoc2 = glGetUniformLocation(program, "Texture2");
	GLint texLoc3 = glGetUniformLocation(program, "Texture3");
	GLint turnLoc = glGetUniformLocation(program, "turn");

	GLint pmatLoc_c = glGetUniformLocation(program2, "Projection");
	GLint mmatLoc_c = glGetUniformLocation(program2, "ModelView");
	GLint transmatLoc_c = glGetUniformLocation(program2, "Translation_c");
	GLint lightPosLoc_c = glGetUniformLocation(program2, "lightPos");
	GLint viewPosLoc_c = glGetUniformLocation(program2, "viewPos");
	GLint texLoc4 = glGetUniformLocation(program2, "Texture4");
	GLint turnLoc_c = glGetUniformLocation(program2, "turn");
	GLint timeLoc_c = glGetUniformLocation(program, "Time");

	glUseProgram(program);

	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);
	if(colli==0)	glUniformMatrix4fv(rotmatLoc, 1, GL_FALSE, rotmtx);
	else			glUniformMatrix4fv(rotmatLoc, 1, GL_FALSE, identmtx);
	glUniformMatrix4fv(sltmatLoc, 1, GL_FALSE, sltmtx);
	glUniformMatrix4fv(revmatLoc, 1, GL_FALSE, revmtx);
	glUniformMatrix4fv(rotmat3Loc, 1, GL_FALSE, rotmtx3);
	glUniformMatrix4fv(transmatLoc, 1, GL_FALSE, transmtx_e);

	glUniform3fv(lightPosLoc, 1, lightPos);
	glUniform3fv(viewPosLoc, 1, viewPos);
	glUniform3fv(turnLoc, 1, turn);
	glUniform3fv(timeLoc_c, 1, t);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textObj[0]);
	glUniform1i(texLoc1, 0);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textObj[1]);
	glUniform1i(texLoc2, 1);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textObj[2]);
	glUniform1i(texLoc3, 2);


	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, slices * stacks * 6);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindTexture(GL_TEXTURE_2D, NULL);
	glBindVertexArray(0);
	glUseProgram(0);


	glUseProgram(program2);

	glUniformMatrix4fv(pmatLoc_c, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc_c, 1, GL_FALSE, mmtx);
	glUniformMatrix4fv(transmatLoc_c, 1, GL_FALSE, transmtx_c);

	glUniform3fv(lightPosLoc_c, 1, lightPos);
	glUniform3fv(viewPosLoc_c, 1, viewPos);
	glUniform3fv(turnLoc_c, 1, turn);
	glUniform3fv(timeLoc_c, 1, t);

	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textObj[3]);

	glBindVertexArray(vao2);
	glUniformMatrix4fv(transmatLoc, 1, GL_FALSE, transmtx_c);
	glUniform1i(texLoc4, 3);
	glDrawArrays(GL_TRIANGLES, 0, slices * stacks * 6);

	glBindTexture(GL_TEXTURE_2D, NULL);
	glBindVertexArray(0);
	glUseProgram(0);


	glPopMatrix();
	glutSwapBuffers();

}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

VertexAttribute *Sphere(int slice, int slice_b, int slice_e, int stack, int stack_b, int stack_e) {

	VertexAttribute *vertices;
	vertices = new VertexAttribute[(slice_e-slice_b) * (stack_e-stack_b) * 6];
	VertexAttribute v[4];
	int direction[4][2] = { { 0, 0 },{ 1, 0 },{ 0, 1 },{ 1, 1 } };
	int index[6] = { 0, 1, 2, 1, 3, 2 };
	float slice_step = 2 * PI / slice, stack_step = PI / stack;
	int n = 0;
	for (int i = slice_b; i < slice_e; i++) {
		for (int j = stack_b; j < stack_e; j++) {
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

	pImage_E = FreeImage_Load(FreeImage_GetFileType("comet.png", 0), "comet.png");
	p32BitsImage_E = FreeImage_ConvertTo32Bits(pImage_E);
	iWidth_E = FreeImage_GetWidth(p32BitsImage_E);
	iHeight_E = FreeImage_GetHeight(p32BitsImage_E);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, textObj[3]);
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

void TranslationMatrix(GLfloat *m, float x, float y, float z) {

	m[0] = 1.0f; m[4] = 0.0f; m[8] = 0.0f;  m[12] = x;
	m[1] = 0.0f; m[5] = 1.0f; m[9] = 0.0f;  m[13] = y;
	m[2] = 0.0f; m[6] = 0.0f; m[10] = 1.0;  m[14] = z;
	m[3] = 0.0f; m[7] = 0.0f; m[11] = 0.0f;	m[15] = 1.0f;
}

void idle() {
	if (!paused) {
		rotat = fmod((rotat + X), 360.0f);
		revol = fmod((revol + X / 10.0f), 360.0f);
		offset = fmod((offset + delta), 360.0f);
		if(colli)	tt+=0.02;
		//rotat_2 = fmod((rotat_2 + X/10.0f), 360.0f);
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

void Multiply(GLfloat M[16], GLfloat N[16], GLfloat Result[16])
{
	for (int I = 0; I < 4; ++I)
	{
		for (int J = 0; J < 4; ++J)
		{
			float SumElements = 0.0f;
			for (int K = 0; K < 4; ++K)
			{
				SumElements += M[4 * K + I] * N[4 * J + K];
			}
			Result[4 * J + I] = SumElements;
		}
	}
}
void Multiply2(GLfloat M[16], GLfloat N[4], GLfloat Result[4])
{
	for (int I = 0; I < 4; ++I)
	{
		float SumElements = 0.0f;
		for (int J = 0; J < 4; ++J)
		{
			SumElements += M[4 * J + I] * N[J];
		}
		Result[I] = SumElements;
	}
}

