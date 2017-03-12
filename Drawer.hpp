#ifndef DRAWER_HPP
#define DRAWER_HPP
#define HEIGHT 900 //Scaled
#define WIDTH 750 //Scaled
#define widthMatrix 600 //Scaled
#define heightmatrix 600 //Scaled
#define WINDOWSIZE 200

#include "Point.hpp"
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
#include <cstring>

class Drawer {
	public :
		static int getXWindow() { return xWindow; }
		static int getYWindow() { return yWindow; }
		static double getRatio() { return (double) viewSize / (double) windowSize; }

		static void clearMatrix() {
    		memset(redPixelMatrix, 0, sizeof(redPixelMatrix));
    		memset(greenPixelMatrix, 0, sizeof(redPixelMatrix));
    		memset(bluePixelMatrix, 0, sizeof(redPixelMatrix));
		}


		// Predikat
		static int isInside(int x, int y, int x0, int y0, int s) {
			if (x0 <= x && x <= x0+s && y0 <= y && y <= y0 + s) return 1;
				else return 0;
		}

		static int isIntersectX(int x, Point p1, Point p2) {
			double gradient = (p2.getY() - p1.getY())/(p2.getX() - p2.getY());
			int c = p2.getY() - (int) (gradient * p2.getX());
			int newY = (int) (gradient * x) + c;
			return newY;
		}

		static int isIntersectY(int y, Point p1, Point p2) {
			double gradient = (p2.getY() - p1.getY())/(p2.getX() - p2.getY());
			int c = p2.getY() - (int) (gradient * p2.getX());
			int newX = (int) ((y-c)/gradient);
			return newX;
		}

		static bool isInsideRectangle(int xRect1, int xRect2, int yRect1, int yRect2, Point p1, Point p2) {
			int yIntersect1 = isIntersectX(xRect1, p1, p2);
			int yIntersect2 = isIntersectX(xRect2, p1, p2);
			int xIntersect1 = isIntersectY(yRect1, p1, p2);
			int xIntersect2 = isIntersectY(yRect2, p1, p2);
			return (yIntersect1 == yRect1 || yIntersect1 == yRect2 || yIntersect2 == yRect1 || yIntersect2 == yRect2)
			&& (xIntersect1 == xRect1 || xIntersect1 == xRect2 || xIntersect2 == xRect1 || xIntersect2 == xRect2);
		}

		// Getter
		static int getGreenValue(int _y, int _x) {
			return greenPixelMatrix[_y][_x];
		}

		static int getRedValue(int _y, int _x) {
			return redPixelMatrix[_y][_x];
		}

		static int getBlueValue(int _y, int _x) {
			return bluePixelMatrix[_y][_x];
		}

		static int isInsideWindow(Point& p) {
			if(isInside(p.getX(), p.getY(), xWindow, yWindow, windowSize)) return 1;
			return 0;
		}

		// Draw Point
		static void drawWhitePoint(int x1, int y1) {
		    redPixelMatrix[x1][y1] = 255;
		    greenPixelMatrix[x1][y1] = 255;
		    bluePixelMatrix[x1][y1] = 255;
		}

		static void drawRedPoint(int x1,int y1){
		    redPixelMatrix[x1][y1] = 255;
		    greenPixelMatrix[x1][y1] = 0;
		    bluePixelMatrix[x1][y1] = 0;   
		}

		static void drawBlackPoint(int x1,int y1){
		    redPixelMatrix[x1][y1] = 0;
		    greenPixelMatrix[x1][y1] = 0;
		    bluePixelMatrix[x1][y1] = 0;   
		}

		// Draw Segment in View
		static bool drawWhiteLineInView(Point p1_inWindow, Point p2_inWindow) {
			int x1 = p1_inWindow.getX(), y1 = p1_inWindow.getY();
			int x2 = p2_inWindow.getX(), y2 = p2_inWindow.getY();

			int xSegmentViewStart = xView+x1, ySegmentViewStart = yView+y1;
			int xSegmentViewEnd = xView + x2, ySegmentViewEnd = yView + y2;

			drawWhiteLine(xSegmentViewStart, ySegmentViewStart, xSegmentViewEnd, ySegmentViewEnd);
		}



