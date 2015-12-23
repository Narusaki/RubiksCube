#include "RubikCube.h"
#include <iomanip>
#include <gl\glut.h>
#include <vector>
#include <array>
#include <iostream>

//#define MAKEBLANK

using namespace std;

double rotateSpeed = 1.0f;

int colors[6][3] = { { 255, 0, 0 },
{ 255, 255, 0 },
{ 0, 255, 0 },
{ 0, 0, 255 },
{ 255, 122, 0 },
{ 255, 255, 255 } };

/************************************************************************/
/* Constructor of RubikCube class
 Read this function carefully to understand the structure of the Rubik's cube*/
/************************************************************************/

RubikCube::RubikCube()
{
	cubeOrder = 3;
	textureOn = false;

	hiddenFace.resize(cubeOrder);
	for (int i = 0; i < cubeOrder; ++i)
	{
		hiddenFace[i].resize(cubeOrder);
		for (int j = 0; j < cubeOrder; ++j)
			hiddenFace[i][j].resize(cubeOrder);
	}
	hiddenFace[0][0][0] = { { 1, 0, 0, 1, 0, 1 } }; hiddenFace[0][0][1] = { { 1, 0, 0, 0, 0, 1 } }; hiddenFace[0][0][2] = { { 1, 1, 0, 0, 0, 1 } };
	hiddenFace[0][1][0] = { { 1, 0, 0, 1, 0, 0 } };	hiddenFace[0][1][1] = { { 1, 0, 0, 0, 0, 0 } };	hiddenFace[0][1][2] = { { 1, 1, 0, 0, 0, 0 } };
	hiddenFace[0][2][0] = { { 1, 0, 0, 1, 1, 0 } };	hiddenFace[0][2][1] = { { 1, 0, 0, 0, 1, 0 } };	hiddenFace[0][2][2] = { { 1, 1, 0, 0, 1, 0 } };

	hiddenFace[1][0][0] = { { 0, 0, 0, 1, 0, 1 } };	hiddenFace[1][0][1] = { { 0, 0, 0, 0, 0, 1 } };	hiddenFace[1][0][2] = { { 0, 1, 0, 0, 0, 1 } };
	hiddenFace[1][1][0] = { { 0, 0, 0, 1, 0, 0 } };	hiddenFace[1][1][1] = { { 0, 0, 0, 0, 0, 0 } };	hiddenFace[1][1][2] = { { 0, 1, 0, 0, 0, 0 } };
	hiddenFace[1][2][0] = { { 0, 0, 0, 1, 1, 0 } };	hiddenFace[1][2][1] = { { 0, 0, 0, 0, 1, 0 } };	hiddenFace[1][2][2] = { { 0, 1, 0, 0, 1, 0 } };

	hiddenFace[2][0][0] = { { 0, 0, 1, 1, 0, 1 } };	hiddenFace[2][0][1] = { { 0, 0, 1, 0, 0, 1 } };	hiddenFace[2][0][2] = { { 0, 1, 1, 0, 0, 1 } };
	hiddenFace[2][1][0] = { { 0, 0, 1, 1, 0, 0 } };	hiddenFace[2][1][1] = { { 0, 0, 1, 0, 0, 0 } };	hiddenFace[2][1][2] = { { 0, 1, 1, 0, 0, 0 } };
	hiddenFace[2][2][0] = { { 0, 0, 1, 1, 1, 0 } };	hiddenFace[2][2][1] = { { 0, 0, 1, 0, 1, 0 } };	hiddenFace[2][2][2] = { { 0, 1, 1, 0, 1, 0 } };

	memset(trackBall.MVmatrix, 0, 16*sizeof(double));
	trackBall.MVmatrix[0] = 1;
	trackBall.MVmatrix[5] = 1;
	trackBall.MVmatrix[10] = 1;
	trackBall.MVmatrix[15] = 1;

	// construct magicCube and layers
	reallocate();
}

const int RubikCube::order[3][2][4] = { { { 4, 3, 5, 1 }, { 1, 5, 3, 4 } },
{ { 3, 2, 1, 0 }, { 0, 1, 2, 3 } },
{ { 4, 2, 5, 0 }, { 0, 5, 2, 4 } } };

