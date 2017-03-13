#include <iostream>
#include <mutex>
#include <fstream>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <sys/ioctl.h>
#include <cstdio>
#include <termios.h>
#include <math.h>
#include <thread>
#include <vector>

using namespace std;

#define HEIGHT 1300
#define WIDTH 700

typedef struct{
    int x;
    int y;
} Point;

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;

int redPixelMatrix[WIDTH][HEIGHT];
int greenPixelMatrix[WIDTH][HEIGHT];
int bluePixelMatrix[WIDTH][HEIGHT];
int posX;
int posY;
int lastCorrectState = 's';
bool exploded = false;

struct bullet
{
    int xStart;
    int yStart;
    int xEnd;
    int yEnd;
    float m;
    float c;
    int partisi;
    int iteration;
    int x1;
    int x2;
    int n;
};

unsigned char* BMP;
int BMP_width, BMP_height;

vector<bullet> bullets;
mutex bullet_mutex;

void clearMatrix() {
    for (int i = 0; i < WIDTH; ++i)
    {
        for (int j = 0; j < HEIGHT; ++j)
        {
            redPixelMatrix[i][j] = 0;
            greenPixelMatrix[i][j] = 0;
            bluePixelMatrix[i][j] = 0;
        }
    }
}

void drawWhitePoint(int x1, int y1) {
    if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT) return;
    redPixelMatrix[x1][y1] = 255;
    greenPixelMatrix[x1][y1] = 255;
    bluePixelMatrix[x1][y1] = 255;
}

void drawRedPoint(int x1,int y1){
    if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT) return;
    redPixelMatrix[x1][y1] = 255;
    greenPixelMatrix[x1][y1] = 0;
    bluePixelMatrix[x1][y1] = 0;
}

void drawBlackPoint(int x1,int y1){
    if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT) return;
    redPixelMatrix[x1][y1] = 0;
    greenPixelMatrix[x1][y1] = 0;
    bluePixelMatrix[x1][y1] = 0;
}

void drawGreenPoint(int x1,int y1){
    if (x1 < 0 || x1 >= WIDTH || y1 < 0 || y1 >= HEIGHT) return;
    redPixelMatrix[x1][y1] = 0;
    greenPixelMatrix[x1][y1] = 255;
    bluePixelMatrix[x1][y1] = 0;
}

void drawPohon(int x, int y, int size) {
    drawGreenLine(x+size-1-size/2, y, x, y+size-1);
    drawGreenLine(x+size/2, y, x+size-1, y+size-1);
    drawGreenLine(x, y, x+size-1, y+size-1);
    floodFill(x/2, y/2, 0,255,0,0,255,0);
}

void drawGreenLine(int x1, int y1, int x2, int y2) {
    //Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;
    deltaX = abs(deltaX);
    deltaY = abs(deltaY);
    
    int x = x1;
    int y = y1;
    
    drawGreenPoint(x,y);
    
    if (deltaX >= deltaY) {
        int error = 2 * deltaY - deltaX;
        
        while (x != x2) {
            if ((error >= 0) && (error || (ix > 0)))
            {
                error -= deltaX;
                y += iy;
            }
            
            error += deltaY;
            x += ix;
            
            drawGreenPoint(x, y);
        }
    } else {
        int error = 2 * deltaX - deltaY;
        
        while (y != y2)
        {
            if ((error >= 0) && (error || (iy > 0)))
            {
                error -= deltaY;
                x += ix;
            }
            
            error += deltaX;
            y += iy;
            
            drawGreenPoint(x, y);
        }
    }
}

bool drawWhiteLine(int x1, int y1, int x2, int y2) {
	//Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
    bool ret = false;

    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;
    deltaX = abs(deltaX);
    deltaY = abs(deltaY);

    int x = x1;
    int y = y1;

    drawWhitePoint(x,y);

    if (deltaX >= deltaY) {
        int error = 2 * deltaY - deltaX;

        while (x != x2) {
            if ((error >= 0) && (error || (ix > 0)))
            {
                error -= deltaX;
                y += iy;
            }

            error += deltaY;
            x += ix;


            if (redPixelMatrix[x][y] == 255 && greenPixelMatrix[x][y] == 255 && bluePixelMatrix[x][y] == 255) {
                ret = true;
            }
            drawWhitePoint(x, y);
        }
    } else {
        int error = 2 * deltaX - deltaY;

        while (y != y2)
        {
            if ((error >= 0) && (error || (iy > 0)))
            {
                error -= deltaY;
                x += ix;
            }

            error += deltaX;
            y += iy;


            if (redPixelMatrix[x][y] == 255 && greenPixelMatrix[x][y] == 255 && bluePixelMatrix[x][y] == 255) {
                ret = true;
            }
            drawWhitePoint(x, y);
        }
    }
    return ret;
}


