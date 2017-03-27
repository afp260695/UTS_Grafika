#ifndef DRAWER_HPP
#define DRAWER_HPP

#include "Color.hpp"
#include "Object.hpp"
#include <cmath>
#include <algorithm>
#define WIDTH 1300
#define HEIGHT 700
#define WIDTH_REAL 1200
#define HEIGHT_REAL 600

Color* WHITE = new Color(255,255,255);
Color* GRAY = new Color(128, 128, 128);
Color* YELLOW = new Color(255, 255, 0);
Color* BLACK = new Color(0, 0, 0);
Color* GREEN = new Color(0, 255, 0);
Color* RED = new Color(255, 0, 0);
Color* RED2 = new Color(254, 0, 0);
Color* MAGENTA = new Color(255, 0, 255);
Color* MAGENTA2 = new Color(255, 0, 254);
Color* DARKGRAY = new Color(112, 128, 144);

class Drawer
{
public:
	Drawer() {}
	static void clearMatrix() {
	    for (int i = 0; i < WIDTH; ++i)
	        for (int j = 0; j < HEIGHT; ++j)
	            pixelMatrix[i][j].setColor(0, 0, 0);
	}

	void drawObject(Object o){
		for(int i=0;i<o.getNTitik()-1;++i){
			drawLine(o.getWarnaBorder(),o.getListOfTitik()[i].getX(),o.getListOfTitik()[i].getY(),o.getListOfTitik()[i+1].getX(),o.getListOfTitik()[i+1].getY());
		}
		drawLine(o.getWarnaBorder(),o.getListOfTitik()[o.getNTitik()-1].getX(),o.getListOfTitik()[o.getNTitik()-1].getY(),o.getListOfTitik()[0].getX(),o.getListOfTitik()[0].getY());
		if(o.getShowIsi()){
			//belum
			floodFill(0,0,o.getWarnaBorder(),o.getWarnaIsi());
		}
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

	static Color getPixelMatrix(int x, int y) {
		return pixelMatrix[x][y];
	}

	static Color pixelMatrix[WIDTH][HEIGHT];
	
};

Color Drawer::pixelMatrix[WIDTH][HEIGHT];

#endif