const int RubikCube::orderTex[3][2][6][4][2] =
{
	// x axis
	{
		{
			{ { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 0 } },
			{ { 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 } },
			{ { 2, 3 }, { 2, 0 }, { 2, 1 }, { 2, 2 } },
			{ { 5, 0 }, { 5, 1 }, { 5, 2 }, { 5, 3 } },
			{ { 3, 1 }, { 3, 2 }, { 3, 3 }, { 3, 0 } },
			{ { 1, 3 }, { 1, 0 }, { 1, 1 }, { 1, 2 } }
		},
		{
			{ { 0, 3 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },
			{ { 5, 1 }, { 5, 2 }, { 5, 3 }, { 5, 0 } },
			{ { 2, 1 }, { 2, 2 }, { 2, 3 }, { 2, 0 } },
			{ { 4, 3 }, { 4, 0 }, { 4, 1 }, { 4, 2 } },
			{ { 1, 0 }, { 1, 1 }, { 1, 2 }, { 1, 3 } },
			{ { 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 } }
		}
	},
	// y axis
	{
		{
			{ { 3, 0 }, { 3, 1 }, { 3, 2 }, { 3, 3 } },
			{ { 0, 3 }, { 0, 0 }, { 0, 1 }, { 0, 2 } },
			{ { 1, 3 }, { 1, 0 }, { 1, 1 }, { 1, 2 } },
			{ { 2, 2 }, { 2, 3 }, { 2, 0 }, { 2, 1 } },
			{ { 4, 3 }, { 4, 0 }, { 4, 1 }, { 4, 2 } },
			{ { 5, 1 }, { 5, 2 }, { 5, 3 }, { 5, 0 } }
		},
		{
			{ { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 0 } },
			{ { 2, 1 }, { 2, 2 }, { 2, 3 }, { 2, 0 } },
			{ { 3, 2 }, { 3, 3 }, { 3, 0 }, { 3, 1 } },
			{ { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 } },
			{ { 4, 1 }, { 4, 2 }, { 4, 3 }, { 4, 0 } },
			{ { 5, 3 }, { 5, 0 }, { 5, 1 }, { 5, 2 } }
		}
	},
	// z axis
	{
		{
			{ { 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 } },
			{ { 1, 3 }, { 1, 0 }, { 1, 1 }, { 1, 2 } },
			{ { 5, 3 }, { 5, 0 }, { 5, 1 }, { 5, 2 } },
			{ { 3, 1 }, { 3, 2 }, { 3, 3 }, { 3, 0 } },
			{ { 2, 0 }, { 2, 1 }, { 2, 2 }, { 2, 3 } },
			{ { 0, 1 }, { 0, 2 }, { 0, 3 }, { 0, 0 } }
		},
		{
			{ { 5, 3 }, { 5, 0 }, { 5, 1 }, { 5, 2 } },
			{ { 1, 1 }, { 1, 2 }, { 1, 3 }, { 1, 0 } },
			{ { 4, 0 }, { 4, 1 }, { 4, 2 }, { 4, 3 } },
			{ { 3, 3 }, { 3, 0 }, { 3, 1 }, { 3, 2 } },
			{ { 0, 0 }, { 0, 1 }, { 0, 2 }, { 0, 3 } },
			{ { 2, 1 }, { 2, 2 }, { 2, 3 }, { 2, 0 } }
		}
	}
};

const int RubikCube::relativeTop[3][2] = { { 0, 2 },
{ 4, 5 },
{ 1, 3 } };

RubikCube::~RubikCube()
{

}

void RubikCube::ReAssignOrder(int newOrder)
{
	clear();
	cubeOrder = newOrder;

	memset(trackBall.MVmatrix, 0, 16 * sizeof(double));
	trackBall.MVmatrix[0] = 1;
	trackBall.MVmatrix[5] = 1;
	trackBall.MVmatrix[10] = 1;
	trackBall.MVmatrix[15] = 1;

	reallocate();
}

void RubikCube::AssignTextures(GLuint textures[6])
{
	for (int i = 0; i < cubeOrder; ++i)
		for (int j = 0; j < cubeOrder; ++j)
			for (int k = 0; k < cubeOrder; ++k)
				for (int l = 0; l < 6; ++l)
					magicCube[i][j][k].texHandles[l] = textures[l];
}

/************************************************************************/
/* Query which layer of the Rubik's cube should rotate
@param select: the sub-cubes selected (represented by the integer-coordinates);
@param layerX, layerY: layer-coordinates
@param dir: rotating direction of the layer (1 or -1)*/
/************************************************************************/

bool RubikCube::QueryLayer(const std::vector<TriInt>& select, int& layerX, int& layerY, int& dir)
{
	/*
	TODO: you need to implement this function to judge which layer is selected and which direction it follows to rotate
	This function will decide how the program react to the operation on the Rubik's cube.
	*/
	return false;
}

/************************************************************************/
/* Rotate a single angle (rotateSpeed) for the sub-cubes in the given layer and store the result model-view matrix for each 
rotated sub-cube in its matrix variable
@param x, y: integer-coordinates of the rotated layer;
@param dir: direction of the rotation, 1 or -1.*/
/************************************************************************/
void RubikCube::RotateStep(int x, int y, int dir)
{
	//TODO:
}

/************************************************************************/
/* For the sub-cubes of rotated layers, update their colors.
@param x, y: integer-coordinates of the rotated layer
@param dir: rotating direction

This function focuses on two things:

1. Set the correct color and texture coordinates for the rotated sub-cubes. Note 
that the correct color and texture coordinates for each rotated sub-cubes can be 
calculated by some sub-cube in the same layer (mathematically this procedure can 
be called as PERMUTATION). You should fill in the order, orderTex and relativeTop 
fields of the RubikCube class and invoke the ReplaceColors() method to help to 
adjust them. Of course you can do this totally by yourself.

2. Reset matrix for each sub-cube to be identity. The matrix of each sub-cube is only 
used to display an animation of rotation. And only the rotated sub-cubes need to be 
reset to identity (others do not change).
*/
/************************************************************************/
void RubikCube::RotateFinish(int x, int y, int dir)
{
	//TODO:
}

void RubikCube::Render(GLenum mode)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixd(trackBall.MVmatrix);

	for (int i = 0;i < cubeOrder;++i)
	{
		for (int j = 0;j < cubeOrder;++j)
		{
			for (int k = 0;k < cubeOrder;++k)
			{
				if(mode == GL_FILL) glLoadName(i+j*cubeOrder+k*cubeOrder*cubeOrder);
				RenderSingleCube(mode, i, j, k);
			}
		}
	}
	glPopMatrix();
}

