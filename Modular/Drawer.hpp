#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "Color.hpp"
#include "Point.hpp"
#define WIDTH 1300
#define HEIGHT 700
#define WIDTH_REAL 1200
#define HEIGHT_REAL 600
#define lookSize 350
class Drawer
{
public:
	
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

	bool drawLine(Color color, int x1, int y1, int x2, int y2) {
	
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

	void drawPoint(Color color, int x, int y) {
	    if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT) return;
	    pixelMatrix[x][y] = color;
	}

	void floodFill(int x, int y, Color batas, Color warna){
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


private:
	static Color pixelMatrix[WIDTH][HEIGHT];
	
};

Color Drawer::pixelMatrix[WIDTH][HEIGHT];

#endif