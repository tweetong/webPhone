#include <GL/glut.h>
#include <math.h>
#include <pthread.h>
#include <png.h>
#include <sys/stat.h>
#include <errno.h>

#include "gui_params.h"
#include "gui_trans.h"
#include "../util/die.h"

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

char* edit_str(char* s){
    static char str[126];
    char *png = ".png";
    char *image_dir = "./image/";
    int i,j,k;
    k = 0;
    if (mkdir(image_dir,
              S_IRUSR | S_IWUSR | S_IXUSR |         /* rwx */
              S_IRGRP | S_IWGRP | S_IXGRP |         /* rwx */
              S_IROTH | S_IXOTH | S_IXOTH) == 0) {  /* rwx */
        printf("%sを作成しました\n", image_dir);
    }else if(errno == EEXIST){
    }else die("mkdir");
    for(k = 0;k < 8;k++){
        str[k] = image_dir[k];
    }
    for(i = 0;s[i] != '\0';i++){
        if(s[i] == ' ') {str[k] = '_';k++;}
        else if(s[i] == '\n') ;
        else {str[k] = s[i];k++;}
    }
    for(j = 0;png[j] != '\0';j++,k++){
        str[k] = png[j];
    }
    return str;
}

void capture()
{
    time_t now_time = time(NULL);
    char* str_now_time = ctime(&now_time);
    char* file_name = edit_str(str_now_time);

    png_bytep raw1D;
    png_bytepp raw2D;
    int i;
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);

    // 構造体確保
    FILE *fp = fopen(file_name, "wb");
    png_structp pp = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    png_infop ip = png_create_info_struct(pp);
    // 書き込み準備
    png_init_io(pp, fp);
    png_set_IHDR(pp, ip, width, height,
        8, // 8bit以外にするなら変える
        PNG_COLOR_TYPE_RGBA, // RGBA以外にするなら変える
        PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);
    // ピクセル領域確保
    raw1D = (png_bytep)malloc(height * png_get_rowbytes(pp, ip));
    raw2D = (png_bytepp)malloc(height * sizeof(png_bytep));
    for (i = 0; i < height; i++)
        raw2D[i] = &raw1D[i*png_get_rowbytes(pp, ip)];
    // 画像のキャプチャ
    glReadBuffer(GL_FRONT);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // 初期値は4
    glReadPixels(0, 0, width, height,
            GL_RGBA, // RGBA以外にするなら変える
            GL_UNSIGNED_BYTE, // 8bit以外にするなら変える
            (void*)raw1D);
    // 上下反転
    for (i = 0; i < height/ 2; i++){
        png_bytep swp = raw2D[i];
        raw2D[i] = raw2D[height - i - 1];
        raw2D[height - i - 1] = swp;
    }
    // 書き込み
    png_write_info(pp, ip);
    png_write_image(pp, raw2D);
    png_write_end(pp, ip);
    // 開放
    png_destroy_write_struct(&pp, &ip);
    fclose(fp);
    free(raw1D);
    free(raw2D);

    printf("write out screen capture to '%s'\n", file_name);
}

void keyboard(unsigned char key, int x, int y)
{
  switch (key) {
  case 'q':
  case 'Q':
  case '\033':
      gui_send(INF,0);
      exit(0);
      break;
  case 'c':
      glClear(GL_COLOR_BUFFER_BIT);
      glFlush();
      gui_send(0,INF);
      break;
  case 's':
      capture();
      break;
  default:
    break;
  }
}