void RubikCube::RenderSingleCube(GLenum mode, int x, int y, int z)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glMultMatrixd(magicCube[x][y][z].matrix);

	//move the whole Rubik's cube to the center
	glTranslatef(-(double)cubeOrder*cubeSize / 2, -(double)cubeOrder*cubeSize / 2, -(double)cubeOrder * cubeSize / 2);

	if (mode == GL_LINE)
	{
		GLfloat sizes[2];
		GLfloat step;
		glGetFloatv(GL_LINE_WIDTH_RANGE, sizes);
		glGetFloatv(GL_LINE_WIDTH_GRANULARITY, &step);
		glLineWidth(sizes[0] + 40 * step);
		glEnable(GL_LINE_SMOOTH);
		glColor3ub(0, 0, 0);
	}

	glBindTexture(GL_TEXTURE_2D, magicCube[x][y][z].texHandles[1]);
	glBegin(GL_QUADS);
	//front face
	if(!textureOn && mode == GL_FILL && hiddenFace[x][y][z][1])
		glColor3ub(magicCube[x][y][z].faceColor[1].x, magicCube[x][y][z].faceColor[1].y, magicCube[x][y][z].faceColor[1].z);
	else if (mode == GL_LINE || !hiddenFace[x][y][z][1])
		glColor3ub(0, 0, 0);
	else glColor3ub(255, 255, 255);
	glNormal3d(0.0, 0.0, 1.0);
	glTexCoord2d(magicCube[x][y][z].tex[1][0].x, magicCube[x][y][z].tex[1][0].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[1][1].x, magicCube[x][y][z].tex[1][1].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[1][2].x, magicCube[x][y][z].tex[1][2].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[1][3].x, magicCube[x][y][z].tex[1][3].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, magicCube[x][y][z].texHandles[3]);
	glBegin(GL_QUADS);
	//back face
	if(!textureOn && mode == GL_FILL && hiddenFace[x][y][z][3])
		glColor3ub(magicCube[x][y][z].faceColor[3].x, magicCube[x][y][z].faceColor[3].y, magicCube[x][y][z].faceColor[3].z);
	else if (mode == GL_LINE || !hiddenFace[x][y][z][3])
		glColor3ub(0, 0, 0);
	else glColor3ub(255, 255, 255);
	glNormal3d(0.0, 0.0, -1.0);
	glTexCoord2d(magicCube[x][y][z].tex[3][0].x, magicCube[x][y][z].tex[3][0].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[3][1].x, magicCube[x][y][z].tex[3][1].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[3][2].x, magicCube[x][y][z].tex[3][2].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[3][3].x, magicCube[x][y][z].tex[3][3].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, magicCube[x][y][z].texHandles[4]);
	glBegin(GL_QUADS);
	//top face
	if(!textureOn && mode == GL_FILL && hiddenFace[x][y][z][4])
		glColor3ub(magicCube[x][y][z].faceColor[4].x, magicCube[x][y][z].faceColor[4].y, magicCube[x][y][z].faceColor[4].z);
	else if (mode == GL_LINE || !hiddenFace[x][y][z][4])
		glColor3ub(0, 0, 0);
	else glColor3ub(255, 255, 255);
	glNormal3d(0.0, 1.0, 0.0);
	glTexCoord2d(magicCube[x][y][z].tex[4][0].x, magicCube[x][y][z].tex[4][0].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[4][1].x, magicCube[x][y][z].tex[4][1].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[4][2].x, magicCube[x][y][z].tex[4][2].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[4][3].x, magicCube[x][y][z].tex[4][3].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, magicCube[x][y][z].texHandles[5]);
	glBegin(GL_QUADS);
	//bottom face
	if(!textureOn && mode == GL_FILL && hiddenFace[x][y][z][5])
		glColor3ub(magicCube[x][y][z].faceColor[5].x, magicCube[x][y][z].faceColor[5].y, magicCube[x][y][z].faceColor[5].z);
	else if (mode == GL_LINE || !hiddenFace[x][y][z][5])
		glColor3ub(0, 0, 0);
	else glColor3ub(255, 255, 255);
	glNormal3d(0.0, -1.0, 0.0);
	glTexCoord2d(magicCube[x][y][z].tex[5][0].x, magicCube[x][y][z].tex[5][0].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[5][1].x, magicCube[x][y][z].tex[5][1].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[5][2].x, magicCube[x][y][z].tex[5][2].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[5][3].x, magicCube[x][y][z].tex[5][3].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, magicCube[x][y][z].texHandles[2]);
	glBegin(GL_QUADS);
	//right face
	if(!textureOn && mode == GL_FILL && hiddenFace[x][y][z][2])
		glColor3ub(magicCube[x][y][z].faceColor[2].x, magicCube[x][y][z].faceColor[2].y, magicCube[x][y][z].faceColor[2].z);
	else if (mode == GL_LINE || !hiddenFace[x][y][z][2])
		glColor3ub(0, 0, 0);
	else glColor3ub(255, 255, 255);
	glNormal3d(1.0, 0.0, 0.0);
	glTexCoord2d(magicCube[x][y][z].tex[2][0].x, magicCube[x][y][z].tex[2][0].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[2][1].x, magicCube[x][y][z].tex[2][1].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[2][2].x, magicCube[x][y][z].tex[2][2].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[2][3].x, magicCube[x][y][z].tex[2][3].y);
	glVertex3f(magicCube[x][y][z].pos[0] + cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glEnd();

	glBindTexture(GL_TEXTURE_2D, magicCube[x][y][z].texHandles[0]);
	glBegin(GL_QUADS);
	//left face
	if(!textureOn && mode == GL_FILL && hiddenFace[x][y][z][0])
		glColor3ub(magicCube[x][y][z].faceColor[0].x, magicCube[x][y][z].faceColor[0].y, magicCube[x][y][z].faceColor[0].z);
	else if (mode == GL_LINE || !hiddenFace[x][y][z][0])
		glColor3ub(0, 0, 0);
	else glColor3ub(255, 255, 255);
	glNormal3d(-1.0, 0.0, 0.0);
	glTexCoord2d(magicCube[x][y][z].tex[0][0].x, magicCube[x][y][z].tex[0][0].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[0][1].x, magicCube[x][y][z].tex[0][1].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] + cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[0][2].x, magicCube[x][y][z].tex[0][2].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] + cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glTexCoord2d(magicCube[x][y][z].tex[0][3].x, magicCube[x][y][z].tex[0][3].y);
	glVertex3f(magicCube[x][y][z].pos[0] - cubeSize/2, magicCube[x][y][z].pos[1] - cubeSize/2, magicCube[x][y][z].pos[2] - cubeSize/2);
	glEnd();

	glPopMatrix();
}

