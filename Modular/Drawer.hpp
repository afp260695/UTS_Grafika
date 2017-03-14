#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "Color.hpp"
#include <cmath>
#include <algorithm>
#define WIDTH 1300
#define HEIGHT 700
#define WIDTH_REAL 1200
#define HEIGHT_REAL 600
#define lookSize 350

class Drawer
{
public:
	static int mode;
	static void clearMatrix() {
	    for (int i = 0; i < WIDTH; ++i)
	        for (int j = 0; j < HEIGHT; ++j)
	            pixelMatrix[i][j].setColor(0, 0, 0);
	}

	static void clearClipperMatrix() {
	    for (int i = 0; i < lookSize; ++i)
	        for (int j = 0; j < lookSize; ++j)
	            clipperMatrix[i][j].setColor(0, 0, 0);
	}

	static void drawFrame() {
	    drawLine(Color::WHITE, 0, 0, WIDTH_REAL, 0);
	    drawLine(Color::WHITE, WIDTH_REAL, 0, WIDTH_REAL, HEIGHT_REAL);
	    drawLine(Color::WHITE, WIDTH_REAL, HEIGHT_REAL, 0, HEIGHT_REAL);
	    drawLine(Color::WHITE, 0, HEIGHT_REAL, 0, 0);
	}

	static bool drawLine(Color color, int x1, int y1, int x2, int y2) {
	
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

	static void drawPoint(Color color, int x, int y) {
	    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
	    pixelMatrix[x][y] = color;
	}

	static void floodFill(int x, int y, Color batas, Color warna){
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

	static void drawClipPointer() {
	    int center = (lookSize-250)/2;
	    if (mode == 0) {
	        for(int i = 1; i < 10; i++) {
	            clipperMatrix[center + i][center] = Color::WHITE;
	            clipperMatrix[center - i][center] = Color::WHITE;
	            clipperMatrix[center][center + i] = Color::WHITE;
	            clipperMatrix[center][center - i] = Color::WHITE;
	        }
	    } else {
	        for(int i = 0; i < 3; i++) {
	            for (int j = 0; j < 3-i; j++) {
	                if (!(i == 0 && j == 0)) {
	                    clipperMatrix[center + i][center+j] = Color::WHITE;
	                    clipperMatrix[center + i][center-j] = Color::WHITE;
	                    clipperMatrix[center - i][center+j] = Color::WHITE;
	                    clipperMatrix[center - i][center-j] = Color::WHITE;
	                }
	            }
	        }
	    }
	}

	static void clipper(int y, int x, int size) {
	    int i, j;

	    drawLine(Color::YELLOW, x-1,y-1,x+size,y-1);
	    drawLine(Color::YELLOW, x+size,y-1,x+size,y+size);
	    drawLine(Color::YELLOW, x+size,y+size,x-1,y+size);
	    drawLine(Color::YELLOW, x-1,y+size,x-1,y-1);

	    clearClipperMatrix();

	    for (i=0;i<lookSize-250;i++) {
	        for (j=0;j<lookSize-250;j++) {
	            clipperMatrix[i][j] = pixelMatrix[x+(i*size/(lookSize-250))][y+(j*size/(lookSize-250))];
	        }
	    }

	    drawClipPointer();
	}


	static void drawClips(int y, int x, int size) {
	    int i,j;
	    
	    drawLine(Color::YELLOW, x-1,y-1,x+size,y-1);
	    drawLine(Color::YELLOW, x+size,y-1,x+size,y+size);
	    drawLine(Color::YELLOW, x+size,y+size,x-1,y+size);
	    drawLine(Color::YELLOW, x-1,y+size,x-1,y-1);

	    for (i=0;i<size;i++) {
	        for (j=0;j<size;j++) {
	            pixelMatrix[x+i][y+j] = clipperMatrix[i][j];
	            
	        }
	    }

	}

	static void drawCircle(int x0, int y0, int radius) {
	    int x = radius;
	    int y = 0;
	    int err = 0;

	    while (x >= y)
	    {
	        drawPoint(Color::WHITE, x0 - x, y0 + y);
	        drawPoint(Color::WHITE, x0 - y, y0 + x);
	        drawPoint(Color::WHITE, x0 - y, y0 - x);
	        drawPoint(Color::WHITE, x0 - x, y0 - y);
	        drawPoint(Color::WHITE, x0 + x, y0 + y);
	        drawPoint(Color::WHITE, x0 + y, y0 + x);
	        drawPoint(Color::WHITE, x0 + y, y0 - x);
	        drawPoint(Color::WHITE, x0 + x, y0 - y);

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
	    floodFill(x0,y0,Color::WHITE,Color::GREEN);
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

	Color getPixelMatrix(int x, int y) {
		return pixelMatrix[x][y];
	}

	Color getClipperMatrix(int x, int y) {
		return clipperMatrix[x][y];
	}

private:
	static Color pixelMatrix[WIDTH][HEIGHT];
	static Color clipperMatrix[lookSize][lookSize];
	
};

Color Drawer::pixelMatrix[WIDTH][HEIGHT];
Color Drawer::clipperMatrix[lookSize][lookSize];
int Drawer::mode = 0;

#endif