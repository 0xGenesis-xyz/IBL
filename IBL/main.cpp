//
//  main.cpp
//  MiddleDemo
//
#include "CGTexture.h"

char root[64] = "/Users/Sylvanus/Sylvanus's Library/Studio/SRTP/IBL/IBL/";
char path[128];
float mat[4 * 4]; // rotation matrix

// This example displays one of the following shapes
#define NUM_SHAPES 5
typedef enum {
	SHAPE_TEAPOT = 1,
	SHAPE_TORUS,
	SHAPE_DRAGON,
	SHAPE_SKULL,
	SHAPE_GARGO,
	DRAW_EN
} Shape;

Shape g_CurrentShape = SHAPE_TEAPOT;

#define NUM_SHADERS 2
typedef enum {
	SHADER_PHONG = 1,
	SHADER_COOKTORRORENCE
} Shader;

Shader currentShader = SHADER_COOKTORRORENCE;

// Shapes scale
float g_Zoom = 1.0f;
// Shape orientation (stored as a quaternion)
float g_Rotation[] = { 0.0f, 0.0f, 0.0f, 1.0f };
// Auto rotate
int g_AutoRotate = 0;
int g_RotateTime = 0;
float g_RotateStart[] = { 0.0f, 0.0f, 0.0f, 1.0f };

float lightAmbient[] = { 0.9f, 0.9f, 0.9f, 1.0f };
float lightDiffuse[] = { 1.0f, 1.0f, 1.0f, 1.0f };
float lightDirection[] = { -0.57735f, -0.57735f, -0.57735f };
float lightDistance = 10.0f;
float rf[] = { 0.91f, 0.92f, 0.92f };
float roughness = 0.1f;

// The OBJ model


GLuint programs[NUM_SHADERS + 1];
GLuint vShader, fShader;

char *textFileRead(const char *fileName)
{
	char *shaderText = NULL;
	GLint shaderLength = 0;
	FILE *fp;

	fp = fopen(fileName, "r");
	if (fp != NULL)
	{
		//get the char length
		while (fgetc(fp) != EOF)
		{
			shaderLength++;
		}
		rewind(fp);

		shaderText = (GLchar*)malloc(shaderLength);
		if (shaderText != NULL)
		{
			fread(shaderText, 1, shaderLength, fp);
		}
		shaderText[shaderLength] = '\0';
		fclose(fp);
	}
	return shaderText;
}

GLuint setupShaders(char *shaderName)
{
	char vshaderFile[120];
	char fshaderFile[120];
	sprintf(vshaderFile, "%s.vert", shaderName);
	sprintf(fshaderFile, "%s.frag", shaderName);

	const GLchar *vShaderSource = textFileRead(vshaderFile);
	const GLchar *fShaderSource = textFileRead(fshaderFile);

	vShader = glCreateShader(GL_VERTEX_SHADER);
	fShader = glCreateShader(GL_FRAGMENT_SHADER);

	glShaderSource(vShader, 1, &vShaderSource, NULL);
	glShaderSource(fShader, 1, &fShaderSource, NULL);

	int success;

	glCompileShader(vShader);
	glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char info_log[5000];
		glGetShaderInfoLog(vShader, 5000, NULL, info_log);
		printf("Error in vertex shader compilation!\n");
		printf("Info Log: %s\n", info_log);
	}

	glCompileShader(fShader);
	glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
	if (!success)
	{
		char info_log[5000];
		glGetShaderInfoLog(fShader, 5000, NULL, info_log);
		printf("Error in fragment shader compilation!\n");
		printf("Info Log: %s\n", info_log);
	}

	GLuint program = glCreateProgram();

	glAttachShader(program, vShader);
	glAttachShader(program, fShader);

	glLinkProgram(program);

	return program;
}