/************************************************************************/
/*  Replace cube1's color with cube2's color under two permutations
@param cube1, cube2: two sub-cubes, former is destination, latter is source;
@param order, relativeTop: two permutations*/
/************************************************************************/
void RubikCube::ReplaceColors(Cube *cube1, Cube *cube2, int o1, int o2)
{
	cube1->faceColor[order[o1][o2][0]] = cube2->faceColor[order[o1][o2][1]];
	cube1->faceColor[order[o1][o2][1]] = cube2->faceColor[order[o1][o2][2]];
	cube1->faceColor[order[o1][o2][2]] = cube2->faceColor[order[o1][o2][3]];
	cube1->faceColor[order[o1][o2][3]] = cube2->faceColor[order[o1][o2][0]];
	cube1->faceColor[relativeTop[o1][0]] = cube2->faceColor[relativeTop[o1][0]];
	cube1->faceColor[relativeTop[o1][1]] = cube2->faceColor[relativeTop[o1][1]];

	cube1->texHandles[order[o1][o2][0]] = cube2->texHandles[order[o1][o2][1]];
	cube1->texHandles[order[o1][o2][1]] = cube2->texHandles[order[o1][o2][2]];
	cube1->texHandles[order[o1][o2][2]] = cube2->texHandles[order[o1][o2][3]];
	cube1->texHandles[order[o1][o2][3]] = cube2->texHandles[order[o1][o2][0]];
	cube1->texHandles[relativeTop[o1][0]] = cube2->texHandles[relativeTop[o1][0]];
	cube1->texHandles[relativeTop[o1][1]] = cube2->texHandles[relativeTop[o1][1]];

	for (int i = 0; i < 6; ++i)
		for (int j = 0; j < 4; ++j)
			cube1->tex[i][j] = cube2->tex[orderTex[o1][o2][i][j][0]][orderTex[o1][o2][i][j][1]];
}


