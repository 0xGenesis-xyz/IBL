#ifndef cgTexture_h
#define cgTexture_h

#include <AntTweakBar.h>

//#include <Windows.h>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <vector>
#include <GL/glew.h>
//#include <glut.h>
#include "SOIL.h"
#pragma comment (lib, "glew32d.lib")
#pragma comment(lib,"lib\\SOIL.lib")
#define IBL 0
#define MAP 1
#define TEX 2
#define FLOOR 3
#define WALL 7
#define BED 4
#define WHALE 5
#define ITEM 6
#define MAXTEX 32
// The GLM code for loading and displying OBJ mesh file
#include "glm.h"

// The trackball code for rotating the model
#include "trackball.h"


extern int textpoint;
extern unsigned int textureObjects[MAXTEX];
void initTextureList();
void soil_texture(int item, GLuint& ID, const char* filepath);
GLuint loadCubemap();
#endif /* cgTexture_h */