void drawEnv(float size)
{
	//glActiveTexture(GL_TEXTURE0 + MAP);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObjects[MAP]);
	//glTexEnvi(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_ENV, GL_DECAL);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	glBegin(GL_QUADS);
	//-x
	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-size, size, -size);
	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-size, size, size);
	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-size, -size, size);
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-size, -size, -size);

	//+x
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(size, -size, -size);
	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(size, -size, size);
	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(size, size, size);
	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(size, size, -size);

	//+y
	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-size, size, -size);
	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-size, size, size);
	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(size, size, size);
	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(size, size, -size);

	//-y
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(size, -size, -size);
	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(size, -size, size);
	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-size, -size, size);
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-size, -size, -size);


	//-z
	glTexCoord3f(-1.0f, -1.0f, -1.0f);
	glVertex3f(-size, -size, -size);
	glTexCoord3f(1.0f, -1.0f, -1.0f);
	glVertex3f(size, -size, -size);
	glTexCoord3f(1.0f, 1.0f, -1.0f);
	glVertex3f(size, size, -size);
	glTexCoord3f(-1.0f, 1.0f, -1.0f);
	glVertex3f(-size, size, -size);

	//+z
	glTexCoord3f(-1.0f, 1.0f, 1.0f);
	glVertex3f(-size, size, size);
	glTexCoord3f(1.0f, 1.0f, 1.0f);
	glVertex3f(size, size, size);
	glTexCoord3f(1.0f, -1.0f, 1.0f);
	glVertex3f(size, -size, size);
	glTexCoord3f(-1.0f, -1.0f, 1.0f);
	glVertex3f(-size, -size, size);
	glEnd();

	//glActiveTexture(GL_TEXTURE0 + MAP);
	glDisable(GL_TEXTURE_CUBE_MAP);
}

void drawBunny(const char * objname)
{
	GLMmodel* pModel = NULL;
	// The obj file will be loaded
	char FileName[128];
	sprintf(FileName, "%smodels/%s.obj", root, objname);

	// Center of the model
	float modelCenter[] = { 0.0f, 0.0f, 0.0f };

	// Load the new obj model
	pModel = glmReadOBJ(FileName);

	// Scale the model to fit the screen
	glmUnitize(pModel, modelCenter);

	// Generate normal for the model
	glmFacetNormals(pModel);
	
	glmScale(pModel, 1.5);
	glmVertexNormals(pModel, 90.0f);

	if (pModel)
	{
		glmDraw(pModel, GLM_SMOOTH);
	}
}

// Routine to set a quaternion from a rotation axis and angle
// ( input axis = float[3] angle = float  output: quat = float[4] )
void SetQuaternionFromAxisAngle(const float *axis, float angle, float *quat)
{
	float sina2, norm;
	sina2 = (float)sin(0.5f * angle);
	norm = (float)sqrt(axis[0] * axis[0] + axis[1] * axis[1] + axis[2] * axis[2]);
	quat[0] = sina2 * axis[0] / norm;
	quat[1] = sina2 * axis[1] / norm;
	quat[2] = sina2 * axis[2] / norm;
	quat[3] = (float)cos(0.5f * angle);
}


// Routine to convert a quaternion to a 4x4 matrix
// ( input: quat = float[4]  output: mat = float[4*4] )
void ConvertQuaternionToMatrix(const float *quat, float *mat)
{
	float yy2 = 2.0f * quat[1] * quat[1];
	float xy2 = 2.0f * quat[0] * quat[1];
	float xz2 = 2.0f * quat[0] * quat[2];
	float yz2 = 2.0f * quat[1] * quat[2];
	float zz2 = 2.0f * quat[2] * quat[2];
	float wz2 = 2.0f * quat[3] * quat[2];
	float wy2 = 2.0f * quat[3] * quat[1];
	float wx2 = 2.0f * quat[3] * quat[0];
	float xx2 = 2.0f * quat[0] * quat[0];
	mat[0 * 4 + 0] = -yy2 - zz2 + 1.0f;
	mat[0 * 4 + 1] = xy2 + wz2;
	mat[0 * 4 + 2] = xz2 - wy2;
	mat[0 * 4 + 3] = 0;
	mat[1 * 4 + 0] = xy2 - wz2;
	mat[1 * 4 + 1] = -xx2 - zz2 + 1.0f;
	mat[1 * 4 + 2] = yz2 + wx2;
	mat[1 * 4 + 3] = 0;
	mat[2 * 4 + 0] = xz2 + wy2;
	mat[2 * 4 + 1] = yz2 - wx2;
	mat[2 * 4 + 2] = -xx2 - yy2 + 1.0f;
	mat[2 * 4 + 3] = 0;
	mat[3 * 4 + 0] = mat[3 * 4 + 1] = mat[3 * 4 + 2] = 0;
	mat[3 * 4 + 3] = 1;
}


