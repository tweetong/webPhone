#include <GL/glut.h>
#include <math.h>
#include <pthread.h>

#include "gui_params.h"
#include "gui_trans.h"

void drawCircle(int x,int y)
{
    glColor3d(1.0, 0.0, 0.0);
    glBegin(GL_POLYGON);//GL_LINE_LOOP);
    double rad;
    for(rad = 0;rad < 2*3.1415;rad += 0.1){
        glVertex2d(x + R*cos(rad),y + R*sin(rad));
    }
    glEnd();
    glFlush();
}

void resize(int w, int h)
{
    /* ウィンドウ全体をビューポートにする */
    glViewport(0, 0, w, h);

    /* 変換行列の初期化 */
    glLoadIdentity();

    /* スクリーン上の座標系をマウスの座標系に一致させる */
    glOrtho(-0.5, (GLdouble)w - 0.5, (GLdouble)h - 0.5, -0.5, -1.0, 1.0);
    /* /\* スクリーン上の表示領域をビューポートの大きさに比例させる *\/ */
    /* glOrtho(-w / 200.0, w / 200.0, -h / 200.0, h / 200.0, -1.0, 1.0); */
}

void init_gui(void)
{
    glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
      gui_send(INF,0);
      exit(0);
  default:
    break;
  }
}
