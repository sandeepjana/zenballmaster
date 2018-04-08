#if 0
#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>

#include "geometry.h"

std::vector<point_t> points;
std::vector<point_t> hull;

//http://stackoverflow.com/questions/12171317/how-do-you-install-glut-and-opengl-in-visual-studio-2012
#include <GL\glut.h>

void display (void) {
	/* Called when OpenGL needs to update the display */
	glClear (GL_COLOR_BUFFER_BIT); /* Clear the window */

	//The model view matrix settings here seem to be optional
	glLoadIdentity (); /* start with a unit modelview matrix */
	gluLookAt (0.0, 0.0, 0.5, /* position of camera */
		0.0, 0.0, 0.0, /* point at which camera looks */
		0.0, 1.0, 0.0); /* "up" direction of camera */

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    /*glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);*/
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glBegin (GL_LINE_LOOP); /* Draw a triangle */
	//glVertex3f(-0.3, -0.3, 0.0);
	//glVertex3f(0.0, 0.3, 0.0);
	//glVertex3f(0.3, -0.3, 0.0);
	//glEnd();
	glPointSize(3.0);
    glLineWidth(1.5);

	glBegin(GL_POINTS);
	std::for_each(points.begin(), points.end(), [](point_t& p) { 
		glVertex3f(p.first, p.second, 0.0);
	});
	glEnd();

	glBegin(GL_LINE_LOOP);
	std::for_each(hull.begin(), hull.end(), [](point_t& p) { 
		glVertex3f(p.first, p.second, 0.0);
	});
	glEnd();

	printf(".\n");

	glFlush(); /* Force update of screen */
}

void keyboard (unsigned char key, int x, int y) {
	/* Called when a key is pressed */
	if (key == 27) exit (0); /* 27 is the Escape key */
}



template <typename T>
std::vector<point_t> half_scan(T begin, T end)
{
	std::vector<point_t> half;
	auto it = begin;
	half.push_back(*it);
	++it;
	half.push_back(*it);
	++it;

	for(; it != end; ++it)
	{
		half.push_back(*it);
		
		auto size = half.size();
		while(size > 2
            && TURN_T::RIGHT != find_turn(half[size - 3], half[size - 2], half[size - 1]))
		{
			//delete middle of the last three points
			half[size - 2] = half[size - 1];
			half.pop_back();

			size = half.size();
		}
	}

	return half;
}

void convex_hull(std::vector<point_t> p, std::vector<point_t>& h)
{
	if(p.size() < 4)
		return;

	std::sort(p.begin(), p.end()); //why did i sort?

	h = half_scan(p.begin(), p.end());
	auto lower = half_scan(p.rbegin(), p.rend());
	h.insert(h.end(), lower.begin() + 1, lower.end() - 1);

	return;
}

int main(int argc, char** argv)
{
	points.resize(100);

    srand(1947);
	std::generate(points.begin(), points.end(),
		[](){ return std::make_pair(randfloat(), randfloat()); }
    );

	convex_hull(points, hull);

	glutInit (&argc, argv); /* Initialise OpenGL */
	glutCreateWindow ("Canvas"); /* Create the window */
	glutDisplayFunc (display); /* Register the "display" function */
	glutKeyboardFunc (keyboard);
	glutMainLoop (); /* Enter the OpenGL main loop */

}
#endif