// Routine to multiply 2 quaternions (ie, compose rotations)
// ( input q1 = float[4] q2 = float[4]  output: qout = float[4] )
void MultiplyQuaternions(const float *q1, const float *q2, float *qout)
{
	float qr[4];
	qr[0] = q1[3] * q2[0] + q1[0] * q2[3] + q1[1] * q2[2] - q1[2] * q2[1];
	qr[1] = q1[3] * q2[1] + q1[1] * q2[3] + q1[2] * q2[0] - q1[0] * q2[2];
	qr[2] = q1[3] * q2[2] + q1[2] * q2[3] + q1[0] * q2[1] - q1[1] * q2[0];
	qr[3] = q1[3] * q2[3] - (q1[0] * q2[0] + q1[1] * q2[1] + q1[2] * q2[2]);
	qout[0] = qr[0]; qout[1] = qr[1]; qout[2] = qr[2]; qout[3] = qr[3];
}


// Return elapsed time in milliseconds
int GetTimeMs()
{
#if !defined(_WIN32)
	return glutGet(GLUT_ELAPSED_TIME);
#else
	// glutGet(GLUT_ELAPSED_TIME) seems buggy on Windows
	return (int)GetTickCount();
#endif
}


// Callback function called by GLUT to render screen
void Display(void)
{

	// Clear frame buffer
	glClearColor(0.7, 0.7, 0.7, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_NORMALIZE);

	float lightPositon[4];
	float l = sqrtf(powf(lightDirection[0], 2) + powf(lightDirection[1], 2) + powf(lightDirection[2], 2));
	lightPositon[0] = -lightDistance*lightDirection[0] / l;
	lightPositon[1] = -lightDistance*lightDirection[1] / l;
	lightPositon[2] = -lightDistance*lightDirection[2] / l;
	lightPositon[3] = 1.0f;                                 // point light

	glUseProgram(0);
	glPushMatrix();
	glTranslatef(0.5f, -0.3f, 0.0f);
	if (g_AutoRotate)
	{
		float axis[3] = { 0, 1, 0 };
		float angle = (float)(GetTimeMs() - g_RotateTime) / 1000.0f;
		float quat[4];
		SetQuaternionFromAxisAngle(axis, angle, quat);
		MultiplyQuaternions(g_RotateStart, quat, g_Rotation);
	}
	ConvertQuaternionToMatrix(g_Rotation, mat);
	glMultMatrixf(mat);
	glScalef(g_Zoom, g_Zoom, g_Zoom);
	glCallList(DRAW_EN);
	glPopMatrix();

	GLuint currentProgram = programs[currentShader];
	if (currentShader == 1) {
		glUseProgram(0);
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_SPECULAR, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPositon);

		float materialAmbient[] = { rf[0] * 0.2f, rf[1] * 0.2f, rf[2] * 0.2f };
		float materialDiffuse[] = { rf[0] * 0.6f, rf[1] * 0.6f, rf[2] * 0.6f };
		float materialSpecular[] = { 1 - rf[0] * roughness / 2, 1 - rf[1] * roughness / 2, 1 - rf[2] * roughness / 2 };
		glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialAmbient);
		glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialDiffuse);
		glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialSpecular);
		glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 24);
	} else {
		glUseProgram(currentProgram);
		glUniform1i(glGetUniformLocation(currentProgram, "env_brdf"), IBL);
		//#####glUniform1i(glGetUniformLocation(currentProgram, "env_map"), MAP);
		glUniform1i(glGetUniformLocation(currentProgram, "cubemap"), MAP);
		glUniform1i(glGetUniformLocation(currentProgram, "tex"), TEX);
		//glUniform1i(glGetUniformLocation(currentShader, "lightNum"), 0);
		//glUniform1f(glGetUniformLocation(currentProgram, "density"), lightAmbient[1]);
		//glUniform3fv(glGetUniformLocation(currentProgram, "ambient"), 1, lightAmbient);
		glLightfv(GL_LIGHT0, GL_POSITION, lightPositon);
		glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
		glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
		//glUniform3fv(glGetUniformLocation(currentProgram, "lightPos"), 1, lightPositon);
		//glUniform3fv(glGetUniformLocation(currentProgram, "diffuse"), 1, lightDiffuse);
		glUniform3fv(glGetUniformLocation(currentProgram, "rf"), 1, rf);
		glUniform1f(glGetUniformLocation(currentProgram, "roughness"), roughness);
		glUniform1i(glGetUniformLocation(currentProgram, "iftex"), -1);
		glUniformMatrix4fv(glGetUniformLocation(currentProgram, "g_rotation"), 1, GL_FALSE, mat);		
	}

	// Rotate and draw shape
	glPushMatrix();
	glTranslatef(0.5f, -0.3f, 0.0f);
	if (g_AutoRotate)
	{
		float axis[3] = { 0, 1, 0 };
		float angle = (float)(GetTimeMs() - g_RotateTime) / 1000.0f;
		float quat[4];
		SetQuaternionFromAxisAngle(axis, angle, quat);
		MultiplyQuaternions(g_RotateStart, quat, g_Rotation);
	}
	ConvertQuaternionToMatrix(g_Rotation, mat);
	glMultMatrixf(mat);
	glScalef(g_Zoom, g_Zoom, g_Zoom);
	glCallList(g_CurrentShape);
	glPopMatrix();

	glUseProgram(0);
	// Draw tweak bars
	TwDraw();

	// Present frame buffer
	glutSwapBuffers();

	// Recall Display at next frame
	glutPostRedisplay();
}


