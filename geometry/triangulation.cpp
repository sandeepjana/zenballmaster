#if 0
/*
Divide a polygon into triangles using a maximal set of diagonals.
The number of triangles = Number of vertices of polygon - 2.
There is a better approach of triangulation using monotones. But here we use a brute force O(n^2)

Find the leftmost vertices v
Find its neighbors u, w
Join the diagonal uw to make a triangle uvw if there is no vertex t of polygon lies on or inside the triangle.
If one or more interior vertices exist, pick one which is leftmost and join it with v.
This approach will make sure that each diagonal added for triangulation resides inside the polygon.
Triangulat the remaining polygon just like above.

Note: You cannot add diagonal from an arbitrary point to another arbitrary point (barring the neighbors)
because that diagonal may fall outside the polygon.

No explicit tree is constructed in order to traverse the triangles. Since anyway we need to
run through all triangles (out of which a triangle can have at max 3 neighbors) in order to see if a
particular triangle is "connected" -- sharing a diagonal with rest of the triangles.

Both triangulation and coloring are implemented using recursion below.

Note: next time, dont represent the triangle by three fields of type point_t. Instead use array of three point_t type.


*/

#include <stdio.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <utility>
#include <limits>
#include <set>
#include <cassert>
#include <fstream>
#include <map>

#include "geometry.h"

//http://stackoverflow.com/questions/12171317/how-do-you-install-glut-and-opengl-in-visual-studio-2012
#include <GL\glut.h>

std::vector<point_t> points;
std::vector<line_t> lines;
std::vector<triangle_t> triangles;

void triangulation(std::vector<point_t> ps, std::vector<triangle_t>& ts)
{
    assert(ps.size() >= 3);
    if (3 == ps.size())
    {
        ts.push_back(triangle_t{ ps[0], ps[1], ps[2] });
        return;
    }

    auto u = std::min_element(begin(ps), end(ps));
    //auto v = u == begin(ps) ? end(ps) - 1 : begin(ps) - 1;
    //auto w = u == end(ps) - 1 ? begin(ps) : u + 1;

    std::rotate(begin(ps), u, end(ps));

    //check if any vertices in the rest fall inside the triangle <u, v, w>
    std::vector<std::vector<point_t>::const_iterator> inside;
    auto tri = triangle_t{ *begin(ps), *(begin(ps) + 1), *(end(ps) - 1) };
    for (auto it = begin(ps) + 2; it != end(ps) - 1; ++it)
    {
        if (POINT_LOC_T::OUTSIDE != is_point_inside_triangle(tri, *it))
        {
            inside.push_back(it);
        }
    }

    if (0 != inside.size())
    {
        auto t = std::min_element(inside.begin(), inside.end(), 
            [](std::vector<point_t>::const_iterator a, std::vector<point_t>::const_iterator b)
        {
            return *a < *b;
        });
        auto topPoly = std::vector<point_t>(*t, cend(ps)); topPoly.push_back(*begin(ps));
        auto botPoly = std::vector<point_t>(cbegin(ps), *t); botPoly.push_back(*(*t));

        triangulation(topPoly, triangles);
        triangulation(botPoly, triangles);
        return;
    }

    triangles.push_back(tri);
    auto rest = std::vector<point_t>(begin(ps) + 1, end(ps));
    triangulation(rest, triangles);

    return;
}

bool apply_color_if_neighbor(const triangle_t& a, triangle_t& b)
{
    auto v1 = find_vertex(a, b.u);
    auto v2 = find_vertex(a, b.v);
    auto v3 = find_vertex(a, b.w);

    if (triangle_t::VERTEX_T::NO != v1 && triangle_t::VERTEX_T::NO != v2)
    {
        auto aother = find_other_vertex(v1, v2);
        /*
        so here: a.v1 = b.u and
        a.v2 = b.v
        */
        b.color[0] = a.color[static_cast<int>(v1)];
        b.color[1] = a.color[static_cast<int>(v2)];
        b.color[2] = a.color[static_cast<int>(aother)];
        return true;
    }

    if (triangle_t::VERTEX_T::NO != v1 && triangle_t::VERTEX_T::NO != v3)
    {
        auto aother = find_other_vertex(v1, v3);
        b.color[0] = a.color[static_cast<int>(v1)];
        b.color[2] = a.color[static_cast<int>(v3)];
        b.color[1] = a.color[static_cast<int>(aother)];
        return true;
    }

    if (triangle_t::VERTEX_T::NO != v2 && triangle_t::VERTEX_T::NO != v3)
    {
        auto aother = find_other_vertex(v2, v3);
        b.color[1] = a.color[static_cast<int>(v2)];
        b.color[2] = a.color[static_cast<int>(v3)];
        b.color[0] = a.color[static_cast<int>(aother)];
        return true;
    }

    return false;

}

void apply_color(int current, std::vector<triangle_t>& triangles)
{
    auto& now = triangles[current];
    if (0 == current) //take care first call
    {
        now.color[0] = 'r';
        now.color[1] = 'g';
        now.color[2] = 'b';
    }

    now.isColored = true; //set before the for-loop below
    
    //find the next triangle(s) that is adjacent to 'now' to color
    int adjacent = 0;
    for (int i = 0; i < triangles.size(); ++i)
    {
        auto& next = triangles[i];
        if (next.isColored) continue; //already visited this triangle

        auto colored = apply_color_if_neighbor(now, next);
        if (colored)
        {
            //apply color to the child of next
            apply_color(i, triangles); //dfs travel
            
            ++adjacent;
            if (2 == adjacent) //there cant be more than two adj 'next's
                break;
        }
    }

}

