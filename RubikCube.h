#ifndef _RUBIKCUBE_H_
#define _RUBIKCUBE_H_

#include <vector>
#include <list>
#include <array>
#include "Vector3D.h"
#include <gl\glut.h>
#include "trackball.h"
#include <ctime>

//sub-cube size
const double cubeSize = 1.0f;

struct TriInt
{
	int x, y, z;
};

//Sub-cube structure
struct Cube
{
	//colors of faces
	TriInt faceColor[6];

	// texture handles
	GLuint texHandles[6];
	//texture coordinates of faces
	Vector2D tex[6][4];

	//center of the sub-cube
	Vector3D pos;

	//model-view matrix, used to display an animation
	double matrix[16];

public:
	Cube() {
		tex[0][0] = Vector2D(1.0, 0.0); tex[0][1] = Vector2D(1.0, 1.0);
		tex[0][2] = Vector2D(0.0, 1.0); tex[0][3] = Vector2D(0.0, 0.0);

		tex[1][0] = Vector2D(0.0, 0.0); tex[1][1] = Vector2D(1.0, 0.0);
		tex[1][2] = Vector2D(1.0, 1.0); tex[1][3] = Vector2D(0.0, 1.0);

		tex[2][0] = Vector2D(0.0, 1.0); tex[2][1] = Vector2D(0.0, 0.0);
		tex[2][2] = Vector2D(1.0, 0.0); tex[2][3] = Vector2D(1.0, 1.0);

		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 4; ++j)
				tex[i + 3][j] = tex[i][j];
	}
};

//Layer structure
struct Layer
{
	//sub-cubes. represented by integer-coordinates
	//see the constructor of RubikCube for details
	TriInt *cubes;

	//rotating axis
	Vector3D axis;
};

//Structure of Rubik's cube
class RubikCube
{
	//27 sub-cubes, organized as a cubeOrder by cubeOrder by cubeOrder structure.
	Cube ***magicCube;

	//9 layers, organized as a 3 by cubeOrder as the rotating axis and position.
	Layer **layers;

	// hidden face information
	std::vector< std::vector< std::vector< std::array<bool, 6> > > > hiddenFace;

public:
	// some dirty numbers used to switch colors and texture coordinates
	static const int order[3][2][4];

	static const int orderTex[3][2][6][4][2];

	static const int relativeTop[3][2];

public:
	unsigned cubeOrder;
	bool textureOn;

public:
	//trackball, controlling the 3D position of the whole Rubik's cube.
	TrackBall trackBall;

public:
	RubikCube();
	~RubikCube();

	void ReAssignOrder(int newOrder);

	void AssignTextures(GLuint textures[6]);

	bool QueryLayer(const std::vector<TriInt>& select, int& layerX, int& layerY, int& dir);

	void RotateStep(int x, int y, int dir);

	void RotateFinish(int x, int y, int dir);

	//Render
	void Render(GLenum mode);

private:

	void RenderSingleCube(GLenum mode, int x, int y, int z);

	void ReplaceColors(Cube *cube1, Cube *cube2, int o1, int o2);

private:
	void clear();
	void reallocate();
};

#endif