// Callback function called by GLUT when window size changes
void Reshape(int width, int height)
{
	// Set OpenGL viewport and camera
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, (double)width / height, 1, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 5, 0, 0, 0, 0, 1, 0);
	glTranslatef(0, 0.6f, -1);

	// Send the new window size to AntTweakBar
	TwWindowSize(width, height);
}


// Function called at exit
void Terminate(void)
{
	//glDeleteLists(SHAPE_TEAPOT, NUM_SHAPES);

	//TwTerminate();
}


//  Callback function called when the 'AutoRotate' variable value of the tweak bar has changed
void TW_CALL SetAutoRotateCB(const void *value, void *clientData)
{
	(void)clientData; // unused

	g_AutoRotate = *(const int *)value; // copy value to g_AutoRotate
	if (g_AutoRotate != 0)
	{
		// init rotation
		g_RotateTime = GetTimeMs();
		g_RotateStart[0] = g_Rotation[0];
		g_RotateStart[1] = g_Rotation[1];
		g_RotateStart[2] = g_Rotation[2];
		g_RotateStart[3] = g_Rotation[3];

		// make Rotation variable read-only
		TwDefine(" TweakBar/ObjRotation readonly ");
	}
	else
		// make Rotation variable read-write
		TwDefine(" TweakBar/ObjRotation readwrite ");
}


//  Callback function called by the tweak bar to get the 'AutoRotate' value
void TW_CALL GetAutoRotateCB(void *value, void *clientData)
{
	(void)clientData; // unused
	*(int *)value = g_AutoRotate; // copy g_AutoRotate to value
}