void drawBlackLine(int x1, int y1, int x2, int y2) {
    //Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;
    deltaX = abs(deltaX);
    deltaY = abs(deltaY);

    int x = x1;
    int y = y1;

    drawBlackPoint(x,y);

    if (deltaX >= deltaY) {
        int error = 2 * deltaY - deltaX;

        while (x != x2) {
            if ((error >= 0) && (error || (ix > 0)))
            {
                error -= deltaX;
                y += iy;
            }

            error += deltaY;
            x += ix;

            drawBlackPoint(x, y);
        }
    } else {
        int error = 2 * deltaX - deltaY;

        while (y != y2)
        {
            if ((error >= 0) && (error || (iy > 0)))
            {
                error -= deltaY;
                x += ix;
            }

            error += deltaX;
            y += iy;

            drawBlackPoint(x, y);
        }
    }
}


void drawRedLine(int x1, int y1, int x2, int y2) {
    //Than kode lu gua benerin dikit di sini, harusnya ngk usah pake absolut
    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;
    deltaX = abs(deltaX);
    deltaY = abs(deltaY);

    int x = x1;
    int y = y1;

    drawRedPoint(x,y);

    if (deltaX >= deltaY) {
        int error = 2 * deltaY - deltaX;

        while (x != x2) {
            if ((error >= 0) && (error || (ix > 0)))
            {
                error -= deltaX;
                y += iy;
            }

            error += deltaY;
            x += ix;

            drawRedPoint(x, y);
        }
    } else {
        int error = 2 * deltaX - deltaY;

        while (y != y2)
        {
            if ((error >= 0) && (error || (iy > 0)))
            {
                error -= deltaY;
                x += ix;
            }

            error += deltaX;
            y += iy;

            drawRedPoint(x, y);
        }
    }
}

void floodFill(int x,int y,int redBatas,int greenBatas,int blueBatas,int redColor,int greenColor,int blueColor){
    if((x>=0 && x<WIDTH) && (y>=0 && y<HEIGHT)){
        if(!((redPixelMatrix[x][y]==redBatas && greenPixelMatrix[x][y]==greenBatas && bluePixelMatrix[x][y]==blueBatas) ||
            (redPixelMatrix[x][y]==redColor && greenPixelMatrix[x][y]==greenColor && bluePixelMatrix[x][y]==blueColor))){
            redPixelMatrix[x][y] = redColor;
            greenPixelMatrix[x][y] = greenColor;
            bluePixelMatrix[x][y] = blueColor;
            floodFill(x,y+1,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
            floodFill(x+1,y,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
            floodFill(x,y-1,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
            floodFill(x-1,y,redBatas,greenBatas,blueBatas,redColor,greenColor,blueColor);
        }
    }
}

void drawSemiCircle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawWhitePoint(x0 - x, y0 + y);
        drawWhitePoint(x0 - y, y0 + x);
        drawWhitePoint(x0 - y, y0 - x);
        drawWhitePoint(x0 - x, y0 - y);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }

    //warnain
    floodFill(x0-radius+5,y0-5,255,255,255,255,255,0);
    floodFill(x0+radius-5,y0-5,255,255,255,255,255,0);
}

void drawCircle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawWhitePoint(x0 - x, y0 + y);
        drawWhitePoint(x0 - y, y0 + x);
        drawWhitePoint(x0 - y, y0 - x);
        drawWhitePoint(x0 - x, y0 - y);
        drawWhitePoint(x0 + x, y0 + y);
        drawWhitePoint(x0 + y, y0 + x);
        drawWhitePoint(x0 + y, y0 - x);
        drawWhitePoint(x0 + x, y0 - y);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
}

