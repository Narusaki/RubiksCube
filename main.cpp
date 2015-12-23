#include <windows.h>
#include <stack>
#define FREEGLUT_STATIC
#include <gl\freeglut.h>
#include "RubikCube.h"
#include "SOIL.h"
#include <iostream>

extern double rotateSpeed;

char title[256] = "Rubik's Cube, Created by Narusaki - rotate speed %4f";

using namespace std;

int curWin;
bool fullscreen;

RubikCube rubikCube;

GLuint textures[6];

const double depthShift = 0.0001;

//selected names and sub-cubes
vector<TriInt> selectCubes;
vector<GLuint> selectNames;

// variables to track which key of mouse is pressed
bool leftkey = false;
bool rightkey = false;

//picking buffer used in OpenGL
const int BUFFERSIZE = 512;
GLuint selectBuffer[BUFFERSIZE];
//viewport
GLint viewport[4];

//rotation parameter for one layer
int globalX, globalY;
int direction;

//a lock used to lock the rotating operation when the animation is being performed
bool isRotating = false;

//used for rotating animation
int rotateIteration = -1;

// stack to track the operations
std::stack<TriInt> operationRecords;

double globalShift = 8.0;

void myDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if (rotateIteration > 0)
		rubikCube.RotateStep(globalX, globalY, direction);
	else if(rotateIteration == 0)
	{
		rubikCube.RotateFinish(globalX, globalY, direction);
		isRotating = false;
	}

	--rotateIteration;

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(0.0, 0.0, -globalShift);
	
	//first pass, rendering the lines
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glDepthRange(0, 1.0 - depthShift);
	rubikCube.Render(GL_LINE);
	
	//second pass, rendering the faces
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glDepthRange(depthShift, 1.0);
	rubikCube.Render(GL_FILL);

	glutSwapBuffers();
	glutPostRedisplay();
}

void myMouseClick(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_UP)
		{
			leftkey = false; rightkey = false;
			return;
		}
		leftkey = true; rightkey = false;

		rubikCube.trackBall.mouseClick(Vector2D(x, y));
		glutPostRedisplay();
	}
	else if (button == GLUT_RIGHT_BUTTON)
	{
		if (isRotating) return;
		leftkey = false; rightkey = true;
		if (state == GLUT_UP)
		{
			selectCubes.clear();
			for (int i = 0;i < selectNames.size();++i)
			{
				//decompose the sub-cube's name to integer-coordinates
				TriInt curCube;
				curCube.x = selectNames[i] % rubikCube.cubeOrder; 
				selectNames[i] /= rubikCube.cubeOrder; curCube.y = selectNames[i] % rubikCube.cubeOrder;
				selectNames[i] /= rubikCube.cubeOrder; curCube.z = selectNames[i] % rubikCube.cubeOrder;
				
				selectCubes.push_back(curCube);
			}
			
			//query for the layer and the rotating direction
			if(!rubikCube.QueryLayer(selectCubes, globalX, globalY, direction))
			{
				selectNames.clear();
				return;
			}

			//record the operation
			TriInt curOp;
			curOp.x = globalX; curOp.y = globalY; curOp.z = direction;
			operationRecords.push(curOp);

			//mark to start rotating
			isRotating = true;
			rotateIteration = 90.0f/rotateSpeed;
			selectNames.clear();
			leftkey = false; rightkey = false;
		}
	}
}

/************************************************************************/
/*Find the nearest sub-cube when mouse moves
@param hitsNum: hit counts of picking

The select buffer is the global variable selectBuffer. This function returns the name 
of the selected (with the smallest z values) object (sub-cube)
*/
/************************************************************************/
GLuint findOutTheNearestCube(GLint hitsNum)
{
	/*
	TODO: find the nearest sub-cube. You need to compare the depth of all the sub-cubes selected, 
	and find the one with the minimal depth.
	*/
	return -1;
}

void myMouseMove(int x, int y)
{
	if (leftkey)
	{
		rubikCube.trackBall.mouseMove(Vector2D(x, y));
		glutPostRedisplay();
	}
	else if (rightkey)
	{
		if (isRotating) return;

		glSelectBuffer(BUFFERSIZE, selectBuffer);

		glRenderMode(GL_SELECT);
		glInitNames();
		glPushName(0);

		glMatrixMode(GL_PROJECTION);
		glPushMatrix();
		glLoadIdentity();
		gluPickMatrix((GLdouble)x, (GLdouble)(viewport[3]-y), 2.0, 2.0, viewport);
		gluPerspective(45.0f, (GLdouble)viewport[2]/(GLdouble)viewport[3], 0.1, 100.0);
		
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glDepthRange(depthShift, 1.0);
		rubikCube.Render(GL_FILL);

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		//glutPostRedisplay();

		GLint hitsNum = glRenderMode(GL_RENDER);
		GLuint pickedCube = findOutTheNearestCube(hitsNum);
		if(pickedCube == -1) return;
		if (selectNames.empty())
			selectNames.push_back(pickedCube);
		else
		{
			GLuint lastName = selectNames.back();
			if (pickedCube != lastName) selectNames.push_back(pickedCube);
		}
	}
}

