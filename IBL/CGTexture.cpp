#include "CGTexture.h"
char root0[64] = "/Users/Sylvanus/Sylvanus's Library/Studio/SRTP/IBL/IBL/";
char path0[128], path1[128], path2[128], path3[128], path4[128], path5[128], path6[128];

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
	glGenTextures(MAXTEX, textureObjects);
    sprintf(path0, "%stextures/IBL2.bmp", root0);
	soil_texture(IBL, textureObjects[IBL], path0);
	//soil_texture(MAP, textureObjects[MAP], "textures/cubemap.jpg",1);
    sprintf(path0, "%stextures/city.bmp", root0);
	soil_texture(TEX, textureObjects[TEX], path0);
    sprintf(path0, "%stextures/city.bmp", root0);
	soil_texture(FLOOR,textureObjects[FLOOR], path0);
	/*soil_texture(WALL, textureObjects[WALL], "textures/floor.bmp");
	soil_texture(BED, textureObjects[BED], "textures/down.bmp");
	soil_texture(WHALE, textureObjects[WHALE], "textures/cow.bmp");*/
	textpoint = FLOOR;
//    sprintf(path1, "%stextures/right.bmp", root0);
//    sprintf(path2, "%stextures/left.bmp", root0);
//    sprintf(path3, "%stextures/up.bmp", root0);
//    sprintf(path4, "%stextures/down.bmp", root0);
//    sprintf(path5, "%stextures/back.bmp", root0);
//    sprintf(path6, "%stextures/front.bmp", root0);
    sprintf(path1, "%stextures/cow.bmp", root0);
    sprintf(path2, "%stextures/cow.bmp", root0);
    sprintf(path3, "%stextures/cow.bmp", root0);
    sprintf(path4, "%stextures/cow.bmp", root0);
    sprintf(path5, "%stextures/cow.bmp", root0);
    sprintf(path6, "%stextures/cow.bmp", root0);
    textureObjects[MAP] = SOIL_load_OGL_cubemap(
		path1,
		path2,
		path3,
		path4,
		path5,
		path6,
		SOIL_LOAD_RGB,
		SOIL_CREATE_NEW_ID,
		SOIL_FLAG_MIPMAPS
		);
	glActiveTexture(GL_TEXTURE0 + MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureObjects[MAP]);
}

GLuint loadCubemap(std::vector<const GLchar*> faces)
{
    GLuint textureID;
    glGenTextures(1, &textureID);
    glActiveTexture(GL_TEXTURE0+MAP);

    int width,height;
    unsigned char* image;
    
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);
    for(GLuint i = 0; i < faces.size(); i++)
    {
        image = SOIL_load_image(faces[i], &width, &height, 0, SOIL_LOAD_RGB);
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    return textureID;
}