int detectKeyStroke() {
    //deteksi adanya keyboard yang ditekan.
    //0 jika tidak, >=1 jika iya
    static bool flag = false;
    static const int STDIN = 0;

    if (!flag) {
        //Memakai termios untuk mematikan line buffering
        struct termios T;

        tcgetattr(STDIN, &T);
        T.c_lflag &= ~ICANON;
        tcsetattr(STDIN, TCSANOW, &T);
        setbuf(stdin, NULL);
        flag = true;
    }

    int NByte;
    ioctl(STDIN, FIONREAD, &NByte);  // STDIN = 0

    return NByte;
}


void drawTank(int absis, int ordinat){
    Point P1,P2,P3,P4,P5,P6,P7,P8,P9,P10,P11,P12;
    Point T1,T2,T3,T4;
    Point B1,B2,B3,B4;
     //kiri atas
     P1.x=absis; //10
     P1.y=ordinat; //100
     //kanan atas
     P2.x=100+absis;
     P2.y=ordinat;
     //kiri bawah
     P3.x=absis;
     P3.y=130+ordinat;
     //kanan bawah
     P4.x=100 + absis;
     P4.y=130 + ordinat;

     //Kiri atas 2
     P5.x=20 + absis;
     P5.y=ordinat;

     //Kanan atas 2
     P6.x=80+absis;
     P6.y=ordinat;

     //Kiri atas 3
     P7.x=20+absis;
     P7.y=30+ordinat;

     //Kanan atas 3
     P8.x=80+absis;
     P8.y=30+ordinat;


     //Kiri bawah 2
     P9.x=20+absis;
     P9.y=130+ordinat;

     //Kanan bawah 2
     P10.x=80+absis;
     P10.y=130+ordinat;

     //Kiri bawah 3
     P11.x=20+absis;
     P11.y=100+ordinat;

     //Kanan bawah 3
     P12.x=80+absis;
     P12.y=100+ordinat;

     //Turret kiri bawah
     T1.x=44+absis;
     T1.y=45+ordinat;

     //Turret kanan bawah
     T2.x=56+absis;
     T2.y=45+ordinat;


     //Turret kiri atas
     T3.x=44+absis;
     T3.y=ordinat-25;

     //Turret kanan atas
     T4.x=56+absis;
     T4.y=ordinat-25;

     //Base kiri atas
     B1.x=30+absis;
     B1.y=45+ordinat;

     //Base kanan atas
     B2.x=70+absis;
     B2.y=45+ordinat;

     //Base kiri bawah
     B3.x=30+absis;
     B3.y=85+ordinat;

     //Base kanan bawah
     B4.x=70+absis;
     B4.y=85+ordinat;
        drawRedLine(P1.y,P1.x,P5.y,P5.x);
        drawRedLine(P1.y,P1.x,P3.y,P3.x);
        drawRedLine(P2.y,P2.x,P4.y,P4.x);
        drawRedLine(P10.y,P10.x,P4.y,P4.x);
        drawRedLine(P2.y,P2.x,P6.y,P6.x);
        drawRedLine(P5.y,P5.x,P7.y,P7.x);
        drawRedLine(P6.y,P6.x,P8.y,P8.x);
        drawRedLine(P7.y,P7.x,P8.y,P8.x);
        drawRedLine(P3.y,P3.x,P9.y,P9.x);
        drawRedLine(P9.y,P9.x,P11.y,P11.x);
        drawRedLine(P11.y,P11.x,P12.y,P12.x);
        drawRedLine(P12.y,P12.x,P10.y,P10.x);
        drawRedLine(T1.y,T1.x,T2.y,T2.x);
        drawRedLine(T3.y,T3.x,T4.y,T4.x);
        drawRedLine(T1.y,T1.x,T3.y,T3.x);
        drawRedLine(T2.y,T2.x,T4.y,T4.x);
        drawRedLine(B1.y,B1.x,B2.y,B2.x);
        drawRedLine(B3.y,B3.x,B4.y,B4.x);
        drawRedLine(B1.y,B1.x,B3.y,B3.x);
        drawRedLine(B2.y,B2.x,B4.y,B4.x);
        //Warna dasar
        floodFill(P1.y+1,P1.x+1,255,0,0,150,255,50);
        //Warna base turret
        floodFill(B1.y+1,B1.x+1,255,0,0,100,255,100);
        //Warna turret
        floodFill(T1.y-1,T1.x+1,255,0,0,170,100,100);
        floodFill(T3.y+1,T3.x+1,255,0,0,170,100,100);
}

