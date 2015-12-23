#include "Vector3D.h"
#include <gl\glut.h>

#ifndef _TRACKBALL_H_
#define  _TRACKBALL_H_

const double PI = 3.1459265359;

class TrackBall
{
	Vector3D last_point_3D;

	double adjustWidth, adjustHeight;
public:
	double MVmatrix[16];

public:
	TrackBall();
	TrackBall(double width, double height);

	void init();

	void reAdjustTrackBall(double width, double height);

	void mouseMove(Vector2D v2d);

	void mouseClick(Vector2D v2d);

private:

	void mapToSphere(const Vector2D& screen_v2d, Vector3D& v3d);

	void rotate(const Vector3D& axis, double angle);
	void translate(const Vector3D& shift);
};

#endif