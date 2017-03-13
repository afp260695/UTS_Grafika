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
#include <pthread.h>
#include <vector>

using namespace std;

#define WIDTH 1300
#define HEIGHT 700
#define lookSize 600
#define jumlahTarget 6
pthread_t d_Drone;
#define N_TARGET 6

typedef struct{
    int x;
    int y;
} Point;

typedef struct rgb {
    double r;       // between 0 and 255
    double g;       // between 0 and 255
    double b;       // between 0 and 255
    static const rgb WHITE;
    static const rgb GRAY;
    static const rgb YELLOW;
    static const rgb BLACK;
    static const rgb GREEN;
    static const rgb RED;
    static const rgb MAGENTA;
    static const rgb DARKGRAY;
    static const rgb TARGET1;
    static const rgb TARGET2;
    static const rgb TARGET3;
    static const rgb TARGET4;
    static const rgb TARGET5;
    static const rgb TARGET6;
    bool operator== (const rgb &c) {
        return (r == c.r && g == c.g && b == c.b);
    }
} rgb;

const rgb rgb::WHITE = {255, 255, 255};
const rgb rgb::GRAY = {128, 128, 128};
const rgb rgb::YELLOW = {255, 255, 0};
const rgb rgb::BLACK = {0, 0, 0};
const rgb rgb::GREEN = {0, 255, 0};
const rgb rgb::RED = {255, 0, 0};
const rgb rgb::MAGENTA = {255, 0, 255};
const rgb rgb::DARKGRAY = {112, 128, 144};
const rgb rgb::TARGET1 = {254, 0, 0};
const rgb rgb::TARGET2 = {253, 0, 0};
const rgb rgb::TARGET3 = {252, 0, 0};
const rgb rgb::TARGET4 = {251, 0, 0};
const rgb rgb::TARGET5 = {250, 0, 0};
const rgb rgb::TARGET6 = {249, 0, 0};


typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;


const int MAP_WIDTH = 363;
const int MAP_HEIGHT = 419;

static rgb   hsv2rgb(hsv in);

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;

rgb pixelMatrix[WIDTH][HEIGHT];
int posX;
int posY;
int lastCorrectState = 's';
bool exploded = false;

int mode = 0;
//Array
rgb targetColor[6];
bool isTargetShot[6];

int arrkepalax[jumlahTarget];
int arrkepalay[jumlahTarget];
int arrbadanx[jumlahTarget];
int arrbadany[jumlahTarget];
int arrkecx[jumlahTarget];
int arrkecy[jumlahTarget];
int arrtargetdir[jumlahTarget];
int arrtargetujung[jumlahTarget];
bool arrstop[jumlahTarget];

int arrtargetx[jumlahTarget];
int arrtargety[jumlahTarget];
rgb arrwarnatarget[jumlahTarget];
bool arrtargetalive[jumlahTarget];


rgb clipperMatrix[lookSize-250][lookSize-250];

rgb petafix[lookSize][lookSize];

int x_god;
int y_god;

int size_god;

unsigned char* BMP;
int BMP_width, BMP_height;

typedef struct {
    vector<int> points;
} Building;

bool pohon = true;
bool jalan = true;
bool bangunan = true;

void clearMatrix() {
    for (int i = 0; i < WIDTH; ++i)
        for (int j = 0; j < HEIGHT; ++j)
            pixelMatrix[i][j] = {0, 0, 0};
}

void clearClipperMatrix() {
    for (int i = 0; i < 400; ++i)
        for (int j = 0; j < 400; ++j)
            clipperMatrix[i][j] = {0, 0, 0};
}

void drawPoint(rgb color, int x, int y) {
    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
    pixelMatrix[x][y] = color;
}

bool drawLine(rgb color, int x1, int y1, int x2, int y2) {
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

    drawPoint(color, x, y);

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
            drawPoint(color, x, y);
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
            drawPoint(color, x, y);
        }
    }
    return ret;
}

