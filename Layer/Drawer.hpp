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

	static void drawObject(Object o){
		if (o.getShow()) {
			int xmin, xmax = o.getListOfTitik()[0].getX();
			int ymin, ymax = o.getListOfTitik()[0].getY();
			for (int j=1; j < o.getNTitik();j++){
				if (xmin > o.getListOfTitik()[j].getX()){
					xmin = o.getListOfTitik()[j].getX();
				}
				if (xmax < o.getListOfTitik()[j].getX()){
					xmax = o.getListOfTitik()[j].getX();
				}
				if (ymin > o.getListOfTitik()[j].getY()){
					ymin = o.getListOfTitik()[j].getY();
				}
				if (ymax < o.getListOfTitik()[j].getY()){
					ymax = o.getListOfTitik()[j].getY();
				}
			}
			int widthTemp = xmax-xmin;
			int heightTemp = ymax-ymin;
			Color** matrixTemp;
			matrixTemp = new Color*[widthTemp];
			for(int k=0;k<widthTemp;k++) {
				matrixTemp[k] = new Color[heightTemp];
			}
			
			for(int i=0;i<o.getNTitik()-1;++i){
				drawLine(&matrixTemp,xmax-xmin,ymax-ymin,o.getWarnaBorder(),o.getListOfTitik()[i].getX()-xmin,o.getListOfTitik()[i].getY(),o.getListOfTitik()[i+1].getX()-xmin,o.getListOfTitik()[i+1].getY());
			}
			drawLine(&matrixTemp,xmax-xmin,ymax-ymin,o.getWarnaBorder(),o.getListOfTitik()[o.getNTitik()-1].getX()-xmin,o.getListOfTitik()[o.getNTitik()-1].getY(),o.getListOfTitik()[0].getX()-xmin,o.getListOfTitik()[0].getY());
			if(o.getShowIsi()){
				//belum
				int xMaks = 0;
				int titikMaks = 0;
				for(int j=0;j<o.getNTitik();j++) {
					if (xMaks < o.getListOfTitik()[j].getX()-xmin) {
						xMaks = o.getListOfTitik()[j].getX()-xmin;
						titikMaks = j;
					}
				}
				int titikIsi[2];
				titikIsi[0] = 0;
				titikIsi[1] = 0;
				for(int j=0;j<2;j++) { 
					if ((titikIsi[0]!=1) && (matrixTemp[xMaks-xmin][o.getListOfTitik()[titikMaks].getY()-1] == o.getWarnaBorder())) {
						titikIsi[j] = 1;
					} else if ((titikIsi[0]!=2) && (matrixTemp[xMaks-xmin-1][o.getListOfTitik()[titikMaks].getY()-1] == o.getWarnaBorder())) {
						titikIsi[j] = 2;
					} else if ((titikIsi[0]!=3) && (matrixTemp[xMaks-xmin-1][o.getListOfTitik()[titikMaks].getY()] == o.getWarnaBorder())) {
						titikIsi[j] = 3;
					} else if ((titikIsi[0]!=4) && (matrixTemp[xMaks-xmin-1][o.getListOfTitik()[titikMaks].getY()+1] == o.getWarnaBorder())) {
						titikIsi[j] = 4;
					} else if ((titikIsi[0]!=5) && (matrixTemp[xMaks-xmin][o.getListOfTitik()[titikMaks].getY()+1] == o.getWarnaBorder())) {
						titikIsi[j] = 5;
					}
				}
				int initX;
				int initY;
				if ((titikIsi[0] == 1) && (titikIsi[1] == 2)) {
					initX = o.getListOfTitik()[titikMaks].getX()-xmin-1;
					initY = o.getListOfTitik()[titikMaks].getY()-ymin-3;
				} else if ((titikIsi[0] == 2) && (titikIsi[1] == 3)) {
					initX = o.getListOfTitik()[titikMaks].getX()-xmin-3;
					initY = o.getListOfTitik()[titikMaks].getY()-ymin-1;
				} else if ((titikIsi[0] == 3) && (titikIsi[1] == 4)) {
					initX = o.getListOfTitik()[titikMaks].getX()-xmin-3;
					initY = o.getListOfTitik()[titikMaks].getY()-ymin+1;
				} else if ((titikIsi[0] == 4) && (titikIsi[1] == 5)) {
					initX = o.getListOfTitik()[titikMaks].getX()-xmin-1;
					initY = o.getListOfTitik()[titikMaks].getY()-ymin+3;
				} else {
					initX = o.getListOfTitik()[titikMaks].getX()-xmin-1;
					initY = o.getListOfTitik()[titikMaks].getY()-ymin-3+((titikIsi[0] + titikIsi[1])/2);
				}
				floodFill(&matrixTemp,xmax-xmin,ymax-ymin,initX,initY,o.getWarnaBorder(),o.getWarnaIsi());
			}
			if (!o.getShowBorder()) {
				Color blackTemp;
				blackTemp.setColor(0,0,0);
				for(int i=0;i<o.getNTitik()-1;++i){
					drawLine(&matrixTemp,widthTemp,heightTemp,blackTemp,o.getListOfTitik()[i].getX()-xmin,o.getListOfTitik()[i].getY(),o.getListOfTitik()[i+1].getX()-xmin,o.getListOfTitik()[i+1].getY());
				}
				drawLine(&matrixTemp,widthTemp,heightTemp,blackTemp,o.getListOfTitik()[o.getNTitik()-1].getX()-xmin,o.getListOfTitik()[o.getNTitik()-1].getY(),o.getListOfTitik()[0].getX()-xmin,o.getListOfTitik()[0].getY());
			}
			for(int i=0;i<widthTemp;i++) {
				for(int j=0;j<heightTemp;j++) {
					pixelMatrix[xmin+i][ymin+j] = matrixTemp[i][j];
				}
			}
		}
	}

	static bool drawLine(Color*** matrixTemp, int widthTemp, int heightTemp, Color color, int x1, int y1, int x2, int y2) {
	    bool ret = false;
	    int deltaX = x2 - x1;
	    int deltaY = y2 - y1;
	    int ix = deltaX > 0 ? 1 : -1;
	    int iy = deltaY > 0 ? 1 : -1;
	    deltaX = abs(deltaX);
	    deltaY = abs(deltaY);
	    int x = x1;
	    int y = y1;
	    drawPoint(matrixTemp,widthTemp,heightTemp,color, x, y);
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
	            drawPoint(matrixTemp, widthTemp,heightTemp,color, x, y);
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
	            drawPoint(matrixTemp, widthTemp,heightTemp,color, x, y);
	        }
	    }
	    return ret;
	}

	static void drawPoint(Color*** matrixTemp,int widthTemp,int heightTemp, Color color, int x, int y) {
	    if (x < 0 || x >= widthTemp || y < 0 || y >= heightTemp) return;
	    matrixTemp[x][y] = &color;
	}

	static void floodFill(Color*** matrixTemp,int widthTemp,int heightTemp, int x, int y, Color batas, Color warna){
	    if((x>=0 && x<widthTemp) && (y>=0 && y<heightTemp)){
	        if(
	            !(
	                (matrixTemp[x][y] == &batas) ||
	                (matrixTemp[x][y] == &warna)
	            )
	        ){
	            matrixTemp[x][y] = &warna;
	            floodFill(matrixTemp, widthTemp, heightTemp, x,y+1, batas, warna);
	            floodFill(matrixTemp, widthTemp, heightTemp, x+1,y, batas, warna);
	            floodFill(matrixTemp, widthTemp, heightTemp, x,y-1, batas, warna);
	            floodFill(matrixTemp, widthTemp, heightTemp, x-1,y, batas, warna);
	        }
	    }
	}

	static Color pixelMatrix[WIDTH][HEIGHT];
};

Color Drawer::pixelMatrix[WIDTH][HEIGHT];

#endif