void myReshape(int width, int height)
{
	if(height == 0) height = 1;

	rubikCube.trackBall.reAdjustTrackBall(width, height);
	glViewport(0, 0, width, height);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (GLdouble)width/(GLdouble)height, 0.1, 100.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glGetIntegerv(GL_VIEWPORT, viewport);
}

void myKeyboard(unsigned char key, int x, int y)
{
	if (isRotating) return;

	TriInt prevOp;
	switch(key)
	{
	case VK_ESCAPE:
		glutDestroyWindow(curWin);
		exit(0);
		break;
	case '+':
		rotateSpeed *= 2;
		char curTitle[256];
		sprintf(curTitle, title, rotateSpeed);
		glutSetWindowTitle(curTitle);
		break;
	case '-':
		rotateSpeed /= 2;
		sprintf(curTitle, title, rotateSpeed);
		glutSetWindowTitle(curTitle);
		break;
	case 'z':
		if (!operationRecords.empty())
		{
			prevOp = operationRecords.top(); operationRecords.pop();
			globalX = prevOp.x; globalY = prevOp.y; direction = -prevOp.z;
			isRotating = true;
			rotateIteration = 90.0f/rotateSpeed;
		}
		break;
	case 't':
		rubikCube.textureOn = !rubikCube.textureOn;
		if (rubikCube.textureOn) glEnable(GL_TEXTURE_2D);
		else glDisable(GL_TEXTURE_2D);
		break;
	case '1':
		globalShift += 0.5;
		break;
	case '2':
		globalShift -= 0.5;
		break;
	case '3':
		rubikCube.ReAssignOrder(3);	
		rubikCube.AssignTextures(textures);
		while (!operationRecords.empty()) operationRecords.pop(); 
		break;
	case '4':
		rubikCube.ReAssignOrder(4);	
		rubikCube.AssignTextures(textures);
		while (!operationRecords.empty()) operationRecords.pop(); 
		break;
	case '5':
		rubikCube.ReAssignOrder(5);	
		rubikCube.AssignTextures(textures);
		while (!operationRecords.empty()) operationRecords.pop(); 
		break;
	case '6':
		rubikCube.ReAssignOrder(6);	
		rubikCube.AssignTextures(textures);
		while (!operationRecords.empty()) operationRecords.pop(); 
		break;
	case '7':
		rubikCube.ReAssignOrder(7);	
		rubikCube.AssignTextures(textures);
		while (!operationRecords.empty()) operationRecords.pop(); 
		break;
	case '8':
		rubikCube.ReAssignOrder(8);	
		rubikCube.AssignTextures(textures);
		while (!operationRecords.empty()) operationRecords.pop(); 
		break;
	case '9':
		rubikCube.ReAssignOrder(9);	
		rubikCube.AssignTextures(textures);
		while (!operationRecords.empty()) operationRecords.pop(); 
		break;
	}
}

void mySpecial(int key, int x, int y)
{
	switch(key)
	{
	case GLUT_KEY_F1:
		if(!fullscreen)
		{
			glutFullScreen();
			fullscreen = true;
		}
		else
		{
			glutReshapeWindow(640, 480);
			fullscreen = false;
		}
		break;
	}
	glutPostRedisplay();
}

bool LoadTextures()
{
	
	for (int i = 0; i < 6; ++i)
	{
		char fileName[255];
		sprintf_s(fileName, "Textures/x-men_%d.png", i);

		textures[i] = SOIL_load_OGL_texture(fileName,
			SOIL_LOAD_AUTO, SOIL_CREATE_NEW_ID, SOIL_FLAG_INVERT_Y);

		if (textures[i] == 0) return false;

		glBindTexture(GL_TEXTURE_2D, textures[i]);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}

	rubikCube.AssignTextures(textures);
	return true;
}

// Initialization of OpenGL
void myInit()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	GLfloat ambient[4] = { 0.1, 0.1, 0.1, 0.1 };
	GLfloat diffuse[4] = { 0.7, 0.7, 0.7, 0.7 };
	GLfloat position[4] = { 0.0, 0.0, 4.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);

	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
	glEnable(GL_COLOR_MATERIAL);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	if (!LoadTextures())
	{
		cout << "Cannot load textures!" << endl;
		return;
	}
}

void OutputInformation()
{
	cout << "Operations: " << endl;
	cout << "Entire rotation: Left mouse + drag" << endl;
	cout << "Layer rotation: Right mouse + drag" << endl;
	cout << "1/2: zoom out/in" << endl;
	cout << "3 to 9: Cube order" << endl;
	cout << "t: Enable/Disable textures" << endl;
	cout << "z: Undo" << endl;
	cout << "+/-: Rotate speed up/slow down" << endl;
}

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(100, 100);
	char curTitle[256];
	sprintf(curTitle, title, rotateSpeed);
	curWin = glutCreateWindow(curTitle);
	fullscreen = false;

	glutDisplayFunc(myDisplay);
	glutReshapeFunc(myReshape);
	glutKeyboardFunc(myKeyboard);
	glutMouseFunc(myMouseClick);
	glutMotionFunc(myMouseMove);
	glutSpecialFunc(mySpecial);

	myInit();

	OutputInformation();

	glutMainLoop();
	return 0;
}