void drawShooter(int xp, int yp, char mode) {

    drawTank(xp-100, yp-90);
    switch (mode) {
        case'd':
        case 'D': {
            posX = xp+50;
            posY = yp-50;
            break;
        }

        case 's':
        case 'S': {
            posX = xp;
            posY = 500;
            break;
        }


        case 'a':
        case 'A': {
            posX = xp-50;
            posY = yp-50;
            break;
        }
        default: {}
    }
}
int screen_size;
void DrawToScreen(){
    /* prosedure yang menggambar ke layar dari matriks RGB (harusnya rata tengah)*/
    long int location = 0;
    int x , y;
    long int bmp_loc = 0;
    for (y = vinfo.yres/2 - WIDTH/2; y < WIDTH + vinfo.yres/2 - WIDTH/2; y++)
        for (x = vinfo.xres/2 - HEIGHT/2; x < HEIGHT + vinfo.xres/2 - HEIGHT/2; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 32) {
                //4byte
                // printf("%ld %d\n", location,  screen_size);
                if (location < screen_size && location > 0) {
                    *(fbp + location) = bluePixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];        // Some blue
                    *(fbp + location + 1) = greenPixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];     // A little green
                    *(fbp + location + 2) = redPixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency
                }
            //location += 4;
            } else  { //assume 16bpp
                int b = 0;
                int g = 100;     // A little green
                int r = 0;    // A lot of red
                unsigned short int t = r<<11 | g << 5 | b;
                *((unsigned short int*)(fbp + location)) = t;
            }
        }
}

void drawExplosion(int x,int y){
    //x = 70
    // bentuk bintang ada 8 garis sesuai dengan parameter titik pusat (x,y)
    int pointx1 = x-20, pointy1 =y+20;
    int pointx3 = x+20, pointy3 =y+20;
    int pointx5 = x+20, pointy5 =y-20;
    int pointx7 = x-20, pointy7 =y-20;

    int pointx2 = x, pointy2 = y+10;
    int pointx4 = x+10, pointy4 = y;
    int pointx6 = x, pointy6 = y-10;
    int pointx8 = x-10, pointy8 = y;

    //gambar ledakan
    drawRedLine(pointx1,pointy1,pointx2,pointy2);
    drawRedLine(pointx2,pointy2,pointx3,pointy3);
    drawRedLine(pointx3,pointy3,pointx4,pointy4);
    drawRedLine(pointx4,pointy4,pointx5,pointy5);
    drawRedLine(pointx5,pointy5,pointx6,pointy6);
    drawRedLine(pointx6,pointy6,pointx7,pointy7);
    drawRedLine(pointx7,pointy7,pointx8,pointy8);
    drawRedLine(pointx8,pointy8,pointx1,pointy1);

    //warnain
    floodFill(x,y,255,0,0,255,255,0);
}

void drawUFOBody(int x1, int y1) {
    drawWhiteLine(x1, y1, x1+20, y1+20);
    drawWhiteLine(x1, y1, x1, y1-50);
    drawWhiteLine(x1, y1-50, x1+20, y1-70);
    drawWhiteLine(x1+20, y1-70, x1+20, y1+20);

    floodFill(x1+5, y1, 255, 255, 255, 0, 255, 0);
}

void drawPecahan(int x1, int y1) {
	drawWhiteLine(x1, y1, x1+40, y1-20);
	drawWhiteLine(x1+40, y1-20, x1+30, y1-30);
	drawWhiteLine(x1+30, y1-30, x1-10, y1-10);
	drawWhiteLine(x1-10, y1-10, x1, y1);
    floodFill(x1, y1-1, 255, 255, 255, 0, 255, 0);
}



