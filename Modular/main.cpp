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
#include "Color.hpp"
#include "Peta.hpp"
#include "Target.hpp"
#include "Drawer.hpp"
#include "Drone.hpp"

using namespace std;

#define WIDTH 1300
#define HEIGHT 700
#define lookSize 350
#define nTarget 6

const int MAP_WIDTH = 442;
const int MAP_HEIGHT = 520;

static Color hsv2Color(hsv in);

struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;


Target targets[nTarget];

int x_god;
int y_god;

int size_god;

unsigned char* BMP;
int BMP_width, BMP_height;

bool pohon = true;
bool jalan = true;
bool bangunan = true;
int screen_size;

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
                    *(fbp + location) = Drawer::getPixelMatrix(x,y).b;        // Some blue
                    *(fbp + location + 1) = Drawer::getPixelMatrix(x,y).g;     // A little green
                    *(fbp + location + 2) = Drawer::getPixelMatrix(x,y).r;    // A lot of red
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
    for (int i = 0; i < nTarget; i++) {
        if (targets[i].targetColor == Drawer::getClipperMatrix((lookSize)/2,(lookSize)/2)) {
            targets[i].targetAlive = false;
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
            Drawer::inverseMode();
        } else if (KeyPressed==' ') {
            fire();
        }
    }
}


int main(int argc, char const *argv[]) {
    Drawer::clearMatrix();
	
    int fbfd = 0;
    long int screensize = 0;
    
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

    char KeyPressed;
    
	int length = 50;
	int width = 20;
	int x = 200;
	int y = 200;
	int direction = 0;
	int scaling = 2;
	
	for(int i = 0;i < nTarget;i++) {
		targets[i].kecY = 0;
		targets[i].kecX = 1;
        //posisi target
        if (i == 0){
            targets[i].targetX = 25;
            targets[i].targetY = 40;
        }
        else if (i == 1){
            targets[i].targetX = 100;
            targets[i].targetY = 115;
        }
        else if (i == 2){
            targets[i].targetX = 150;
            targets[i].targetY = 175;
        }
        else if (i == 3){
            targets[i].targetX = 235;
            targets[i].targetY = 235;
        }
        else if (i == 4){
            targets[i].targetX = 175;
            targets[i].targetY = 300;
        }
        else{
            targets[i].targetX = 310;
            targets[i].targetY = 375;
        }
        Color* temp = new Color();
        temp->r = 254-i;
        temp->g = 0;
        temp->b = 0;
        targets[i].targetColor = *temp;
		
		targets[i].targetDir = 0;
		targets[i].stop = false;
		targets[i].targetAlive = true;
		targets[i].targetUjung = 0;
	}
    ifstream myfile;
    vector<Building> B;

    Peta::readBangunanFromFile(myfile, B);
    do {
        Drawer::clearMatrix();
        Drawer::drawFrame();
        Drawer::drawLine(*WHITE, 0, 0, 442, 0);
        Drawer::drawLine(*WHITE, 0, 0, 0, 520);
        Drawer::drawLine(*WHITE, 0, 520, 442, 520);
        Drawer::drawLine(*WHITE, 442, 0, 442, 520);
        
		if (bangunan) {
            Peta::drawBangunan(B);
        }
		
		// draw Pohon
        if (pohon)
            Peta::drawAllPohon();
		
        // perilaku target
		for(int i = 0;i < nTarget;i++) {
            if (i == 0){
                targets[i].behaviourOrang(scaling, 5, 75);
            }
            else if (i == 1){
                targets[i].behaviourOrang(scaling, 2, 55);
            }
            else if (i == 2){
                targets[i].behaviourOrang(scaling, 1, 100);
            }
            else if (i == 3){
                targets[i].behaviourOrang(scaling, 4, 40);
            }
            else if (i == 4){
                targets[i].behaviourOrang(scaling, 3, 30);
            }
            else{
                targets[i].behaviourOrang(scaling, 2, 50);
            }
			
		}
		//draw bangunan
		
        
        //draw jalan
        if (jalan)
            Peta::DrawJalan(0,0);
            
		
		Drawer::drawCircle(100,445,20);
        
        
        Drawer::drawClips(100,700,lookSize);
		//draw drone		
		int x_ = Drawer::rotateX (x, y, 1);
		int y_ = Drawer::rotateY (x, y, 1);
		Drone::drawDrone (x_, y_, length, width, *MAGENTA);
		Drawer::floodFill(x_, y_+10, *MAGENTA, *DARKGRAY);
		x = x_; y = y_;
        Drawer::clipper(x_god,y_god,size_god);
        DrawToScreen();
    } while (KeyPressed!='C');
	
	//pthread_join(d_Drone, NULL);
    munmap(fbp, screensize);
    close(fbfd);

    return 0;
}
