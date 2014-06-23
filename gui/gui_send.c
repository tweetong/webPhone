#include <GL/glut.h>

#include "gui_util.h"
#include "gui_params.h"
#include "gui_trans.h"

static int pointnum = 0;
static GLint point[MAXPOINTS][2];

static void motion(int x, int y)
{
    gui_send(x,y);
    /* ボタンを操作した位置を記録する */
    point[pointnum][0] = x;
    point[pointnum][1] = y;
    if(pointnum < MAXPOINTS) pointnum++;
    drawCircle(x,y);
}

static void display(void)
{
    int i;
    glClear(GL_COLOR_BUFFER_BIT);
    /* 記録したデータで線を描く */
    if (pointnum > 1) {
        glBegin(GL_LINES);
        for (i = 0; i < pointnum; ++i) {
            drawCircle(point[i][0],point[i][1]);
        }
        glEnd();
    }
    glFlush();
}

int gui_tcp_send(int argc, char *argv[], int sd)
{
    init_gui_trans(sd);
    glutInitWindowPosition(WINDOW_POS_SEND_X, WINDOW_POS_SEND_Y);
    glutInitWindowSize(WINDOW_SIZE_X, WINDOW_SIZE_Y);
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_RGBA);
    glutCreateWindow("send");
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutMotionFunc(motion);
    init_gui();
    glutMainLoop();
    return 0;
}
