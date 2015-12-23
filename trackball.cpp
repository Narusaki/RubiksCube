#include "trackball.h"

TrackBall::TrackBall()
{
}

/************************************************************************/
/* Initialize the trackball.
@param width: width of the window
@param height: height of the window
The two parameters here are used for extending the unit trackball to fill 
the whole screen. 
*/
/************************************************************************/

TrackBall::TrackBall(double width, double height)
{
	adjustWidth = 1 / (width * 0.5);
	adjustHeight = 1 / (height * 0.5);
}

/************************************************************************/
/* Map a 2D point onto the surface of the trackball.
@param screen_v2d: The screen coordinates of the current cursor
@param v3d: The converted 3D point on the surface of the trackball*/
/************************************************************************/

void TrackBall::mapToSphere(const Vector2D& screen_v2d, Vector3D& v3d)
{
	//Convert screen_v2d to coordinates in OpenGL
	Vector2D v2d;
	v2d.x = screen_v2d.x * adjustWidth - 1.0;
	v2d.y = 1.0 - screen_v2d.y * adjustHeight;

	/*
	TODO: map v2d to a point on the surface of the trackball.
	When v2d is out of the range of the trackball, you can always map it onto the z=0 plane.
	*/
	double x2y2 = v2d.x*v2d.x + v2d.y*v2d.y;

	if (x2y2 < 1.0)
	{
		v3d.x = v2d.x;
		v3d.y = v2d.y;
		v3d.z = sqrt(1.0 - x2y2);
	}
	else
	{
		double norm = 1.0 / sqrt(x2y2);
		v3d.x = v2d.x * norm;
		v3d.y = v2d.y * norm;
		v3d.z = 0;
	}
}

/************************************************************************/
/* Initialization*/
/************************************************************************/
void TrackBall::init()
{
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0, 0.0, 6.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glGetDoublev(GL_MODELVIEW_MATRIX, MVmatrix);
}

/************************************************************************/
/* Reset the adjustment parameters. Called when the window is reshaped.
@param width: width of the window
@param height: height of the window */
/************************************************************************/

void TrackBall::reAdjustTrackBall(double width, double height)
{
	adjustWidth = 1.0 / (width * 0.5);
	adjustHeight = 1.0 / (height * 0.5);
}

/************************************************************************/
/* Accomplish a trackball-rotation. Called by myMouseMove() in main.cpp.
@param v2d: The screen coordinates of the current cursor. */
/************************************************************************/

void TrackBall::mouseMove(Vector2D v2d)
{
	/*
	TODO: calculation the rotation from last_point_3D to v2d, and append 
	the operation to MVmatrix. The rotation uses the cross product of this two 
	points as the axis. After the rotation, the mapped v2d point should be stored 
	in last_point_3D for the next rotation.

	Note that you need to (implement and) use mapToSphere() to convert the screen 
	coordinates to 3D coordinates on the surface of the trackball. And the rotate() 
	also helps you to append the rotation to the "tail" of the MVmatrix;
	*/
	Vector2D new_point_2D = v2d;
	Vector3D new_point_3D;
	mapToSphere(new_point_2D, new_point_3D);

	Vector3D axis = last_point_3D ^ new_point_3D;
	if(axis.length() < 1e-7)
		axis = Vector3D(0, 0, 0);
	else
		axis.norm();

	Vector3D dist = last_point_3D - new_point_3D;
	double sinangle = 0.5*dist.length();
	double angle = asin(sinangle) / PI * 180.0;

	rotate(axis, 4*angle);

	last_point_3D = new_point_3D;
}

/************************************************************************/
/* Initialize the last_point_3D point. Called by myMouseClick() in main.cpp.
@param v2d The screen coordinates of the current cursor. */
/************************************************************************/
void TrackBall::mouseClick(Vector2D v2d)
{
	mapToSphere(v2d, last_point_3D);
}

/************************************************************************/
/* Append the rotate operation to the "tail" of MVmatrix
@param axis: the axis of the rotation
@param angle: the angle of the rotation */
/************************************************************************/
void TrackBall::rotate(const Vector3D& axis, double angle)
{
	/*
	TODO: Left multiply the MVmatrix by the rotation matrix , and store the result in MVmatrix

	Note: 
	1 The rotate should be around the center of the Rubik's cube itself. But since you've (probably) 
	done some shifting (into the screen), you may need to shift the cube back to origin, rotate it, then shift it back.
	2 A LEFT multiplication is requested here to achieve a first-in-first-out order for the operations , but the Model-View matrix 
	multiplication in OpenGL is Right multiplication.
	*/

	Vector3D trans(MVmatrix[12], MVmatrix[13], MVmatrix[14]);
	
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(trans.x, trans.y, trans.z);
	glRotatef(angle, axis.x, axis.y, axis.z);
	glTranslatef(-trans.x, -trans.y, -trans.z);

	glMultMatrixd(MVmatrix);

	glGetDoublev(GL_MODELVIEW_MATRIX, MVmatrix);
}

/************************************************************************/
/* Append the shift operation to the "tail" of MVmatrix
@param shift: the shifting vector */
/************************************************************************/
void TrackBall::translate(const Vector3D& shift)
{
	/*
	TODO: Left multiply the MVmatrix by a translating matrix , and store the result in MVmatrix
	*/
}