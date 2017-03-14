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
#define N_TARGET 6

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;


const int MAP_WIDTH = 442;
const int MAP_HEIGHT = 520;

static rgb hsv2rgb(hsv in);

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;


int posX;
int posY;
bool exploded = false;

int mode = 0;
//Array



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


void fire() {
    for (int i = 0; i < jumlahTarget; i++) {
        if (targetColor[i] == clipperMatrix[(lookSize-250)/2][(lookSize-250)/2]) {
            isTargetShot[i] = true;
        }
    }
}

void listenerKeyStroke(){

    while (!detectKeyStroke()) {
        char KeyPressed = getchar();
        if ((KeyPressed=='D') ||(KeyPressed=='d')) {
            y_god++;
        } else if ((KeyPressed=='A') ||(KeyPressed=='a')) {
            if(y_god > 0)
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
        } else if (KeyPressed==' ') {
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
    
    drawLine(rgb::YELLOW, x-1,y-1,x+size,y-1);
    drawLine(rgb::YELLOW, x+size,y-1,x+size,y+size);
    drawLine(rgb::YELLOW, x+size,y+size,x-1,y+size);
    drawLine(rgb::YELLOW, x-1,y+size,x-1,y-1);

    for (i=0;i<size;i++) {
        for (j=0;j<size;j++) {
            pixelMatrix[x+i][y+j] = clipperMatrix[i][j];
            
        }
    }

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

bool drawLineOrang(rgb color, int x1, int y1, int x2, int y2) {
    bool ret = false;

    int deltaX = x2 - x1;
    int deltaY = y2 - y1;
    int ix = deltaX > 0 ? 1 : -1;
    int iy = deltaY > 0 ? 1 : -1;
    deltaX = abs(deltaX);
    deltaY = abs(deltaY);

    int x = x1;
    int y = y1;

    if(pixelMatrix[x][y] == rgb::BLACK)
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
            if(pixelMatrix[x][y] == rgb::BLACK)
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
            if(pixelMatrix[x][y] == rgb::BLACK)
                drawPoint(color, x, y);
        }
    }
    return ret;
}

void floodFillOrang(int x, int y, rgb batas, rgb warna){
    if((x>=0 && x<WIDTH) && (y>=0 && y<HEIGHT)){
        if(pixelMatrix[x][y] == rgb::BLACK){
            pixelMatrix[x][y] = warna;
            floodFillOrang(x,y+1, batas, warna);
            floodFillOrang(x+1,y, batas, warna);
            floodFillOrang(x,y-1, batas, warna);
            floodFillOrang(x-1,y, batas, warna);
        }
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

void drawKepala(int x, int y, int scaling, rgb warna, rgb batas){
    drawLineOrang(batas, x, y, x+2*scaling, y);
    drawLineOrang(batas, x+2*scaling, y, x+2*scaling, y+2*scaling);
    drawLineOrang(batas, x, y, x, y+2*scaling);
    drawLineOrang(batas, x, y+2*scaling, x+2*scaling, y+2*scaling);
    floodFillOrang(x+1, y+1, batas, warna);
}

void drawBadan(int x, int y, int scaling, rgb warna, rgb batas){
    drawLineOrang(batas, x-2*scaling, y+2*scaling, x+4*scaling, y+2*scaling);
    drawLineOrang(batas, x-2*scaling, y+2*scaling, x-2*scaling, y+6*scaling); 
    drawLineOrang(batas, x-2*scaling, y+2*scaling, x-2*scaling, y+6*scaling);
    // drawCircle(x+4*scaling, y+6*scaling, 2);
    drawLineOrang(batas, x-2*scaling, y+6*scaling, x, y+6*scaling);
    drawLineOrang(batas, x, y+6*scaling, x, y+4*scaling);
    drawLineOrang(batas, x, y+6*scaling, x, y+8*scaling);
    drawLineOrang(batas, x, y+8*scaling, x+2*scaling, y+8*scaling);
    drawLineOrang(batas, x+2*scaling, y+8*scaling, x+2*scaling, y+4*scaling);

    drawLineOrang(batas, x+4*scaling, y+2*scaling, x+4*scaling, y+6*scaling);
    drawLineOrang(batas, x+4*scaling, y+6*scaling, x+2*scaling, y+6*scaling);
    floodFillOrang((x-2*scaling) + 1, (y+2*scaling) + 1, batas, warna);
    floodFillOrang((x-2*scaling) + 1, (y+6*scaling) - 1, batas,warna);
    floodFillOrang((x) + 1, (y+8*scaling) - 1, batas, warna);
    floodFillOrang((x+2*scaling) - 1, (y+8*scaling) - 1, batas, warna);
    floodFillOrang((x+4*scaling) - 1, (y+2*scaling) + 1, batas, warna);
    floodFillOrang((x+4*scaling) - 1, (y+6*scaling) - 1, batas, warna);
    floodFillOrang(x+1, y+3*scaling, batas, warna);
    floodFillOrang(x+1, y+5*scaling, batas, warna);
}

void drawTarget(int x, int y, int scaling, rgb warna, rgb batas){
    drawKepala(x,y,scaling,warna,batas);

    //badan
    drawBadan(x,y,scaling,warna,batas);
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



void drawBangunan(vector<Building> &B) {
    double hue = 0;
    // for (int i = 0; i < B.size()-10 ; i++) {
    // cout << B.size()<<endl;
    for (int i = 0; i < B.size() ; i++) {
        // int i = 0;
        int j;
        int x_min = 99999999;
        int x_max = -1;
        int y_min = 99999999;
        int y_max = -1;
        int x_average = 0;
        int y_average = 0;
        for (j= 0; j < (B[i].points.size()-2); j += 2) {
            drawLine(rgb::WHITE, B[i].points[j], B[i].points[j+1], B[i].points[j+2], B[i].points[j+3]);
            x_min = min(x_min,min(B[i].points[j],B[i].points[j+2]));
            y_min = min(y_min,min(B[i].points[j+1],B[i].points[j+3]));
            x_max = max(x_max,max(B[i].points[j], B[i].points[j+2]));
            y_max = max(y_max, max(B[i].points[j+1], B[i].points[j+3]));
            
        }
        // drawCircle(x_max, y_max, 5);
        // drawCircle(B[i].points[j+1], B[i].points[j+3], 5);
        x_average = (x_max+x_min) / 2;
        y_average = (y_max+y_min) / 2;
        // cout <<  B[i].points.size() <<endl;
        // cout << "x_average" << x_average << endl;
        // cout << "y_average" << y_average << endl;
        // drawCircle(x_average, y_average, 5);
        hsv buildingColor = {hue, 0.7, 0.8};
        // x_average = x_average / B[i].points.size();
        // y_average = y_average / B[i].points.size();
        floodFill(x_average, y_average, rgb::WHITE, hsv2rgb(buildingColor));
        hue += 10;
        if (hue > 360) {
            hue = 0;
        }
        
    }

    
    
}

void behaviourOrang(int i, int scaling, int speed, int range){
    if(!isTargetShot[i]){
        drawTarget(arrtargetx[i],arrtargety[i],scaling,targetColor[i], rgb::MAGENTA2);

        arrtargetx[i] = arrtargetx[i] + speed*arrtargetdir[i];
        arrtargetujung[i] += arrtargetdir[i];
        if (arrtargetujung[i] >= range){
            arrtargetdir[i] = -1;
        }
        if (arrtargetujung[i] <= 0) {
            arrtargetdir[i] = 1;
        }
        arrkepalay[i] = arrtargety[i];
        arrkepalax[i] = arrtargetx[i];
    } else {
        //kecy = 0;
        
        //if (!arrstop[i]) {
            if (arrkepalay[i] < arrkepalay[i]+(8*scaling)){
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
            drawKepala(arrkepalax[i],arrkepalay[i],scaling,targetColor[i], rgb::MAGENTA2);
            
        //}
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
    thread thread1(listenerKeyStroke);

    int xp = 600;
    int yp = 574;
    char KeyPressed;

    int xawal = 100, yawal = 150;
    bool left = true;
    
    
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
        //posisi target
        if (i == 0){
            arrtargetx[i] = 25;
            arrtargety[i] = 40;
        }
        else if (i == 1){
            arrtargetx[i] = 100;
            arrtargety[i] = 115;
        }
        else if (i == 2){
            arrtargetx[i] = 150;
            arrtargety[i] = 175;
        }
        else if (i == 3){
            arrtargetx[i] = 235;
            arrtargety[i] = 235;
        }
        else if (i == 4){
            arrtargetx[i] = 175;
            arrtargety[i] = 300;
        }
        else{
            arrtargetx[i] = 310;
            arrtargety[i] = 375;
        }
        rgb temp;
        temp.r = 254-i;
        temp.g = 0;
        temp.b = 0;
        targetColor[i] = temp;
		
		arrtargetdir[i] = 0;
		arrstop[i] = false;
		arrtargetalive[i] = true;
		arrtargetujung[i] = 0;
	}
    ifstream myfile;
    vector<Building> B;

    readBangunanFromFile(myfile, B);
    do {
        clearMatrix();
        drawFrame();
        drawLine(rgb::WHITE, 0, 0, 442, 0);
        drawLine(rgb::WHITE, 0, 0, 0, 520);
        drawLine(rgb::WHITE, 0, 520, 442, 520);
        drawLine(rgb::WHITE, 442, 0, 442, 520);
        
		if (bangunan) {
            drawBangunan(B);
        }
		
		// draw Pohon
        if (pohon)
            drawAllPohon();
		
        // perilaku target
		for(int i = 0;i < jumlahTarget;i++) {
            if (i == 0){
                behaviourOrang(i, scaling, 5, 75);
            }
            else if (i == 1){
                behaviourOrang(i, scaling, 2, 55);
            }
            else if (i == 2){
                behaviourOrang(i, scaling, 1, 100);
            }
            else if (i == 3){
                behaviourOrang(i, scaling, 4, 40);
            }
            else if (i == 4){
                behaviourOrang(i, scaling, 3, 30);
            }
            else{
                behaviourOrang(i, scaling, 2, 50);
            }
			
		}
		//draw bangunan
		
        
        //draw jalan
        if (jalan)
            DrawJalan(0,0);
            
		
		drawCircle(100,445,20);
        
        clipper(x_god,y_god,size_god);
        drawClips(100,700,lookSize-250);
		//draw drone		
		int x_ = rotateX (x, y, 1);
		int y_ = rotateY (x, y, 1);
		drawDrone (x_, y_, length, width, rgb::MAGENTA);
		floodFill(x_, y_+10, rgb::MAGENTA, rgb::DARKGRAY);
		x = x_; y = y_;
        DrawToScreen();
    } while (KeyPressed!='C');
	
	//pthread_join(d_Drone, NULL);
    munmap(fbp, screensize);
    close(fbfd);

    return 0;
}
