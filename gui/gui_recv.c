#include <GL/glut.h>
#include <stdio.h>

#include "gui_util.h"
#include "gui_params.h"
#include "gui_trans.h"

static int pointnum = 0;
static GLint point[MAXPOINTS][2];

static void display(void)
{
    /* 記録したデータで線を描く */
    if (pointnum > 1) {
        drawCircle(point[pointnum-1][0],point[pointnum-1][1]);
    }
    glFlush();
}

static void Idle(){
    GLint *p;
    if((p = gui_recv()) == NULL) return;
    point[pointnum][0] = p[0];
    point[pointnum][1] = p[1];
    pointnum++;
    glutPostRedisplay();
}

int gui_tcp_recv(int argc, char *argv[], int sd)
{
    init_gui_trans(sd);
    glutInitWindowPosition(400, 100);
    glutInitWindowSize(320, 240);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutCreateWindow("recv");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutIdleFunc(Idle);
    init_gui();
    glutMainLoop();
    return 0;
}