void drawUFO(int x1, int y1) {
    // x1 += 50*sin(0.5*x1); 
    y1 += HEIGHT*sin(0.01*y1);
    drawUFOBody(x1,y1);

    drawSemiCircle(x1, y1-25, 25);
}

void drawUFOSemiCircle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawWhitePoint(x0 - x, y0 + y);
        drawWhitePoint(x0 - y, y0 + x);
        drawWhitePoint(x0 - y, y0 - x);
        drawWhitePoint(x0 - x, y0 - y);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }

    drawWhiteLine(x0, y0+radius, x0, y0-radius);
    //warnain
    floodFill(x0-5,y0,255,255,255,255,255,0);
}



void drawExplosion2(int x,int y){

    drawUFOSemiCircle(x+5, y-50, 25);
    drawUFOBody(x+15, y+10);

}



void drawFrame() {
    drawWhiteLine(0, 0, 0, HEIGHT-1);
    drawWhiteLine(0, HEIGHT-1, WIDTH-1, HEIGHT-1);
    drawWhiteLine(WIDTH-1, HEIGHT-1, WIDTH-1, 0);
    drawWhiteLine(HEIGHT-1, 0, 0, 0);
}

void addBullet(int x1, int y1, int x2, int y2 , int n)
//x1,y1 titik asal peluru
//x2,y2 titik sampai peluru
//n adalah pembagian tahap gerak peluru
{
    bullet newBullet;
    //persamaan garis
    newBullet.m = (y2-y1);
    newBullet.m /= (x2-x1);
    newBullet.c = y1 - newBullet.m * x1;

    newBullet.partisi = 0;
    for (int i=1;i<=n;i++) {
        newBullet.partisi += i;
    }

    newBullet.xStart = x1;
    newBullet.yStart = (int) floor(newBullet.m * newBullet.xStart + newBullet.c + 0.5);
    newBullet.xEnd = x1 + (x2-x1) * n / newBullet.partisi;
    newBullet.yEnd = (int) floor(newBullet.m * newBullet.xEnd + newBullet.c + 0.5);

    newBullet.x1 = x1;
    newBullet.x2 = x2;
    newBullet.iteration = n;
    newBullet.n = n;

    lock_guard<mutex> guard(bullet_mutex);
    bullets.push_back(newBullet);
}

void drawKeyShooter(){
    while(!exploded){
        if(!detectKeyStroke()) {
                char KeyPressed = getchar();
                if ((KeyPressed=='A')||(KeyPressed=='a') ||(KeyPressed=='S') ||(KeyPressed=='s') ||(KeyPressed=='D') ||(KeyPressed=='d')) {
                    lastCorrectState = KeyPressed;
                } else if (KeyPressed==' ') {

                    if (lastCorrectState == 'a')
                        addBullet(posY-75,posX,0,0,20);
                    else if (lastCorrectState == 's')
                        addBullet(posY-45,posX-50,0,600,20);
                    else if (lastCorrectState == 'd')
                        addBullet(posY-75,posX-100,0,1200,20);

            }
        }
    }

}

void drawBullets() {
    lock_guard<mutex> guard(bullet_mutex);
    //persamaan garis
    for (int i = bullets.size()-1; i >=1; --i)
    {
        if (bullets[i].iteration >0) {
            if (drawWhiteLine(bullets[i].xStart,bullets[i].yStart,bullets[i].xEnd,bullets[i].yEnd)) exploded = true;
            bullets[i].xStart = bullets[i].xEnd;
            bullets[i].yStart = bullets[i].yEnd;
            bullets[i].xEnd = bullets[i].xStart + (bullets[i].x2 - bullets[i].x1) * (bullets[i].iteration - 1) / bullets[i].partisi;
            bullets[i].yEnd = (int) floor(bullets[i].m * bullets[i].xEnd + bullets[i].c + 0.5);
            bullets[i].iteration--;
        }
    }
}

