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

#define WIDTH 1300
#define HEIGHT 700
#define lookSize 600

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
    drawLine(roadColor, 46,404,43,414); drawLine(roadColor, 44,404,41,414);
    drawLine(roadColor, 46,404,9,280); drawLine(roadColor, 43,404,7,280);
    drawLine(roadColor, 45,407,151,377); drawLine(roadColor, 45,403,151,373);
    drawLine(roadColor, 153,376,149,414); drawLine(roadColor, 150,376,147,414);
    drawLine(roadColor, 151,378,181,374); drawLine(roadColor, 151,374,181,370);
    drawLine(roadColor, 181,373,287,403); drawLine(roadColor, 181,371,287,401);
    drawLine(roadColor, 288,402,288,414); drawLine(roadColor, 285,402,285,414);
    drawLine(roadColor, 287,404,356,404); drawLine(roadColor, 287,401,356,401);
    //itb kanan
    drawLine(roadColor, 308,231,310,297); drawLine(roadColor, 305,231,307,297);
    drawLine(roadColor, 308,297,292,300); drawLine(roadColor, 308,297,292,300);
    drawLine(roadColor, 292,302,291,337); drawLine(roadColor, 292,299,291,334);
    drawLine(roadColor, 291,337,287,404); drawLine(roadColor, 291,334,287,401);
    drawLine(roadColor, 268,231,268,12); drawLine(roadColor, 265,231,265,12);
    drawLine(roadColor, 268,50,343,56); drawLine(roadColor, 265,50,340,56);
    drawLine(roadColor, 266,113,326,118); drawLine(roadColor, 266,110,326,115);
    drawLine(roadColor, 266,134,326,138); drawLine(roadColor, 266,131,326,135);
    //itb tengah
    drawLine(roadColor, 184,372,184,335); drawLine(roadColor, 178,372,178,335);
    drawLine(roadColor, 181,337,71,337); drawLine(roadColor, 181,333,71,333);
    drawLine(roadColor, 184,335,184,231); drawLine(roadColor, 178,335,178,231);
    drawLine(roadColor, 181,233,58,233); drawLine(roadColor, 181,230,58,230);
    drawLine(roadColor, 58,233,23,233); drawLine(roadColor, 58,230,23,230);
    drawLine(roadColor, 181,233,306,233); drawLine(roadColor, 181,230,306,230);
    drawLine(roadColor, 181,337,291,337); drawLine(roadColor, 181,334,291,334);
    //drawLine(rgb::WHITE, 181,335,291,335);

    drawLine(roadColor, 184,231,184,160); drawLine(roadColor, 178,231,178,160);
    drawLine(roadColor, 184,160,184,83); drawLine(roadColor, 178,160,178,83);
    drawLine(roadColor, 184,29,184,16); drawLine(roadColor, 178,29,178,16);
    drawLine(roadColor, 80,18,181,18); drawLine(roadColor, 80,15,181,15);
    drawLine(roadColor, 181,18,219,1); drawLine(roadColor, 181,15,219,0);
    //itb kiri
    drawLine(roadColor, 22,231,22,294); drawLine(roadColor, 24,231,24,294);
    drawLine(roadColor, 23,293,44,329); drawLine(roadColor, 23,295,44,331);
    drawLine(roadColor, 44,329,71,334); drawLine(roadColor, 44,331,71,336);
    drawLine(roadColor, 57,231,57,315); drawLine(roadColor, 59,231,59,315);
    drawLine(roadColor, 57,315,70,335); drawLine(roadColor, 59,315,72,335);
    drawLine(roadColor, 21,231,23,173); drawLine(roadColor, 25,231,27,173);
    drawLine(roadColor, 25,171,82,171); drawLine(roadColor, 25,175,82,175);
    drawLine(roadColor, 82,172,95,172); drawLine(roadColor, 82,174,95,174);
    drawLine(roadColor, 94,173,94,199); drawLine(roadColor, 96,173,96,199);
    drawLine(roadColor, 95,198,102,201); drawLine(roadColor, 95,200,102,203);
    drawLine(roadColor, 101,202,102,231); drawLine(roadColor, 103,202,104,231);
    drawLine(roadColor, 107,83,108,28); drawLine(roadColor, 104,83,105,28);
    drawLine(roadColor, 107,30,266,35); drawLine(roadColor, 107,27,266,32);
    drawLine(roadColor, 83,173,83,83); drawLine(roadColor, 81,173,81,83);
    drawLine(roadColor, 82,85,266,85); drawLine(roadColor, 82,82,266,82);
    drawLine(roadColor, 82,120,266,120); drawLine(roadColor, 82,117,266,117);
    drawLine(roadColor, 82,162,266,162); drawLine(roadColor, 82,159,266,159);
    drawLine(roadColor, 27,173,29,46); drawLine(roadColor, 24,173,26,46);
    drawLine(roadColor, 29,47,52,29); drawLine(roadColor, 27,45,50,27);
    drawLine(roadColor, 52,29,81,29); drawLine(roadColor, 50,27,79,27);
    drawLine(roadColor, 82,28,82,16); drawLine(roadColor, 79,28,79,16);
    drawLine(roadColor, 80,17,48,17); drawLine(roadColor, 80,15,48,15);
    drawLine(roadColor, 49,17,22,51); drawLine(roadColor, 47,15,20,49);
    drawLine(roadColor, 22,50,9,280); drawLine(roadColor, 20,50,7,280);

}

