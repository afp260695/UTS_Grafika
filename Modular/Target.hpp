#ifndef TARGET_HPP
#define TARGET_HPP
#include "Color.hpp"
#include "Drawer.hpp"
#define WIDTH 1300
#define HEIGHT 700
#define WIDTH_REAL 1200
#define HEIGHT_REAL 600	

class Target
{
public:

	
	void drawKepala(int x, int y, int scaling, Color warna, Color batas){
	    drawLineOrang(batas, x, y, x+2*scaling, y);
	    drawLineOrang(batas, x+2*scaling, y, x+2*scaling, y+2*scaling);
	    drawLineOrang(batas, x, y, x, y+2*scaling);
	    drawLineOrang(batas, x, y+2*scaling, x+2*scaling, y+2*scaling);
	    floodFillOrang(x+1, y+1, batas, warna);
	}

	void drawBadan(int x, int y, int scaling, Color warna, Color batas){
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

	void drawTarget(int x, int y, int scaling, Color warna, Color batas){
	    drawKepala(x,y,scaling,warna,batas);
	    drawBadan(x,y,scaling,warna,batas);
	}

	void behaviourOrang(int scaling, int speed, int range){
	    if(targetAlive){
	        drawTarget(targetX,targetY,scaling,targetColor, *MAGENTA2);

	        targetX = targetX + speed*targetDir;
	        targetUjung += targetDir;
	        if (targetUjung >= range){
	            targetDir = -1;
	        }
	        if (targetUjung <= 0) {
	            targetDir = 1;
	        }
	        kepalaY = targetY;
	        kepalaX = targetX;
	    } else {
	        if (!stop) {
	            if (turun) {
	                if (kepalaY < targetY+(8*scaling)){
	                    kepalaX += kecX;
	                    kepalaY += kecY;
	                    hitungan ++;
	                    if ((hitungan % 3) == 0) {
	                        kecY++;   
	                    }
	                } else {
	                    turun = false;
	                    hitungan = 0;
	                }
	            } else {
	                if ((kecY <= 1) && (kepalaY >= targetY+(8*scaling))) {
	                    stop = true;
	                }
	                if (kecY > 0) {
	                    kepalaX += kecX;
	                    kepalaY -= kecY;
	                    if ((hitungan % 3) == 0) {
	                        kecY -= 2;   
	                    }
	                } else {
	                    turun = true;
	                    hitungan = 0;
	                }
	            }
	        }
	        drawKepala(kepalaX,kepalaY,scaling,targetColor, *MAGENTA2);
	    }
	}

	bool drawLineOrang(Color color, int x1, int y1, int x2, int y2) {
	    bool ret = false;

	    int deltaX = x2 - x1;
	    int deltaY = y2 - y1;
	    int ix = deltaX > 0 ? 1 : -1;
	    int iy = deltaY > 0 ? 1 : -1;
	    deltaX = abs(deltaX);
	    deltaY = abs(deltaY);

	    int x = x1;
	    int y = y1;

	    if(Drawer::pixelMatrix[x][y] == *BLACK)
	        Drawer::drawPoint(color, x, y);

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
	            if(Drawer::pixelMatrix[x][y] == *BLACK)
	                Drawer::drawPoint(color, x, y);
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
	            if(Drawer::pixelMatrix[x][y] == *BLACK)
	                Drawer::drawPoint(color, x, y);
	        }
	    }
	    return ret;
	}

	void floodFillOrang(int x, int y, Color batas, Color warna){
	    if((x>=0 && x<WIDTH) && (y>=0 && y<HEIGHT)){
	        if(Drawer::pixelMatrix[x][y] == *BLACK){
	            Drawer::pixelMatrix[x][y] = warna;
	            floodFillOrang(x,y+1, batas, warna);
	            floodFillOrang(x+1,y, batas, warna);
	            floodFillOrang(x,y-1, batas, warna);
	            floodFillOrang(x-1,y, batas, warna);
	        }
	    }
	}



	Color targetColor;
	
	int kepalaX;
	int kepalaY;
	int kecX;
	int kecY;
	int targetDir;
	int targetUjung;
	bool stop;
	bool turun;
	int hitungan;

	int targetX;
	int targetY;
	bool targetAlive;
	
};

#endif