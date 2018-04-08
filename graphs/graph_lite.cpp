#if 0
#include <iostream>
#include <string>

#include <GL/glut.h>
#include <chrono>
#include <fstream>
#include <algorithm>

auto prev = std::chrono::steady_clock::now();
const int MAX_NUM_VERTICES = 16;

struct graph_t
{
    int numvertices;
    int adj[MAX_NUM_VERTICES][MAX_NUM_VERTICES];

    int pathlength[MAX_NUM_VERTICES];
    bool isAdded[MAX_NUM_VERTICES];

    int prev[MAX_NUM_VERTICES]; //optional: needed if you want to trace the path

    //debug and draw
    std::pair<float, float> vertices[MAX_NUM_VERTICES];
};

auto g = graph_t{};

/*
Initialize path lengths to each vertex: infinity if there is no edge from source, otherwise the edge weight
In loop
  1. Find the vertex vnear with smallest path length
  2. Update path lengths via vnear for all vertices NOT in "found-shortest-path" using W[vnear][j]
  3. Add vnear to the set of "found-shortest-path"
*/

/*
Q. How vertices with shortest paths computed added to a set? Do I need to maintained "done" flag to each vertex?
A. Better to maintain the flag instead of cheekily using some other field
Q. How to keep track of short path (seq of edges) from source to each vertex? (do I need to maintain one array for each vertex?)
A. A one dim array of length = number of vertices is sufficient. This is because shortest path to a vertex v via another vertex w
contains the shortest path to w as well. 
*/
void dijkstra_init(graph_t& g)
{
    g.prev[0] = 0;

    for (int i = 0; i < g.numvertices; ++i)
    {
        g.pathlength[i] = 0 == g.adj[0][i]? INT_MAX : g.adj[0][i];
    }

    g.pathlength[0] = 0;
    g.isAdded[0] = true;
}

void dijkstra_step(graph_t& g)
{
    int minpath = INT_MAX;
    int vnear = 0; //TODO: what is the proper value here?
    for (int i = 0; i < g.numvertices; ++i)
    {
        if (false == g.isAdded[i] //not in "found-shortest-path"
            && minpath > g.pathlength[i])
        {
            minpath = g.pathlength[i];
            vnear = i;
        }
    }

    std::cout << "vnear is " << vnear << "\n";
    //add vnear to "found-shortest-path"
    if (0 == vnear)
    {
        return; //done with vertices that has path from source
    }

    g.isAdded[vnear] = true;

    //see if path length via vnear is any better
    for (int j = 0; j < g.numvertices; ++j)
    {
        if (false == g.isAdded[j] //not in "found-shortest-path" 
            && 0 != g.adj[vnear][j] //not disconnected from vnear
            && g.pathlength[j] > g.adj[vnear][j] + minpath) //avoid integer overflow
        {
            g.pathlength[j] = minpath + g.adj[vnear][j];
            g.prev[j] = vnear;
        }
    }
}

void drawString(void *font, float x, float y, char *str) {
    /* Draws string ’str’ in font ’font’, at world (x,y,0) */
    char *ch;
    glColor3f(0, 0, 1);

    glRasterPos3f(x, y, 0.0);
    for (ch = str; *ch; ch++)
        glutBitmapCharacter(font, (int)*ch);

    glColor3f(1, 1, 1);
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

    glPointSize(50.0);
    glLineWidth(1.5);

    glBegin(GL_POINTS);
    for (int i = 0; i < g.numvertices; ++i)
        glVertex3f(g.vertices[i].first, g.vertices[i].second, 0.0);
    glEnd();

    
    for (int i = 0; i < MAX_NUM_VERTICES; ++i)
        for (int j = 0; j < MAX_NUM_VERTICES; ++j)
        {
            if (0 != g.adj[i][j])
            {
                glBegin(GL_LINES);
                glVertex3f(g.vertices[i].first, g.vertices[i].second, 0.0);
                glVertex3f(g.vertices[j].first, g.vertices[j].second, 0.0);
                glEnd();

                float x = (0.2 * g.vertices[i].first + 0.8 * g.vertices[j].first);
                float y = (0.2 * g.vertices[i].second + 0.8 * g.vertices[j].second);

                glRasterPos3f(x, y, 0.0);
                glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, (int)('0' + g.adj[i][j]));
            }
        }
    
    //now draw the paths found so far
    glLineWidth(4);
    
    for (int i = 0; i < g.numvertices; ++i)
    {
        if (true == g.isAdded[i])
        {
            std::cout << "prev " << g.prev[i] << "\n";
            //edge from prev[i] to i is part of the shortest path to i (so far)
            glBegin(GL_LINES);
            glVertex3f(g.vertices[g.prev[i]].first, g.vertices[g.prev[i]].second, 0.0);
            glVertex3f(g.vertices[i].first, g.vertices[i].second, 0.0);
            glEnd();

            char bignumber[4] = {};
            drawString(GLUT_BITMAP_TIMES_ROMAN_24, g.vertices[i].first, g.vertices[i].second,
                itoa(g.pathlength[i], bignumber, 10));
        }
    }
    

    glFlush();
}
void reshape(int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(60, (GLfloat)w / (GLfloat)h, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 27) exit(0); /* escape key */

    if (key == ' ')
    {
        dijkstra_step(g);
    }

    glutPostRedisplay();
}
int main(int argc, char **argv) {

    std::ifstream infile = std::ifstream("graphdata.txt");

    int numedges = 0;
    infile >> g.numvertices;
    infile >> numedges;
    for (int i = 0; i < numedges; ++i)
    {
        int from, to;
        infile >> from >> to;
        infile >> g.adj[from][to];
    }

    for (int i = 0; i < g.numvertices; ++i)
    {
        float x, y;
        infile >> x >> y;
        g.vertices[i] = std::make_pair(x, y);
    }

    dijkstra_init(g);

    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);

    glutCreateWindow("graphs");
    //glClearColor(1.0f, 1.0f, 0.0f, 0.0f);

    glutDisplayFunc(display);
    //glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}

#endif