void drawFrame() {
    drawLine(rgb::WHITE, 0, 0, 1200, 0);
    drawLine(rgb::WHITE, 1200, 0, 1200, 600);
    drawLine(rgb::WHITE, 1200, 600, 0, 600);
    drawLine(rgb::WHITE, 0, 600, 0, 0);
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
    drawPohon(21,14,10);
    drawPohon(40,40,7);
    drawPohon(40,54,7);
    drawPohon(32,65,6);
    drawPohon(32,54,7);
    drawPohon(79,73,7);
    drawPohon(88,62,11);
    drawPohon(34,88,8);
    drawPohon(34,102,8);
    drawPohon(63,141,10);
    drawPohon(128,89,10);
    drawPohon(128,102,10);
    drawPohon(128,125,10);
    drawPohon(128,143,10);
    drawPohon(38,179,13);
    drawPohon(32,210,17);
    drawPohon(194,16,10);
    drawPohon(216,16,10);
    drawPohon(233,16,10);
    drawPohon(250,16,10);
    drawPohon(317,16,29);
    drawPohon(337,132,16);
    drawPohon(251,123,10);
    drawPohon(251,143,10);
    drawPohon(251,88,10);
    drawPohon(251,102,10);
    drawPohon(333,176,19);
    drawPohon(328,217,24);
    drawPohon(316,262,32);
    drawPohon(309,309,36);
    drawPohon(276,87,12);
}

void readBangunanFromFile (ifstream& myfile, vector<Building> &B) {
    myfile.open ("bangunan.txt");

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

void drawTarget(int x, int y, int scaling, rgb warna){
    //kelapa
    drawLine(rgb::WHITE, x, y, x+2*scaling, y);
    drawLine(rgb::WHITE, x+2*scaling, y, x+2*scaling, y+2*scaling);
    drawLine(rgb::WHITE, x, y, x, y+2*scaling);
    //badan
    drawLine(rgb::WHITE, x-2*scaling, y+2*scaling, x+4*scaling, y+2*scaling);
    drawLine(rgb::WHITE, x-2*scaling, y+2*scaling, x-2*scaling, y+6*scaling);
    drawLine(rgb::WHITE, x-2*scaling, y+2*scaling, x-2*scaling, y+6*scaling);
    drawLine(rgb::WHITE, x-2*scaling, y+6*scaling, x, y+6*scaling);
    drawLine(rgb::WHITE, x, y+6*scaling, x, y+4*scaling);
    drawLine(rgb::WHITE, x, y+6*scaling, x, y+8*scaling);
    drawLine(rgb::WHITE, x, y+8*scaling, x+2*scaling, y+8*scaling);
    drawLine(rgb::WHITE, x+2*scaling, y+8*scaling, x+2*scaling, y+4*scaling);
    drawLine(rgb::WHITE, x+4*scaling, y+2*scaling, x+4*scaling, y+6*scaling);
    drawLine(rgb::WHITE, x+4*scaling, y+6*scaling, x+2*scaling, y+6*scaling);
    //floodfill
    floodFill(x+1*scaling, y+1*scaling, rgb::WHITE, warna);
    floodFill(x+1*scaling, y+3*scaling, rgb::WHITE, warna);
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
    
    do {
        clearMatrix();
        drawFrame();
        drawLine(rgb::WHITE, 0, 0, 363, 0);
        drawLine(rgb::WHITE, 0, 0, 0, 419);
        drawLine(rgb::WHITE, 0, 419, 363, 419);
        drawLine(rgb::WHITE, 363, 0, 363, 419);

/*
        drawShooter(xp,yp,lastCorrectState);
        drawShooter(xp,yp-150,lastCorrectState);
        drawShooter(xp-200,yp,lastCorrectState);
        drawShooter(xp-200,yp-150,lastCorrectState);
*/
        ifstream myfile;
        vector<Building> B;

        readBangunanFromFile(myfile, B);

        //draw bangunan
        /*if (bangunan) {
            drawBangunan(B);
            colorBangunan();
        }


        // draw Pohon
        if (pohon)
            drawAllPohon();

        //draw jalan
        if (jalan)
            DrawJalan(0,0);
        */

        for (int i=0;i<lookSize;i++) {
            for (int j=0;j<lookSize;j++) {
                pixelMatrix[i][j] = petafix[i][j];
            }
        }

        drawTarget(100,100,2,rgb::RED);

        clipper(x_god,y_god,size_god);
        drawClips(100,700,lookSize-250);

        DrawToScreen();
    } while (KeyPressed!='C');

    munmap(fbp, screensize);
    close(fbfd);

    return 0;
}