void floodFill(int x, int y, rgb batas, rgb warna){
    if((x>=0 && x<WIDTH) && (y>=0 && y<HEIGHT)){
        if(
            !(
                (pixelMatrix[x][y] == batas) ||
                (pixelMatrix[x][y] == warna)
            )
        ){
            pixelMatrix[x][y] = warna;
            floodFill(x,y+1, batas, warna);
            floodFill(x+1,y, batas, warna);
            floodFill(x,y-1, batas, warna);
            floodFill(x-1,y, batas, warna);
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


int screen_size;
void DrawToScreen(){
    /* prosedure yang menggambar ke layar dari matriks RGB (harusnya rata tengah)*/
    long int location = 0;
    int x , y;
    long int bmp_loc = 0;
    for (y = 0; y < HEIGHT; y++)
        for (x = 0; x < WIDTH; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 32) {
                //4byte
                // printf("%ld %d\n", location,  screen_size);
                if (location < screen_size && location > 0) {
                    *(fbp + location) = pixelMatrix[x][y].b;        // Some blue
                    *(fbp + location + 1) = pixelMatrix[x][y].g;     // A little green
                    *(fbp + location + 2) = pixelMatrix[x][y].r;    // A lot of red
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
	drawLine(rgb::WHITE, x1,y1,x3,y3);
}

//drawWhiteJalan(int x1, int y1, int x2, int y2) {}

void DrawJalan(int x, int y) {

    rgb roadColor = rgb::GRAY;
	drawLine(roadColor,58,505,54,518);drawLine(roadColor,55,505,51,518);
	drawLine(roadColor,58,505,11,350);drawLine(roadColor,54,505,9,350);
	drawLine(roadColor,56,509,189,471);drawLine(roadColor,56,504,189,466);
	drawLine(roadColor,191,470,186,518);drawLine(roadColor,188,470,184,518);
	drawLine(roadColor,189,473,226,468);drawLine(roadColor,189,468,226,463);
	drawLine(roadColor,226,466,359,504);drawLine(roadColor,226,464,359,501);
	drawLine(roadColor,360,503,360,518);drawLine(roadColor,356,503,356,518);
	drawLine(roadColor,359,505,445,505);drawLine(roadColor,359,501,445,501);
	drawLine(roadColor,385,289,388,371);drawLine(roadColor,381,289,384,371);
	drawLine(roadColor,385,371,365,375);drawLine(roadColor,385,371,365,375);
	drawLine(roadColor,365,378,364,421);drawLine(roadColor,365,374,364,418);
	drawLine(roadColor,364,421,359,505);drawLine(roadColor,364,418,359,501);
	drawLine(roadColor,335,289,335,15);drawLine(roadColor,331,289,331,15);
	drawLine(roadColor,335,63,429,70);drawLine(roadColor,331,63,425,70);
	drawLine(roadColor,333,141,408,148);drawLine(roadColor,333,138,408,144);
	drawLine(roadColor,333,168,408,173);drawLine(roadColor,333,164,408,169);
	drawLine(roadColor,230,465,230,419);drawLine(roadColor,223,465,223,419);
	drawLine(roadColor,226,421,89,421);drawLine(roadColor,226,416,89,416);
	drawLine(roadColor,230,419,230,289);drawLine(roadColor,223,419,223,289);
	drawLine(roadColor,226,291,73,291);drawLine(roadColor,226,288,73,288);
	drawLine(roadColor,73,291,29,291);drawLine(roadColor,73,288,29,288);
	drawLine(roadColor,226,291,383,291);drawLine(roadColor,226,288,383,288);
	drawLine(roadColor,226,421,364,421);drawLine(roadColor,226,418,364,418);
	drawLine(roadColor,230,289,230,200);drawLine(roadColor,223,289,223,200);
	drawLine(roadColor,230,200,230,104);drawLine(roadColor,223,200,223,104);
	drawLine(roadColor,230,36,230,20);drawLine(roadColor,223,36,223,20);
	drawLine(roadColor,100,23,226,23);drawLine(roadColor,100,19,226,19);
	drawLine(roadColor,226,23,274,1);drawLine(roadColor,226,19,274,0);
	drawLine(roadColor,28,289,28,368);drawLine(roadColor,30,289,30,368);
	drawLine(roadColor,29,366,55,411);drawLine(roadColor,29,369,55,414);
	drawLine(roadColor,55,411,89,418);drawLine(roadColor,55,414,89,420);
	drawLine(roadColor,71,289,71,394);drawLine(roadColor,74,289,74,394);
	drawLine(roadColor,71,394,88,419);drawLine(roadColor,74,394,90,419);
	drawLine(roadColor,26,289,29,216);drawLine(roadColor,31,289,34,216);
	drawLine(roadColor,31,214,103,214);drawLine(roadColor,31,219,103,219);
	drawLine(roadColor,103,215,119,215);drawLine(roadColor,103,218,119,218);
	drawLine(roadColor,118,216,118,249);drawLine(roadColor,120,216,120,249);
	drawLine(roadColor,119,248,128,251);drawLine(roadColor,119,250,128,254);
	drawLine(roadColor,126,253,128,289);drawLine(roadColor,129,253,130,289);
	drawLine(roadColor,134,104,135,35);drawLine(roadColor,130,104,131,35);
	drawLine(roadColor,134,38,333,44);drawLine(roadColor,134,34,333,40);
	drawLine(roadColor,104,216,104,104);drawLine(roadColor,101,216,101,104);
	drawLine(roadColor,103,106,333,106);drawLine(roadColor,103,103,333,103);
	drawLine(roadColor,103,150,333,150);drawLine(roadColor,103,146,333,146);
	drawLine(roadColor,103,203,333,203);drawLine(roadColor,103,199,333,199);
	drawLine(roadColor,34,216,36,58);drawLine(roadColor,30,216,33,58);
	drawLine(roadColor,36,59,65,36);drawLine(roadColor,34,56,63,34);
	drawLine(roadColor,65,36,101,36);drawLine(roadColor,63,34,99,34);
	drawLine(roadColor,103,35,103,20);drawLine(roadColor,99,35,99,20);
	drawLine(roadColor,100,21,60,21);drawLine(roadColor,100,19,60,19);
	drawLine(roadColor,61,21,28,64);drawLine(roadColor,59,19,25,61);
	drawLine(roadColor,28,63,11,350);drawLine(roadColor,25,63,9,350);

}

void drawFrame() {
    drawLine(rgb::WHITE, 0, 0, 1200, 0);
    drawLine(rgb::WHITE, 1200, 0, 1200, 600);
    drawLine(rgb::WHITE, 1200, 600, 0, 600);
    drawLine(rgb::WHITE, 0, 600, 0, 0);
}

void fire() {
    for (int i = 0; i < jumlahTarget; i++) {
        if (arrwarnatarget[i] == clipperMatrix[(lookSize-250)/2][(lookSize-250)/2]) {
            arrtargetalive[i] = false;
        }
    }
}

void drawKeyClip(){

    while (!detectKeyStroke()) {
        char KeyPressed = getchar();
        if ((KeyPressed=='D') ||(KeyPressed=='d')) {
            y_god++;
        } else if ((KeyPressed=='A') ||(KeyPressed=='a')) {
            y_god--;
        } else if ((KeyPressed=='S') ||(KeyPressed=='s')) {
            x_god++;
        } else if ((KeyPressed=='W') ||(KeyPressed=='w')) {
            x_god--;
        } else if ((KeyPressed=='I') ||(KeyPressed=='i')) {
            size_god--;
        } else if ((KeyPressed=='O') ||(KeyPressed=='o')) {
            size_god++;
        } else if ((KeyPressed=='P') ||(KeyPressed=='p')) {
            pohon = !pohon;
        } else if ((KeyPressed=='J') ||(KeyPressed=='j')) {
            jalan = !jalan;
        } else if ((KeyPressed=='B') ||(KeyPressed=='b')) {
            bangunan = !bangunan;
        } else if ((KeyPressed=='M') ||(KeyPressed=='m')) {
            mode = !mode;
        } else if ((KeyPressed==' ')) {
            fire();
        }
    }
}

void drawClipPointer() {
    int center = (lookSize-250)/2;
    if (mode == 0) {
        for(int i = 1; i < 10; i++) {
            clipperMatrix[center + i][center] = rgb::WHITE;
            clipperMatrix[center - i][center] = rgb::WHITE;
            clipperMatrix[center][center + i] = rgb::WHITE;
            clipperMatrix[center][center - i] = rgb::WHITE;
        }
    } else {
        for(int i = 0; i < 3; i++) {
            for (int j = 0; j < 3-i; j++) {
                if (!(i == 0 && j == 0)) {
                    clipperMatrix[center + i][center+j] = rgb::WHITE;
                    clipperMatrix[center + i][center-j] = rgb::WHITE;
                    clipperMatrix[center - i][center+j] = rgb::WHITE;
                    clipperMatrix[center - i][center-j] = rgb::WHITE;
                }
            }
        }
    }
}

void clipper(int y, int x, int size) {
    int i, j;

    drawLine(rgb::YELLOW, x-1,y-1,x+size,y-1);
    drawLine(rgb::YELLOW, x+size,y-1,x+size,y+size);
    drawLine(rgb::YELLOW, x+size,y+size,x-1,y+size);
    drawLine(rgb::YELLOW, x-1,y+size,x-1,y-1);

    clearClipperMatrix();

    for (i=0;i<lookSize-250;i++) {
        for (j=0;j<lookSize-250;j++) {
            clipperMatrix[i][j] = pixelMatrix[x+(i*size/(lookSize-250))][y+(j*size/(lookSize-250))];
        }
    }

    drawClipPointer();
}


void drawClips(int y, int x, int size) {
    int i,j;
    /*ofstream peta("peta.txt");

    peta << "ngewe";*/
    drawLine(rgb::YELLOW, x-1,y-1,x+size,y-1);
    drawLine(rgb::YELLOW, x+size,y-1,x+size,y+size);
    drawLine(rgb::YELLOW, x+size,y+size,x-1,y+size);
    drawLine(rgb::YELLOW, x-1,y+size,x-1,y-1);

    for (i=0;i<size;i++) {
        for (j=0;j<size;j++) {
            pixelMatrix[x+i][y+j] = clipperMatrix[i][j];
            /*peta << clipperMatrix[i][j].r;
            peta << "\n";
            peta << clipperMatrix[i][j].g;
            peta << "\n";
            peta << clipperMatrix[i][j].b;
            peta << "\n";
            */
        }
    }
    //peta.close();

}

//
//  pohon.cpp
//
//
//  Created by Raudi on 2/28/17.
//
//

/*

 misal size 5 kurang lebih gini
 | | |*| | |
 | |*| |*| |
 | |*| |*| |
 |*| | | |*|
 |*|*|*|*|*|

*/
void drawPohon(int x, int y, int size) {
    drawLine(rgb::GREEN, x+size-1-(size-1)/2, y, x, y+size-1);
    drawLine(rgb::GREEN, x+(size-1)/2, y, x+size-1, y+size-1);
    drawLine(rgb::GREEN, x, y+size-1, x+size-1, y+size-1);
    floodFill(x+(size-1)/2, y+(size-1)/2, rgb::GREEN, rgb::GREEN);

}

void drawAllPohon() {
    drawPohon(26,18,13);
	drawPohon(50,50,9);
	drawPohon(50,68,9);
	drawPohon(40,81,8);
	drawPohon(40,68,9);
	drawPohon(99,91,9);
	drawPohon(110,78,14);
	drawPohon(43,110,10);
	drawPohon(43,128,10);
	drawPohon(79,176,13);
	drawPohon(160,111,13);
	drawPohon(160,128,13);
	drawPohon(160,156,13);
	drawPohon(160,179,13);
	drawPohon(48,224,16);
	drawPohon(40,263,21);
	drawPohon(243,20,13);
	drawPohon(270,20,13);
	drawPohon(291,20,13);
	drawPohon(313,20,13);
	drawPohon(396,20,36);
	drawPohon(421,165,20);
	drawPohon(314,154,13);
	drawPohon(314,179,13);
	drawPohon(314,110,13);
	drawPohon(314,128,13);
	drawPohon(416,220,24);
	drawPohon(410,271,30);
	drawPohon(395,328,40);
	drawPohon(386,386,45);
	drawPohon(345,109,15);
}

// x and y are center point
void drawDrone (int x, int y, int width, int length, rgb dcolor) {
	int wbawah = width/4;
	//badannya
	drawLine(dcolor, x-width, y, x+width, y);
	drawLine(dcolor, x+width, y, x+width, y+(wbawah/2));
	drawLine(dcolor, x+width, y+(wbawah/2), x+(width-10), y+(wbawah/2));
	drawLine(dcolor, x+(width-10), y+(wbawah/2), x+wbawah, y+length);
	drawLine(dcolor, x+wbawah, y+length, x-wbawah, y+length);
	drawLine(dcolor, x-wbawah, y+length, x-(width-10), y+(wbawah/2));
	drawLine(dcolor, x-(width-10), y+(wbawah/2), x-width, y+(wbawah/2));
	drawLine(dcolor, x-width, y+(wbawah/2), x-width, y);
	//tangan
	drawLine(dcolor, x+width-5, y, x+width-5, y-10);
	drawLine(dcolor, x-width, y-10, x-width+10, y-10);
	drawLine(dcolor, x-width+5, y, x-width+5, y-10);
	drawLine(dcolor, x+width, y-10, x+width-10, y-10);
}

int rotateX (int x, int y, int degree) {
	float t = 3.14*degree/180;
	float c = cos(t);
	float s = sin(t);
	int xt = 200;
	int yt = 100;
	x -= xt;
	y -= yt;
	float xtemp = x * c - y * s;
	return (xtemp + xt);
}

int rotateY (int x, int y, int degree) {
	float t = 3.14*degree/180;
	float c = cos(t);
	float s = sin(t);
	int xt = 200;
	int yt = 100;
	x -= xt;
	y -= yt;
	float ytemp = x * s + y * c;
	return (ytemp + yt);
}

void readBangunanFromFile (ifstream& myfile, vector<Building> &B) {
    myfile.open ("bangunan2.txt");

    char c;
    char num[3]; int countchar = 0; int countline = 0;
    int temp;
    Building pointTemp;
    int i = 0;

	while (!myfile.eof() ) {
        myfile.get(c);
        if (c == '*') {
            B.push_back(pointTemp);
            pointTemp.points.clear();
            myfile.get(c);
        } else if ((c == ',') || (c == '|')) {
        } else {
             //cout << "$";
            num[countchar] = c;
            if (countchar < 2) {
                countchar++;
            } else {
                countchar = 0;
                temp = atoi(num);
                pointTemp.points.push_back(temp);
                i++;
            }
        }
	}

    myfile.close();
}

void drawBangunan(vector<Building> &B) {
    for (int i = 0; i < B.size() ; i++) {
        int j;
        for (j= 0; j < (B[i].points.size()-2); j += 2) {
            drawLine(rgb::WHITE, B[i].points[j], B[i].points[j+1], B[i].points[j+2], B[i].points[j+3]);
        }
        //floodFill(B[i].points[j]+1, B[i].points[j+1]+1, 255,255,255,255,122,122);
    }
    // DrawToScreen();
}


rgb hsv2rgb(hsv in)
{
    double      hh, p, q, t, ff;
    long        i;
    rgb         out;

    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
        out.r = in.v;
        out.g = in.v;
        out.b = in.v;
        return out;
    }
    hh = in.h;
    if(hh >= 360.0) hh = 0.0;
    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = in.v * (1.0 - in.s);
    q = in.v * (1.0 - (in.s * ff));
    t = in.v * (1.0 - (in.s * (1.0 - ff)));

    switch(i) {
    case 0:
        out.r = in.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = in.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = in.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = in.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = in.v;
        break;
    case 5:
    default:
        out.r = in.v;
        out.g = p;
        out.b = q;
        break;
    }
    out.r *= 255;
    out.g *= 255;
    out.b *= 255;
    return out;
}

void colorBangunan() {
    floodFill(3, 3, rgb::WHITE, rgb::GRAY);
    double hue = 0;
    for (int i = 0; i < MAP_WIDTH; ++i)
    {
        for (int j = 0; j < MAP_HEIGHT; ++j)
        {
            if (pixelMatrix[i][j] == rgb::BLACK)
            {
                hsv buildingColor = {hue, 0.7, 0.8};
                floodFill(i, j, rgb::WHITE, hsv2rgb(buildingColor));
                hue += 10;
                if (hue > 360)
                {
                    hue = 0;
                }
            }
        }
    }
    floodFill(3, 3, rgb::WHITE, rgb::BLACK);
}

void inputPeta() {
    ifstream peta_file("peta_tanpa_ngewe.txt");
    string line;
    string::size_type sz;
    rgb temp;
    for (int i=0;i<lookSize;i++) {
        for (int j=0;j<lookSize;j++) {
            getline(peta_file,line);
            temp.r = atof(line.c_str());
            getline(peta_file,line);
            temp.g = atof(line.c_str());
            getline(peta_file,line);
            temp.b = atof(line.c_str());
            petafix[i][j] = temp;
        }
    }
    peta_file.close();
}

void drawKepala(int x, int y, int scaling, rgb warna, rgb batas){
    drawLine(batas, x, y, x+2*scaling, y);
    drawLine(batas, x+2*scaling, y, x+2*scaling, y+2*scaling);
    drawLine(batas, x, y, x, y+2*scaling);
    drawLine(batas, x, y+2*scaling, x+2*scaling, y+2*scaling);
    floodFill(x+1, y+1, rgb::WHITE, warna);
}

void drawBadan(int x, int y, int scaling, rgb warna, rgb batas){
     drawLine(batas, x-2*scaling, y+2*scaling, x+4*scaling, y+2*scaling);
    drawLine(batas, x-2*scaling, y+2*scaling, x-2*scaling, y+6*scaling);
    drawLine(batas, x-2*scaling, y+2*scaling, x-2*scaling, y+6*scaling);
    drawLine(batas, x-2*scaling, y+6*scaling, x, y+6*scaling);
    drawLine(batas, x, y+6*scaling, x, y+4*scaling);
    drawLine(batas, x, y+6*scaling, x, y+8*scaling);
    drawLine(batas, x, y+8*scaling, x+2*scaling, y+8*scaling);
    drawLine(batas, x+2*scaling, y+8*scaling, x+2*scaling, y+4*scaling);
    drawLine(batas, x+4*scaling, y+2*scaling, x+4*scaling, y+6*scaling);
    drawLine(batas, x+4*scaling, y+6*scaling, x+2*scaling, y+6*scaling);
    floodFill(x+1, y+3*scaling, rgb::WHITE, warna);
}

void drawTarget(int x, int y, int scaling, rgb warna, rgb batas){
    //kelapa
    drawKepala(x,y,scaling,warna,batas);

    //badan
    drawBadan(x,y,scaling,warna,batas);
    //floodfill

}


void destroyTarget(int x, int y, int scaling, rgb warna,rgb batas){
    int kecx = -1;
    int kecy = 1;
    bool stop = false;
    while(!stop){
        kecy = 0;
        while (y < y+8*scaling){
            x += kecx;
            y += kecy;
            kecy++;
        }
        if (kecy < 2) {
            stop = true;
        }
        while (kecy > 0) {
            x += kecx;
            y -= kecy;
            kecy -= 2;
        }
        drawKepala(x,y,scaling,warna,batas);
        sleep(1000);
    }
}

void drawCircle(int x0, int y0, int radius)
{
    int x = radius;
    int y = 0;
    int err = 0;

    while (x >= y)
    {
        drawPoint(rgb::WHITE, x0 - x, y0 + y);
        drawPoint(rgb::WHITE, x0 - y, y0 + x);
        drawPoint(rgb::WHITE, x0 - y, y0 - x);
        drawPoint(rgb::WHITE, x0 - x, y0 - y);
        drawPoint(rgb::WHITE, x0 + x, y0 + y);
        drawPoint(rgb::WHITE, x0 + y, y0 + x);
        drawPoint(rgb::WHITE, x0 + y, y0 - x);
        drawPoint(rgb::WHITE, x0 + x, y0 - y);

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
    floodFill(x0,y0,rgb::WHITE,rgb::GREEN);
}

void *drawDroneThread(void *args){
		//draw drone
        int length = 50;
		int width = 20;
		int x = 200;
		int y = 200;
		while (1) {
			int x_ = rotateX (x, y, 36);
			int y_ = rotateY (x, y, 36);
			drawDrone (x_, y_, length, width, rgb::WHITE);
			floodFill(x_, y_+10, rgb::WHITE, rgb::GRAY);
			sleep(1);
			floodFill(x_, y_+10, rgb::WHITE, rgb::BLACK);
			drawDrone (x_, y_, length, width, rgb::BLACK);
			x = x_; y = y_;
		}
}

int main(int argc, char const *argv[]) {
    clearMatrix();

    int fbfd = 0;
    long int screensize = 0;
    bool exploded = false;

    x_god = 0;
    y_god = 0;
    size_god = 420;

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
    thread thread1(drawKeyClip);

    int xp = 600;
    int yp = 574;
    char KeyPressed;

    int xawal = 100, yawal = 150;
    bool left = true;
    inputPeta();
    //pthread_create(&d_Drone, NULL, drawDroneThread, NULL);
	int length = 50;
	int width = 20;
	int x = 200;
	int y = 200;
	int i = 0;
	int posx = 100;
	int posy = 100;
	int direction = 0;
	int scaling = 2;

	for(int i = 0;i < jumlahTarget;i++) {
		arrkecy[i] = 0;
		arrkecx[i] = 1;
		arrtargetx[i] = 25*(i+1);
		arrtargety[i] = 25*(i+1);
		arrtargetdir[i] = 0;
		arrstop[i] = false;
		arrtargetalive[i] = true;
		arrtargetujung[i] = 0;
	}
	arrwarnatarget[0] = rgb::TARGET1;
	arrwarnatarget[1] = rgb::TARGET2;
	arrwarnatarget[2] = rgb::TARGET3;
	arrwarnatarget[3] = rgb::TARGET4;
	arrwarnatarget[4] = rgb::TARGET5;
	arrwarnatarget[5] = rgb::TARGET6;

    do {
        clearMatrix();
        drawFrame();
        drawLine(rgb::WHITE, 0, 0, 363, 0);
        drawLine(rgb::WHITE, 0, 0, 0, 419);
        drawLine(rgb::WHITE, 0, 419, 363, 419);
        drawLine(rgb::WHITE, 363, 0, 363, 419);
        ifstream myfile;
        vector<Building> B;

        readBangunanFromFile(myfile, B);

		//draw bangunan
        if (bangunan) {
            drawBangunan(B);
            colorBangunan();
        }


        // draw Pohon
        if (pohon)
            drawAllPohon();

        //draw jalan
        if (jalan)
            DrawJalan(0,0);

		for(int i = 0;i < jumlahTarget;i++) {
			if(arrtargetalive[i]){
				drawTarget(arrtargetx[i],arrtargety[i],scaling,arrwarnatarget[i], rgb::WHITE);

				arrtargetx[i] = arrtargetx[i] + 5*arrtargetdir[i];
				arrtargetujung[i] += arrtargetdir[i];
				if (arrtargetujung[i] >= 10){
					arrtargetdir[i] = -1;
				}
				if (arrtargetujung[i] <= 0) {
					arrtargetdir[i] = 1;
				}
			} else {
				//kecy = 0;

				if (!arrstop[i]) {
					if (arrkepalay[i] < arrkepalay[i]+8*scaling){
						arrkepalax[i] += arrkecx[i];
						arrkepalay[i] += arrkecy[i];
						arrkecy[i]++;
					}
					if (arrkecy[i] < 2) {
						arrstop[i] = true;
					}
					if (arrkecy[i] > 0) {
						arrkepalax[i] += arrkecx[i];
						arrkepalay[i] -= arrkecy[i];
						arrkecy[i] -= 2;
					}
					drawKepala(arrkepalax[i],arrkepalay[i],scaling,rgb::RED, rgb::WHITE);
					//sleep(5);
				}
			}
		}

		/*for (int i=0;i<lookSize;i++) {
            for (int j=0;j<lookSize;j++) {
                pixelMatrix[i][j] = petafix[i][j];
            }
        }*/
		/*drawTarget(posx,posy,scaling,rgb::RED, rgb::WHITE);

		//DrawToScreen();
		//floodFill(posx+1*scaling, posy+1*scaling, rgb::WHITE, rgb::BLACK);
		//floodFill(posx+1*scaling, posy+3*scaling, rgb::WHITE, rgb::BLACK);
		//sleep(1000);
		//drawTarget(posx,posy,scaling,rgb::BLACK, rgb::BLACK);

		//
		posx = posx + 5*direction;
		i += direction;
		if (i >= 10){
			direction = -1;
		}
		if (i <= 0) {
			direction = 1;
		}*/
		drawCircle(100,520,20);

        //draw drone
		int x_ = rotateX (x, y, 1);
		int y_ = rotateY (x, y, 1);
		drawDrone (x_, y_, length, width, rgb::MAGENTA);
		floodFill(x_, y_+10, rgb::MAGENTA, rgb::DARKGRAY);
		x = x_; y = y_;

        clipper(x_god,y_god,size_god);
        drawClips(100,700,lookSize-250);

        DrawToScreen();
    } while (KeyPressed!='C');

	//pthread_join(d_Drone, NULL);
    munmap(fbp, screensize);
    close(fbfd);

    return 0;
}
