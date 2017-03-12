#include <iostream>
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
#include <sstream>
#include <string>
#include <cstring>
#include "Point.hpp"
#include "Drawer.hpp"
#include "Building.hpp"
#include "Road.hpp"
#include "Tree.hpp"
#include "conio.h"

using namespace std;
using std::string;
using std::stringstream;


struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
char *fbp = 0;


bool finish = false;
int modeShow = 0;

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

void workerKeyStroke(){
    while(!finish){
        if(!detectKeyStroke()) {
            char KeyPressed = getch();
            switch(KeyPressed) {
            	case 'a':
            		Drawer::moveWindow(0, -3);
            		break;
            	case 's':
            		Drawer::moveWindow(3, 0);
            		break;
            	case 'd':
            		Drawer::moveWindow(0, 3);
            		break;
            	case 'w':
            		Drawer::moveWindow(-3, 0);
            		break;
            	case 'u':
            		modeShow = 0; 
            		break;
            	case 'i':
            		modeShow = 1; 
            		break;
            	case 'o':
            		modeShow = 2; 
            		break;
            	case 'p':
            		modeShow = 3; 
            		break;
            	case 'c':
                    finish = true;
                    break;
                case '-':
                    Drawer::growWindow();
                    break;
                case '=':
                    Drawer::shrinkWindow();
                    break;

            }
        }
    }
        
}

void DrawToScreen(){
    /* prosedure yang menggambar ke layar dari matriks RGB (harusnya rata tengah)*/
    long int location = 0;
    int x , y;
    for (y = 0; y < WIDTH; y++)
        for (x = 0; x < HEIGHT; x++) {
            location = (x+vinfo.xoffset) * (vinfo.bits_per_pixel/8) + (y+vinfo.yoffset) * finfo.line_length;
            if (vinfo.bits_per_pixel == 32) { 
                //4byte
                    *(fbp + location) = Drawer::getBlueValue(y, x);//bluePixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];        // Some blue
                    *(fbp + location + 1) = Drawer::getGreenValue(y, x);//greenPixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];     // A little green
                    *(fbp + location + 2) = Drawer::getRedValue(y, x);//redPixelMatrix[y - vinfo.yres/2 + WIDTH/2][x - vinfo.xres/2 + HEIGHT/2];    // A lot of red
                    *(fbp + location + 3) = 0;      // No transparency
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


int main() {
    Drawer::clearMatrix();    
    
    int fbfd = 0;
    long int screensize = 0;
    int sizeBuilding = 0;
    int sizeRoad = 0;
    int sizeTree = 0;
    Building building[100];
    Road road[100];
    Tree tree[100];

    // Inisialisasi data
    int number1;
    int number2;
    Point tempP[4];
    Point tempTree;

    string line;

  	ifstream myfile ("dataRoad");
	if (myfile) {  // same as: if (myfile.good())
	    while (getline( myfile, line )) {   // same as: while (getline( myfile, line ).good())
	    	stringstream stream(line);
	    	int i = 0;
	    	while(stream >> number1){
	    		stream >> number2;
	    		tempP[i].setX(number1);
	    		tempP[i].setY(number2);
	    		i++;
			}
			road[sizeRoad].setRoad(tempP);
			sizeRoad++;
	    }
	    myfile.close();
    }

    myfile.open("dataBuilding");
    if (myfile) {  // same as: if (myfile.good())
	    while (getline( myfile, line )) {   // same as: while (getline( myfile, line ).good())
	    	stringstream stream(line);
	    	int i = 0;
	    	while(stream >> number1){
	    		stream >> number2;
	    		tempP[i].setX(number1);
	    		tempP[i].setY(number2);
	    		i++;
			}
			building[sizeBuilding].setBuilding(tempP);
			sizeBuilding++;
	    }
	    myfile.close();
    }

    myfile.open("dataTree");
    if (myfile) {  // same as: if (myfile.good())
	    while (getline( myfile, line )) {   // same as: while (getline( myfile, line ).good())
	    	stringstream stream(line);
	    	while(stream >> number1){
	    		stream >> number2;
	    		tempTree.setX(number1);
	    		tempTree.setY(number2);
	    		
			}
			tree[sizeTree].setTree(tempTree);
			sizeTree++;
	    }
	    myfile.close();
    }

	




    // Akses Frame Buffer
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

    // Map the device to memory
    fbp = (char *)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, (off_t)0);

    // Key listener
    thread thread1(&workerKeyStroke);

    // xWindow = 0; yWindow = 0;
    // xView = 0; yView = heightmatrix;
    
    do {
        Drawer::clearMatrix();
        Drawer::drawFrame(); Drawer::drawView(); Drawer::drawWindow();
        if(modeShow == 0 || modeShow == 1) {
        	for (int i = 0; i < sizeBuilding; ++i)
        	{
        		building[i].drawBuilding();
        	}
        }
        
        if(modeShow == 0 || modeShow == 2) {
        	for (int i = 0; i < sizeRoad; ++i)
        	{
        		road[i].drawRoad();
        	}
        }

        if(modeShow == 0 || modeShow == 3) {
        	for (int i = 0; i < sizeTree; ++i)
        	{
        		tree[i].drawTree();
        	}
        }

        DrawToScreen(); 
        usleep(50000);
    } while (!finish);
    thread1.detach();
	
    munmap(fbp, screensize);
    close(fbfd);
    
    return 0;
}