// Main
int main(int argc, char *argv[])
{
	TwBar *bar; // Pointer to the tweak bar
	float axis[] = { 0.7f, 0.7f, 0.0f }; // initial model rotation
	float angle = 0.8f;

	// Initialize GLUT
	//OutputDebugStringA("Test\n");
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("SRTP Middle Check");
	glutCreateMenu(NULL);
	glewInit();
	initTextureList();
	for (int i = 0; i < textpoint; i++){
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textureObjects[i]);
	}
	glActiveTexture(GL_TEXTURE0);

	sprintf(path, "%sCook-Torrorence", root);
	programs[2] = setupShaders(path);

	// Set GLUT callbacks
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);

	// Initialize AntTweakBar
	TwInit(TW_OPENGL, NULL);

	// Set GLUT event callbacks
	// - Directly redirect GLUT mouse button events to AntTweakBar
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	// - Directly redirect GLUT mouse motion events to AntTweakBar
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT mouse "passive" motion events to AntTweakBar (same as MouseMotion)
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	// - Directly redirect GLUT key events to AntTweakBar
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	// - Directly redirect GLUT special key events to AntTweakBar
	glutSpecialFunc((GLUTspecialfun)TwEventSpecialGLUT);
	// - Send 'glutGetModifers' function pointer to AntTweakBar;
	//   required because the GLUT key event functions do not report key modifiers states.
	TwGLUTModifiersFunc(glutGetModifiers);

	// Create some 3D objects (stored in display lists)
	glNewList(SHAPE_TEAPOT, GL_COMPILE);
	glutSolidTeapot(1.0);
	glEndList();
	glNewList(SHAPE_TORUS, GL_COMPILE);
	//glutSolidTorus(0.3, 1.0, 16, 32);
	glutSolidSphere(0.75f, 20, 20);
	glEndList();
	glNewList(SHAPE_DRAGON, GL_COMPILE);
	drawBunny("dragon");
	glEndList();
	glNewList(SHAPE_SKULL, GL_COMPILE);
	drawBunny("skull");
	glEndList();
	glNewList(SHAPE_GARGO, GL_COMPILE);
	drawBunny("Gargoyle_ABF");
	glEndList();
	glNewList(DRAW_EN, GL_COMPILE);
	drawEnv(30);
	glEndList();

	// Create a tweak bar
	bar = TwNewBar("TweakBar");
	TwDefine(" GLOBAL help='This example shows how to integrate AntTweakBar with GLUT and OpenGL.' "); // Message added to the help bar.
	TwDefine(" TweakBar size='250 540' color='96 216 224' "); // change default tweak bar size and color

	TwAddVarRW(bar, "Zoom", TW_TYPE_FLOAT, &g_Zoom, " min=0.01 max=7.5 step=0.01 ");
	TwAddVarRW(bar, "ObjRotation", TW_TYPE_QUAT4F, &g_Rotation, " label='Object rotation' opened=true ");
	TwAddVarCB(bar, "AutoRotate", TW_TYPE_BOOL32, SetAutoRotateCB, GetAutoRotateCB, NULL, " label='Auto-rotate' key=space ");
	TwAddVarRW(bar, "LightDir", TW_TYPE_DIR3F, &lightDirection, " label='Light direction' opened=true ");
	TwAddVarRW(bar, "LightDist", TW_TYPE_FLOAT, &lightDistance, " label='Light distance' ");
	TwAddVarRW(bar, "Ambient", TW_TYPE_COLOR3F, &lightAmbient, "");
	TwAddVarRW(bar, "Diffuse", TW_TYPE_COLOR3F, &lightDiffuse, "");
	TwAddVarRW(bar, "Rf", TW_TYPE_COLOR3F, &rf, "");
	TwAddVarRW(bar, "Roughness", TW_TYPE_FLOAT, &roughness, " label='Roughness' min=0.01 max=1.99 step=0.01 keyIncr='+' keyDecr='-' ");

	{
		TwEnumVal shaders[NUM_SHADERS] = { { SHADER_PHONG, "Phong" }, { SHADER_COOKTORRORENCE, "CookTorrorence" } };
		TwType shaderType = TwDefineEnum("ShaderType", shaders, NUM_SHADERS);
		TwAddVarRW(bar, "Shader", shaderType, &currentShader, "");
	}

	// Add the enum variable 'g_CurrentShape' to 'bar'
	// (before adding an enum variable, its enum type must be declared to AntTweakBar as follow)
	{
		// ShapeEV associates Shape enum values with labels that will be displayed instead of enum values
		TwEnumVal shapeEV[NUM_SHAPES] = { { SHAPE_TEAPOT, "Teapot" }, { SHAPE_TORUS, "Sphere" }, { SHAPE_DRAGON, "Dragon" }, { SHAPE_SKULL, "Skull" }, { SHAPE_GARGO, "Gargo" } };
		// Create a type for the enum shapeEV
		TwType shapeType = TwDefineEnum("ShapeType", shapeEV, NUM_SHAPES);
		// add 'g_CurrentShape' to 'bar': this is a variable of type ShapeType. Its key shortcuts are [<] and [>].
		TwAddVarRW(bar, "Shape", shapeType, &g_CurrentShape, "");
	}

	// Store time
	g_RotateTime = GetTimeMs();
	// Init rotation
	SetQuaternionFromAxisAngle(axis, angle, g_Rotation);
	SetQuaternionFromAxisAngle(axis, angle, g_RotateStart);

	atexit(Terminate);  // Called after glutMainLoop ends
	// Call the GLUT main loop
	glutMainLoop();

	return 0;
}
