#include "CGTexture.h"
unsigned int textureObjects[MAXTEX];
int textpoint;

void soil_texture(int item, GLuint& ID, const char* filepath){
	ID = SOIL_load_OGL_texture(
		filepath,
		SOIL_LOAD_AUTO,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_NTSC_SAFE_RGB | SOIL_FLAG_MULTIPLY_ALPHA | SOIL_FLAG_INVERT_Y | SOIL_FLAG_MIPMAPS
		);
	glActiveTexture(GL_TEXTURE0 + item);
	glBindTexture(GL_TEXTURE_2D, ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void initTextureList(){
    char root[64] = "/Users/Sylvanus/Sylvanus's Library/Studio/SRTP/IBL/IBL/";
    char path[128];
	glGenTextures(MAXTEX, textureObjects);
    sprintf(path, "%stextures/IBL2.bmp", root);
	soil_texture(IBL, textureObjects[IBL], path);
	//soil_texture(MAP, textureObjects[MAP], "textures/cubemap.jpg",1);

	textpoint = FLOOR;

	textureObjects[MAP] = loadCubemap();
	/*textureObjects[MAP] = SOIL_load_OGL_cubemap(
		"textures/right.bmp",
		"textures/left.bmp",
		"textures/up.bmp",
		"textures/down.bmp",
		"textures/back.bmp",
		"textures/front.bmp",
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
		);*/
	glActiveTexture(GL_TEXTURE0 + MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObjects[MAP]);
}

GLuint loadCubemap()
{
    char root[64] = "/Users/Sylvanus/Sylvanus's Library/Studio/SRTP/IBL/IBL/";
    char path[128];
	GLuint textureID;
	glGenTextures(1, &textureID);
	glActiveTexture(GL_TEXTURE0 + MAP);

	int width, height;
	unsigned char* image;

	int level = 5;
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, level);
	for(int i = 0; i <= level; i++){
		sprintf(path, "%stextures/CM2/1-%d.bmp", root, i);
		image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, i, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		sprintf(path, "%stextures/CM2/0-%d.bmp", root, i);
		image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, i, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		sprintf(path, "%stextures/CM2/2-%d.bmp", root, i);
		image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, i, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		sprintf(path, "%stextures/CM2/3-%d.bmp", root, i);
		image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, i, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		sprintf(path, "%stextures/CM2/5-%d.bmp", root, i);
		image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, i, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
		sprintf(path, "%stextures/CM2/4-%d.bmp", root, i);
		image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, i, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	}
	/*sprintf(path, "textures/CM2/1-2.bmp");
	image = SOIL_load_image(path, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 1, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 1, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 1, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 1, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 1, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 1, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);*/
	return textureID;
}
