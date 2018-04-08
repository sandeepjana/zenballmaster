#if 0
#include <GL/glut.h>
#include <chrono>
#include <iostream>

auto prev = std::chrono::steady_clock::now();
GLfloat angle = 0.0;

void spin(void) {
    angle += 0.01;
    glutPostRedisplay();

    //auto now = std::chrono::steady_clock::now();
    //auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev);
    //std::cout << elapsed.count() << "\n";
    //prev = now;
}
void display(void) {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
    glRotatef(angle, 1, 0, 0);
    glRotatef(angle, 0, 1, 0);
    glRotatef(angle, 0, 0, 1);
    glutWireCube(2.0);

    //important for smooth animation:
    glutSwapBuffers();
    //Dont call glFlush because it is called inside SwapBuffers!
    //glFlush(); /* Force update of screen */
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
}
int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitWindowSize(500, 500);
    glutInitWindowPosition(100, 100);

    //important: for double buffering:
    glutInitDisplayMode(GLUT_DOUBLE);

    glutCreateWindow("ex6: A rotating cube.");
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutIdleFunc(spin); /* Register the "idle" function */
    glutMainLoop();
    return 0;
}
#endif