bool insert_into_map(std::map<point_t, char>& colormap, const point_t& p, char c)
{
    auto status = colormap.insert(make_pair(p, c));
    if (status.second == false && status.first->second != c)
        throw std::string("triangle coloring is inconsistent");

    return status.second;
}

void drawString(void *font, float x, float y, char *str) {
    /* Draws string ’str’ in font ’font’, at world (x,y,0) */
    char *ch;
    glRasterPos3f(x, y, 0.0);
    for (ch = str; *ch; ch++)
        glutBitmapCharacter(font, (int)*ch);
}

void display(void) {
    /* Called when OpenGL needs to update the display */
    glClear(GL_COLOR_BUFFER_BIT); /* Clear the window */

    //The model view matrix settings here seem to be optional
    glLoadIdentity(); /* start with a unit modelview matrix */
    gluLookAt(0.0, 0.0, 0.5, /* position of camera */
        0.0, 0.0, 0.0, /* point at which camera looks */
        0.0, 1.0, 0.0); /* "up" direction of camera */

    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glPointSize(4.0);
    glLineWidth(1.5);

    if (points.size())
    {
        glBegin(GL_LINE_LOOP);//glBegin(GL_POINTS);
        //std::for_each(lines.begin(), lines.end(), [](line_t& line) {
        //    glVertex3f(line.first.first, line.first.second, 0.0);
        //});
        std::for_each(points.begin(), points.end(), [](point_t& p) {
            glVertex3f(p.first, p.second, 0.0);
        });
        glEnd();
    }

    if (lines.size())
    {
        glBegin(GL_LINES);
        //each pair of vertices form a line
        std::for_each(lines.begin(), lines.end(), [](line_t& line) {
            glVertex3f(line.first.first, line.first.second, 0.0);
            glVertex3f(line.second.first, line.second.second, 0.0);
        });
        glEnd();
    }

    int i = 0;
    for (auto& t : triangles)
    {
        glBegin(GL_LINE_LOOP);
        glVertex3f(t.u.first, t.u.second, 0.0);
        glVertex3f(t.v.first, t.v.second, 0.0);
        glVertex3f(t.w.first, t.w.second, 0.0);
        glEnd();

        auto cg = (t.u + t.v + t.w);
        glRasterPos3f(cg.first/3.f, cg.second/3.f, 0.0);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)('A' + i));
        ++i;

        glRasterPos3f(t.u.first, t.u.second, 0.0);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)(t.color[0]));
        glRasterPos3f(t.v.first, t.v.second, 0.0);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)(t.color[1]));
        glRasterPos3f(t.w.first, t.w.second, 0.0);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)(t.color[2]));

    }

    //label top end-points of lines
    auto tmp = lines;
    std::sort(tmp.begin(), tmp.end(), [](const line_t& a, const line_t& b){
        return a.first < b.first;;
    });
    i = 0;
    for (auto& line : tmp)
    {
        //glColor3f(1.0, 1.0, 1.0); /* Select white */
        glRasterPos3f(line.first.first, line.first.second, 0.0);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)('A' + i));
        ++i;
    }

    i = 0;
    for (auto& p : points)
    {
        glRasterPos3f(p.first, p.second, 0.0);
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)('a' + i));
        ++i;
    }



    printf(".\n");

    glFlush(); /* Force update of screen */
}

void keyboard(unsigned char key, int x, int y) {
    /* Called when a key is pressed */
    if (key == 27) exit(0); /* 27 is the Escape key */
}



int main(int argc, char** argv)
{
    float x, y;
    std::ifstream ifs("polygons.txt");
    while (ifs >> x >> y)
    {
        x = (x - 220) / 220.f;
        y = -(y - 150) / 220.f;
        points.push_back({ x, y });
    }

    //points.push_back({ -0.5, -0.5 });
    //points.push_back({ +0.5, -0.5 });
    //points.push_back({ +0.5, +0.5 });
    //points.push_back({ -0.5, +0.5 });

    triangulation(points, triangles);

    int current = 0;
    apply_color(current, triangles);

    //test for (partial) consistency
    std::map<point_t, char> colormap;
    int colorcount[3] = {};
    for (auto & tri : triangles)
    {
        insert_into_map(colormap, tri.u, tri.color[0]);
        insert_into_map(colormap, tri.v, tri.color[1]);
        insert_into_map(colormap, tri.w, tri.color[2]);
    }

    for (auto it = colormap.begin(); it != colormap.end(); ++it)
    {
        (it->second == 'r') ? ++colorcount[0] :
            ('b' == it->second) ? ++colorcount[1] : ++colorcount[2];
    }

    std::cout << "r " << colorcount[0] << std::endl;
    std::cout << "g " << colorcount[1] << std::endl;
    std::cout << "b " << colorcount[2] << std::endl;

    //for concise display
    points.clear();


    glutInit(&argc, argv); /* Initialise OpenGL */
    glutCreateWindow("Canvas"); /* Create the window */
    glutDisplayFunc(display); /* Register the "display" function */
    glutKeyboardFunc(keyboard);
    glutMainLoop(); /* Enter the OpenGL main loop */

}
#endif