void RubikCube::clear()
{
	for (int i = 0; i < cubeOrder; ++i)
	{
		for (int j = 0; j < cubeOrder; ++j)
			delete []magicCube[i][j];
		delete []magicCube[i];
	}
	delete[] magicCube;

	for (int i = 0; i < 3; ++i)
		delete[] layers[i];
	delete[] layers;
}

void RubikCube::reallocate()
{
	magicCube = new Cube**[cubeOrder];
	for (int i = 0; i < cubeOrder; ++i)
	{
		magicCube[i] = new Cube*[cubeOrder];
		for (int j = 0; j < cubeOrder; ++j)
			magicCube[i][j] = new Cube[cubeOrder];
	}

	layers = new Layer*[3];
	for (int i = 0; i < 3; ++i)
	{
		layers[i] = new Layer[cubeOrder];
		for (int j = 0; j < cubeOrder; ++j)
			layers[i][j].cubes = new TriInt[cubeOrder*cubeOrder];
	}


	int faceSeq = 0;
	for (int i = 0; i < cubeOrder; ++i)
	{
		for (int j = 0; j < cubeOrder; ++j)
		{
			for (int k = 0; k < cubeOrder; ++k)
			{
				magicCube[i][j][k].pos = Vector3D(cubeSize / 2 + i*cubeSize,
					cubeSize / 2 + j*cubeSize, cubeSize / 2 + k*cubeSize);
				memset(magicCube[i][j][k].matrix, 0, 16 * sizeof(double));
				magicCube[i][j][k].matrix[0] = 1;
				magicCube[i][j][k].matrix[5] = 1;
				magicCube[i][j][k].matrix[10] = 1;
				magicCube[i][j][k].matrix[15] = 1;

				for (int l = 0; l < 6; ++l)
				{
					magicCube[i][j][k].faceColor[l].x = colors[l][0];
					magicCube[i][j][k].faceColor[l].y = colors[l][1];
					magicCube[i][j][k].faceColor[l].z = colors[l][2];
				}
			}
		}
	}

	//Construct layers rotate around x-axis
	for (int i = 0; i < cubeOrder; ++i)
	{
		unsigned baseNum = 0;
		unsigned shift = 0;
		for (int j = cubeOrder; j >= 1; j -= 2)
		{
			if (j == 1)
			{
				layers[0][i].cubes[cubeOrder*cubeOrder - 1].x = i;
				layers[0][i].cubes[cubeOrder*cubeOrder - 1].y = cubeOrder / 2;
				layers[0][i].cubes[cubeOrder*cubeOrder - 1].z = cubeOrder / 2;
				continue;
			}
			for (int k = 0; k < j - 1; ++k)
			{
				layers[0][i].cubes[k + baseNum].x = i;
				layers[0][i].cubes[k + baseNum].y = k + shift;
				layers[0][i].cubes[k + baseNum].z = 0 + shift;
			}
			for (int k = j - 1; k < 2 * j - 2; ++k)
			{
				layers[0][i].cubes[k + baseNum].x = i;
				layers[0][i].cubes[k + baseNum].y = j - 1 + shift;
				layers[0][i].cubes[k + baseNum].z = k - (j - 1) + shift;
			}
			for (int k = 2 * j - 2; k < 3 * j - 3; ++k)
			{
				layers[0][i].cubes[k + baseNum].x = i;
				layers[0][i].cubes[k + baseNum].y = 3 * j - 3 - k + shift;
				layers[0][i].cubes[k + baseNum].z = j - 1 + shift;
			}
			for (int k = 3 * j - 3; k < 4 * j - 4; ++k)
			{
				layers[0][i].cubes[k + baseNum].x = i;
				layers[0][i].cubes[k + baseNum].y = 0 + shift;
				layers[0][i].cubes[k + baseNum].z = 4 * j - 4 - k + shift;
			}

			baseNum += 4 * (j - 1);
			shift += 1;
		}
		layers[0][i].axis = Vector3D(1.0, 0.0, 0.0);
	}

	//Construct layers rotate around y-axis
	for (int i = 0; i < cubeOrder; ++i)
	{
		unsigned baseNum = 0;
		unsigned shift = 0;
		for (int j = cubeOrder; j >= 1; j -= 2)
		{
			if (j == 1)
			{
				layers[1][i].cubes[cubeOrder*cubeOrder - 1].y = i;
				layers[1][i].cubes[cubeOrder*cubeOrder - 1].z = cubeOrder / 2;
				layers[1][i].cubes[cubeOrder*cubeOrder - 1].x = cubeOrder / 2;
				continue;
			}
			for (int k = 0; k < j - 1; ++k)
			{
				layers[1][i].cubes[k + baseNum].y = i;
				layers[1][i].cubes[k + baseNum].z = k + shift;
				layers[1][i].cubes[k + baseNum].x = 0 + shift;
			}
			for (int k = j - 1; k < 2 * j - 2; ++k)
			{
				layers[1][i].cubes[k + baseNum].y = i;
				layers[1][i].cubes[k + baseNum].z = j - 1 + shift;
				layers[1][i].cubes[k + baseNum].x = k - (j - 1) + shift;
			}
			for (int k = 2 * j - 2; k < 3 * j - 3; ++k)
			{
				layers[1][i].cubes[k + baseNum].y = i;
				layers[1][i].cubes[k + baseNum].z = 3 * j - 3 - k + shift;
				layers[1][i].cubes[k + baseNum].x = j - 1 + shift;
			}
			for (int k = 3 * j - 3; k < 4 * j - 4; ++k)
			{
				layers[1][i].cubes[k + baseNum].y = i;
				layers[1][i].cubes[k + baseNum].z = 0 + shift;
				layers[1][i].cubes[k + baseNum].x = 4 * j - 4 - k + shift;
			}

			baseNum += 4 * (j - 1);
			shift += 1;
		}
		layers[1][i].axis = Vector3D(0.0, 1.0, 0.0);
	}

	//Construct layers rotate around z-axis
	for (int i = 0; i < cubeOrder; ++i)
	{
		unsigned baseNum = 0;
		unsigned shift = 0;
		for (int j = cubeOrder; j >= 1; j -= 2)
		{
			if (j == 1)
			{
				layers[2][i].cubes[cubeOrder*cubeOrder - 1].z = i;
				layers[2][i].cubes[cubeOrder*cubeOrder - 1].x = cubeOrder / 2;
				layers[2][i].cubes[cubeOrder*cubeOrder - 1].y = cubeOrder / 2;
				continue;
			}
			for (int k = 0; k < j - 1; ++k)
			{
				layers[2][i].cubes[k + baseNum].z = i;
				layers[2][i].cubes[k + baseNum].x = k + shift;
				layers[2][i].cubes[k + baseNum].y = 0 + shift;
			}
			for (int k = j - 1; k < 2 * j - 2; ++k)
			{
				layers[2][i].cubes[k + baseNum].z = i;
				layers[2][i].cubes[k + baseNum].x = j - 1 + shift;
				layers[2][i].cubes[k + baseNum].y = k - (j - 1) + shift;
			}
			for (int k = 2 * j - 2; k < 3 * j - 3; ++k)
			{
				layers[2][i].cubes[k + baseNum].z = i;
				layers[2][i].cubes[k + baseNum].x = 3 * j - 3 - k + shift;
				layers[2][i].cubes[k + baseNum].y = j - 1 + shift;
			}
			for (int k = 3 * j - 3; k < 4 * j - 4; ++k)
			{
				layers[2][i].cubes[k + baseNum].z = i;
				layers[2][i].cubes[k + baseNum].x = 0 + shift;
				layers[2][i].cubes[k + baseNum].y = 4 * j - 4 - k + shift;
			}

			baseNum += 4 * (j - 1);
			shift += 1;
		}
		layers[2][i].axis = Vector3D(0.0, 0.0, 1.0);
	}

	// adjust hidden face
	if (hiddenFace.size() < cubeOrder)
	{
		unsigned originOrder = hiddenFace.size();

		for (int i = 0; i < originOrder; ++i)
		{
			for (int j = 0; j < originOrder; ++j)
			{
				for (int l = 0; l < cubeOrder - originOrder; ++l)
				{
					auto beg = hiddenFace[i][j].begin();
					auto end = hiddenFace[i][j].end() - 2;
					hiddenFace[i][j].insert(beg+1, *end);
				}
			}
		}

		for (int i = 0; i < originOrder; ++i)
		{
			for (int l = 0; l < cubeOrder - originOrder; ++l)
			{
				auto beg = hiddenFace[i].begin();
				auto end = hiddenFace[i].end() - 2;
				hiddenFace[i].insert(beg+1, *end);
			}
		}

		for (int l = 0; l < cubeOrder - originOrder; ++l)
		{
			auto beg = hiddenFace.begin();
			auto end = hiddenFace.end() - 2;
			hiddenFace.insert(beg+1, *end);
		}
	}
	else if (hiddenFace.size() > cubeOrder)
	{
		unsigned originOrder = hiddenFace.size();

		for (int i = 0; i < originOrder; ++i)
		{
			for (int j = 0; j < originOrder; ++j)
			{
				for (int l = 0; l < originOrder - cubeOrder; ++l)
				{
					auto end = hiddenFace[i][j].end() - 2;
					hiddenFace[i][j].erase(end);
				}
			}
		}

		for (int i = 0; i < originOrder; ++i)
		{
			for (int l = 0; l < originOrder - cubeOrder; ++l)
			{
				auto end = hiddenFace[i].end() - 2;
				hiddenFace[i].erase(end);
			}
		}

		for (int l = 0; l < originOrder - cubeOrder; ++l)
		{
			auto end = hiddenFace.end() - 2;
			hiddenFace.erase(end);
		}
	}

}