/*
    Credit : Bang Radz
    Idenya kaya pake gravitasi : accX -> kecepatan; accY -> kecepatan sumbu horizontal; gravity -> gravitasi;
    Bangun yang mantul : UfoBody
*/
void drawBounce(int xbody, int ybody, int accX,int accY,int xp,int yp,int lastCorrectState) {
    int gravity = 1;
    // Batas bawah layar && Gerakan ke bawah
    while(xbody<550) {
        clearMatrix();
        drawFrame();
        drawShooter(xp,yp,lastCorrectState);
        drawUFOBody(xbody+15,ybody+10);
        // Kecepatan bertambah
        accX += gravity;
        xbody += accX;
        // Batas kanan kiri layar
        if (ybody <= 70) {
            accY = 7;
        } else if (ybody >= 1160) {
            accY = -7;
        }
        ybody += accY;
        DrawToScreen();
        usleep(50000);
    }
    // Jika gerakan ke bawah kecepatannya 0 -> dah berhenti
    while(accX > 0) {
        // Gravity diubah ubah biar enak dilihat
        gravity = 3;
        // Jika accX == 0 -> sudah sampai titik puncak && Gerakan naik
        while (accX > 0) {
            clearMatrix();
            drawFrame();
            drawShooter(xp,yp,lastCorrectState);
            drawUFOBody(xbody+15,ybody+10);
            // Kecepatan turun
            accX -= gravity;
            xbody -= accX;
            // Batas kanan kiri layar
            if (ybody <= 70) {
                accY = 7;
            } else if (ybody >= 1160) {
                accY = -7;
            }
            ybody += accY;
            DrawToScreen();
            usleep(50000);
        }
        // Gravity diubah ubah biar enak dilihat
        gravity = 2;
        // Batas bawah layar & Gerakan ke bawah
        while (xbody < 560) {
            clearMatrix();
            drawFrame();
            drawShooter(xp,yp,lastCorrectState);
            drawUFOBody(xbody+15,ybody+10);
            // Kecepatan naik
            accX += gravity;
            xbody += accX;
            // Batas kanan kiri layar
            if (ybody <= 70) {
                accY = 7;
            } else if (ybody >= 1160) {
                accY = -7;
            }
            ybody += accY;
            DrawToScreen();
            usleep(50000);
        }
    }
}

void drawPecahanJatuh(int x1, int y1) {
	int i = 0;
	while (x1<400) {
	    clearMatrix();
	    drawFrame();
		drawPecahan(x1+i, y1 + i/2);
        DrawToScreen();
        usleep(50000);
		i = i+10;
	}
}

//Algoritma translasi point
//x1 dan y1 adalah titik awal
//dx dan dy adalah jarak translasi
//Mengembalikan point hasil translasi
Point pointTranslation(int x1, int y1, int x2, int y2, int dx, int dy){
	Point retval;
	retval.x=x1+dx;
	retval.x=x1+dy;
	return retval;
}

//Algoritma rotasi dengan x1 dan y1 sebagai titik pivot
//Mengembalikan Point, yaitu titik baru hasil rotasi
//Algoritma sama dengan prosedur lineRotation
//Point hasil return bisa digunakan untuk drawLine ataupun drawPoint
Point lineRotate(int x1, int y1, int x2, int y2, double derajatRotasi){
	double degree;
	//jarak dari origin
	int xdiff=x1;
	int ydiff=y1;
	//mengubah sudut menjadi representasi yang sesuai
	degree=(22*derajatRotasi)/(180*7);

	x2=x2-xdiff;
	y2=y2-ydiff;

	//Titik akhir setelah rotasi
	int x3,y3;
	x3=((x2*cos(degree))-(y2*sin(degree)))+xdiff;
	y3=((x2*sin(degree))+(y2*cos(degree)))+ydiff;
	Point retval;
	retval.x=x3;
	retval.y=y3;
	return retval;
}


//x1 dan y1 adalah titik pivot rotasi, alias titik yang akan diam saja
//x2 dan y2 adalah titik yang dirotasi
//Untuk sementara, algo dibuat untuk garis merah (menyesuaikan dengan
//Algo yang ada sebelumnya)
void lineRotation(int x1, int y1, int x2, int y2, double derajatRotasi){
	double degree;
	//jarak dari origin
	int xdiff=x1;
	int ydiff=y1;
	//mengubah sudut menjadi representasi yang sesuai
	degree=(22*derajatRotasi)/(180*7);

	x2=x2-xdiff;
	y2=y2-ydiff;

	//Titik akhir setelah rotasi
	int x3,y3;
	x3=((x2*cos(degree))-(y2*sin(degree)))+xdiff;
	y3=((x2*sin(degree))+(y2*cos(degree)))+ydiff;
	drawWhiteLine(x1,y1,x3,y3);
}