		// Draw Line
		static bool drawWhiteLine(int x1, int y1, int x2, int y2) {
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

		static void drawBlackLine(int x1, int y1, int x2, int y2) {
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

		static void drawRedLine(int x1, int y1, int x2, int y2) {
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


		// Draw Circle
		static void drawSemiCircle(int x0, int y0, int radius)
		{
		    int x = radius;
		    int y = 0;
		    int err = 0;
			int a, b;
			int centerX = x0-1; int centerY = y0; //Scaled
		    while (x >= y)
		    {
				a = x0-x; b = y0+y;
		        drawWhitePoint(a, b);
		        a = x0-y; b = y0+x;
		        drawWhitePoint(x0 - y, y0 + x); 
		        a = x0-y; b = y0-x;
		        drawWhitePoint(x0 - y, y0 - x);
		        a = x0-y; b = y0-x;
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

		}

		static void drawCircle(int x0, int y0, int radius)
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

		// Draw System
		static void drawFrame() {
		    drawWhiteLine(0, 0, 0, heightmatrix);
		    drawWhiteLine(0, heightmatrix, widthMatrix, heightmatrix);
		    drawWhiteLine(widthMatrix, heightmatrix, widthMatrix, 0);
		    drawWhiteLine(widthMatrix, 0, 0, 0);
		}

		static void drawWindow() {
			drawRedLine(xWindow, yWindow, xWindow, yWindow+windowSize);
		    drawRedLine(xWindow+windowSize, yWindow, xWindow+windowSize, yWindow+windowSize);
		    drawRedLine(xWindow, yWindow, xWindow+windowSize, yWindow);
		    drawRedLine(xWindow, yWindow+windowSize, xWindow+windowSize, yWindow+windowSize);
		}

		static void drawView() {
			drawWhiteLine(0, heightmatrix, 0, heightmatrix+viewSize);
		    drawWhiteLine(viewSize, heightmatrix, viewSize, heightmatrix+viewSize);
		    drawWhiteLine(0, heightmatrix+viewSize, viewSize, heightmatrix+viewSize);
		}

		static void drawPointInView(Point p_inWindow) {
			int x1 = p_inWindow.getX(), y1 = p_inWindow.getY();
	    	redPixelMatrix[xView+x1][yView+y1] = 255;
	    	greenPixelMatrix[xView+x1][yView+y1] = 255;
	    	bluePixelMatrix[xView+x1][yView+y1] = 255;
		}


		static void moveWindow(int dx, int dy) {
			int x = xWindow+dx, y = yWindow+dy;
			if (0 <= x && x <= widthMatrix-windowSize && 0 <= y && y <= heightmatrix-windowSize) {
				xWindow += dx; yWindow += dy;
			}
		}

		static void shrinkWindow() {
			if(windowSize > 5) {
				xWindow++; yWindow++;
				windowSize -= 2;
			}
		}

		static void growWindow() {
			if(windowSize < WINDOWSIZE) {
				xWindow--; yWindow--;
				windowSize += 2;
			}
		}

		// Algoritma dasar 
		/* Rotating using xp and yp as its center
		 * and angle in radian */
		void rotate(int xp, int yp, int *x1, int *y1, int angle){
			int x = (*x1) - xp; int y = (*y1) - yp;
			*x1 = cos(angle)*(x) - sin(angle)*(y);
			*y1 = sin(angle)*(x) + cos(angle)*(y);
			*x1 += xp; *y1 += yp;
		}

		static Point scaledByWindowSize(Point& p) {
			int windowSize = Drawer::windowSize, viewSize = Drawer::viewSize;
			double ratio = (double) viewSize/ (double) windowSize;
			Point p1;
			p1.setX((int) p.getX() * ratio);
			p1.setY((int) p.getY() * ratio);
			return p;
		}

		
	private :
		static int redPixelMatrix[WIDTH][HEIGHT];
		static int greenPixelMatrix[WIDTH][HEIGHT];
		static int bluePixelMatrix[WIDTH][HEIGHT];
		static int windowSize; // = WINDOWSIZE; // window yg digeser2
		static int viewSize;// = WINDOWSIZE; // view yg ditampilkan
		static int xView, yWindow, xWindow;// = 0;
		static int yView;// = heightmatrix;

};

int Drawer::windowSize = WINDOWSIZE;
int Drawer::viewSize = WINDOWSIZE;
int Drawer::xView = 0;
int Drawer::yView = heightmatrix;
int Drawer::xWindow = 0;
int Drawer::yWindow = 0;
int Drawer::redPixelMatrix[WIDTH][HEIGHT];
int Drawer::greenPixelMatrix[WIDTH][HEIGHT];
int Drawer::bluePixelMatrix[WIDTH][HEIGHT];
#endif