void rotateSemiCircle(int x0, int y0, int radius, int position) {
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        switch(position) {
            case 1:
                drawWhitePoint(x0 - x, y0 + y); //kanan atas1
                drawWhitePoint(x0 - y, y0 + x); //kanan atas2
                drawWhitePoint(x0 - y, y0 - x); //kiri atas2
                drawWhitePoint(x0 - x, y0 - y); //kiri atas1
                drawWhiteLine(x0, y0+radius, x0, y0-radius);
                break;
            case 2:
                drawWhitePoint(x0 - x, y0 - y); //kiri atas1
                drawWhitePoint(x0 - x, y0 + y); //kanan atas1
                drawWhitePoint(x0 - y, y0 + x); //kanan atas2
                drawWhitePoint(x0 + y, y0 + x); //kanan bawah2
                lineRotation(x0, y0, x0, y0-radius, -45);
                lineRotation(x0, y0, x0, y0+radius, -45);
                break;
            case 3:
                drawWhitePoint(x0 - x, y0 + y); //kanan atas1
                drawWhitePoint(x0 - y, y0 + x); //kanan atas2
                drawWhitePoint(x0 + y, y0 + x); //kanan bawah2
                drawWhitePoint(x0 + x, y0 + y); //kanan bawah1
                lineRotation(x0, y0, x0, y0-radius, -90);
                lineRotation(x0, y0, x0, y0+radius, -90);
                break;
            case 4:
                drawWhitePoint(x0 - y, y0 + x); //kanan atas2
                drawWhitePoint(x0 + x, y0 + y); //kanan bawah1
                drawWhitePoint(x0 + y, y0 + x); //kanan bawah2
                drawWhitePoint(x0 + x, y0 - y); //kiri bawah1
                lineRotation(x0, y0, x0, y0-radius, -135);
                lineRotation(x0, y0, x0, y0+radius, -135);
                break;
            case 5:
                drawWhitePoint(x0 + x, y0 + y); //kanan bawah1
                drawWhitePoint(x0 + y, y0 + x); //kanan bawah2
                drawWhitePoint(x0 + y, y0 - x); //kiri bawah2
                drawWhitePoint(x0 + x, y0 - y); //kiri bawah1
                drawWhiteLine(x0, y0+radius, x0, y0-radius);
                break;
            case 6:
                drawWhitePoint(x0 + x, y0 + y); //kanan bawah1
                drawWhitePoint(x0 + y, y0 - x); //kiri bawah2
                drawWhitePoint(x0 + x, y0 - y); //kiri bawah1
                drawWhitePoint(x0 - y, y0 - x); //kiri atas2
                lineRotation(x0, y0, x0, y0-radius, -45);
                lineRotation(x0, y0, x0, y0+radius, -45);
                break;
            case 7:
                drawWhitePoint(x0 + y, y0 - x); //kiri bawah2
                drawWhitePoint(x0 + x, y0 - y); //kiri bawah1
                drawWhitePoint(x0 - y, y0 - x); //kiri atas2
                drawWhitePoint(x0 - x, y0 - y); //kiri atas1
                lineRotation(x0, y0, x0, y0-radius, -90);
                lineRotation(x0, y0, x0, y0+radius, -90);
                break;
            case 8:
                drawWhitePoint(x0 + y, y0 - x); //kiri bawah2
                drawWhitePoint(x0 - y, y0 - x); //kiri atas2
                drawWhitePoint(x0 - x, y0 - y); //kiri atas1
                drawWhitePoint(x0 - x, y0 + y); //kanan atas1
                lineRotation(x0, y0, x0, y0-radius, -135);
                lineRotation(x0, y0, x0, y0+radius, -135);
                break;
            default:
                break;

        }

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }

    switch(position) {
        case 1:
        case 2:
            floodFill(x0-5,y0,255,255,255,255,255,0);
            break;
        case 3:
        case 4:
            floodFill(x0,y0+5,255,255,255,255,255,0);
            break;
        case 5:
        case 6:
            floodFill(x0+5,y0,255,255,255,255,255,0);
            break;
        case 7:
        case 8:
            floodFill(x0,y0-5,255,255,255,255,255,0);
            break;
        default:
            break;
    }
}

void clearCircle(int x0, int y0, int radius) {
        int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawBlackPoint(x0 - x, y0 + y);
        drawBlackPoint(x0 - y, y0 + x);
        drawBlackPoint(x0 - y, y0 - x);
        drawBlackPoint(x0 - x, y0 - y);
        drawBlackPoint(x0 + x, y0 + y);
        drawBlackPoint(x0 + y, y0 + x);
        drawBlackPoint(x0 + y, y0 - x);
        drawBlackPoint(x0 + x, y0 - y);

        if (err <= 0)
        {
            y += 1;
            err += 2*y + 1;
        }
        if (err > 0)
        {
            x -= 1;
            err -= 2*x + 1;
        }
    }
    floodFill(x0,y0,0,0,0,0,0,0);
}

void fallingSemiCircle(int x, int y, int xp, int yp, int r, int lastCorrectState) {
//x, y: posisi awal; r: radius

    int yspeed = 7; //kecepatan horizontal
    int num; //nentuin semicircle rotasi ke arah mana
    int it = 1;
    while (x < 590) {
        clearMatrix();
        drawFrame();

        drawShooter(xp,yp,lastCorrectState);

        num = (it % 8);
        rotateSemiCircle(x,y,25,num); //semicircle muter
        usleep(25000);

        if (y <= 70) {
            yspeed = 7;
        } else if (y >= 1160) {
            yspeed = -7;
        }

        x += 10;
        y += yspeed;
        ++it;
        DrawToScreen();
        usleep(25000);
    }

    while (x < 610) {
        clearMatrix();
        drawFrame();

        drawShooter(xp,yp,lastCorrectState);

        drawUFOSemiCircle(x,y,25);
        usleep(25000);
        //clearCircle(xawal,yawal,25);

        if (y <= 70) {
            yspeed = 7;
        } else if (y >= 1160) {
            yspeed = -7;
        }

        x += 10;
        y += yspeed;
        DrawToScreen();
        usleep(25000);
    }
}

int main(int argc, char const *argv[]) {

    clearMatrix();

    int fbfd = 0;
    long int screensize = 0;
    exploded = false;

    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }

    // Get fixed screen information
    if (ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        exit(2);
    }
    // Get variable screen information
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        exit(3);
    }

    // mendapat screensize layar monitor
    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    screen_size = screensize;

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0);

    //display merge center
    // Menulis ke layar tengah file
    //Gambar trapesium
    thread thread1(drawKeyShooter);
    int xp = 600;
    int yp = 574;
    char KeyPressed;

    int xawal = 100, yawal = HEIGHT;
    bool left = true;

    do {
        usleep(50000);
        clearMatrix();
        drawFrame();

        drawShooter(xp,yp,lastCorrectState);

        // draw UFO
        drawUFO(xawal, yawal);
        if(yawal-70<=0) {
            left = false;
        } else if(yawal+20>=1200) {
            left = true;
        }
        if (left) {
            yawal -= 10;
        } else {
            yawal += 10;
        }

        // draw bullet
        drawBullets();

        DrawToScreen();
    } while (KeyPressed!='C' && !exploded);

    thread1.detach();
    clearMatrix();
    //drawFrame();
    drawShooter(xp,yp,lastCorrectState);
    //drawStars();
    int proportion = 2;
    int accX = 12;
    int accY;
    if (left) {
        accY = -7;
    } else {
        accY = 7;
    }


    if (fork()) {
        drawBounce(xawal,yawal,accX,accY,xp,yp,lastCorrectState);
    } else {
        if (fork()) {
            fallingSemiCircle(xawal,yawal,xp,yp,25,lastCorrectState);
        } else {
            drawPecahanJatuh(xawal, yawal);
        }
    }
    /*------*/
    //DrawToScren();

    

    munmap(fbp, screensize);
    close(fbfd);

    return 0;
}
