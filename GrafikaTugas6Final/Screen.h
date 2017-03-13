#ifndef SCREEN_H
#define SCREEN_H

#include "Line.h"
#include "LineDetails.h"

/// Constants for Screen's border color
#define BORDER_COLOR_RED 255
#define BORDER_COLOR_GREEN 255
#define BORDER_COLOR_BLUE 255

using namespace std;

class Screen {
	private:
		int originX;		// x-coordinate of screen's origin point
		int originY;		// y-coordinate of screen's origin point
		int width;			// screen's width
		int height;			// screen's height
		Line lineManager;	// this variable manages drawing lines in screen

	public:
		/// Constructor
		Screen(int originX, int originY, int width, int height) {
			this->originX = originX;
			this->originY = originY;
			this->width = width;
			this->height = height;
			this->lineManager = Line();
		}
		//Function for change the origin
		void changeOrigin(int originX, int originY) {
			this->originX = originX;
			this->originY = originY;
		} 
		/// This function render screen's border to monitor with color youwant
		void renderBorderCol(int r,int g, int b) {	
			// render north line
			lineManager.insertLine(this->originX, this->originY, 
				this->width+this->originX, this->originY, 
				r,g,b);
			// render east line
			lineManager.insertLine(this->width+this->originX, this->originY, 
				this->width+this->originX, this->height+this->originY, 				
				r,g,b);
			// render south line
			lineManager.insertLine(this->width+this->originX, this->height+this->originY,
				this->originX, this->height+this->originY,
				r,g,b);
			// render west line
			lineManager.insertLine(this->originX, this->height+this->originY,
				this->originX, this->originY, 
				r,g,b);
		}
		
		/// This function render screen's border to monitor
		void renderBorder() {	
			// render north line
			lineManager.insertLine(this->originX, this->originY, 
				this->width+this->originX, this->originY, 
				BORDER_COLOR_BLUE, BORDER_COLOR_GREEN, BORDER_COLOR_BLUE);
			// render east line
			lineManager.insertLine(this->width+this->originX, this->originY, 
				this->width+this->originX, this->height+this->originY, 				
				BORDER_COLOR_BLUE, BORDER_COLOR_GREEN, BORDER_COLOR_BLUE);
			// render south line
			lineManager.insertLine(this->width+this->originX, this->height+this->originY,
				this->originX, this->height+this->originY,
				BORDER_COLOR_BLUE, BORDER_COLOR_GREEN, BORDER_COLOR_BLUE);
			// render west line
			lineManager.insertLine(this->originX, this->height+this->originY,
				this->originX, this->originY, 
				BORDER_COLOR_BLUE, BORDER_COLOR_GREEN, BORDER_COLOR_BLUE);
		}
		
		/// This function returns true if coordinate (x, y) is within screen's boundary
		bool isOnScreen(int x, int y) {
			return (x >= this->originX+1 && x <= this->originX+this->width-1
				&& y >= this->originY+1 && y <= this->originY+this->height-1);
		}
		
		/// This function RENDER A LINE inside screen
		///
		/// !! Will not draw a line outside the screen's boundary
		/// !! Will resize line relative to screen's size
		/// !! Use this to draw object inside screen, do not use Line.insertLine() directly
		///
		/// Example usage:
		/// 	Screen bigScreen = Screen(0, 0, 800, 600);
		///		bigScreen.renderLine(0.1, 0.1, 0.9, 0.9, 255, 255, 255);
		///
		void renderLine(float x1, float y1, float x2, float y2, int red, int green, int blue) {
			// check if line is outside the screen (only accept value between 0 and 1)
			if(x1 < 0 || x1 > 1 || x2 < 0 || x2 > 1 
				|| y1 < 0 || y1 > 1 || y2 < 0 || y2 > 1 ) {
					return;
				}
				
			int newx1 = (int) this->originX + this->width;
			int newy1 = (int) this->originY + this->height;
			int newx2 = (int) this->originX + this->width;
			int newy2 = (int) this->originY + this->height;
			
			// draw line
			lineManager.insertLine(newx1, newy1, newx2, newy2, red, green, blue);
		}
		
		void renderLine(LineDetails *l, int red, int green, int blue) {
			// check if line is outside the screen (only accept value between 0 and 1)
			if(l->x1 < 0 || l->x1 > 1 || l->x2 < 0 || l->x2 > 1 
				|| l->y1 < 0 || l->y1 > 1 || l->y2 < 0 || l->y2 > 1 ) {
					return;
				}
				
			int newx1 = (int) this->originX + l->x1 * this->width;
			int newy1 = (int) this->originY + l->y1 * this->height;
			int newx2 = (int) this->originX + l->x2 * this->width;
			int newy2 = (int) this->originY + l->y2 * this->height;
			
			// draw line
			lineManager.insertLine(newx1, newy1, newx2, newy2, red, green, blue);
		}
		
		//RenderSmall
		//Only used by small screen.
		//View is the moving window, world is the big screeen.
		void renderSmall(list<LineDetails*> l, Screen view, Screen world, int r, int g, int b){
			int x1,x2,y1,y2,tmp1,tmp2, outPoint;
			float skalaX=0, skalaY=0;
			float newx1,newx2,newy1,newy2,xn,yn;
			int outX, outY,outR,cek=0;
		
			for(list<LineDetails*>::iterator it = l.begin(); it != l.end(); it++) {
							
				newx1 = world.originX + (*it)->x1 * world.width;
				newy1 = world.originY + (*it)->y1 * world.height;
				newx2 = world.originX + (*it)->x2 * world.width;
				newy2 = world.originY + (*it)->y2 * world.height;
								
				tmp1 = CohenSutherPos(newx1, newy1, view.originX,  view.originY, (view.originX+view.width), (view.originY+view.height));
				tmp2 = CohenSutherPos(newx2, newy2, view.originX,  view.originY, (view.originX+view.width), (view.originY+view.height));
				cek = 0;	
				
				//CLIPPING		
				while (!cek){
					if ((tmp1 == 0) && (tmp2 == 0)) {
						skalaX = width/view.width;
						skalaY = height/view.height;
						x1 = (newx1-view.originX)*skalaX+originX;
						y1 = (newy1-view.originY)*skalaY+originY;
						x2 = (newx2-view.originX)*skalaX+originX;
						y2 = (newy2-view.originY)*skalaY+originY;				
						lineManager.insertLine(x1,y1,x2,y2,r,g,b);
						cek++;
					}else if ((tmp1&tmp2)) cek++;
					else{
						if (tmp1!=0){
							outR=tmp1;
							outPoint=1;
						}else{
							outPoint=2;
							outR=tmp2;
						}
					
						float m = (newy2-newy1)/(newx2-newx1);
						
						if (outR&8){
							xn = newx1 +(view.originY + view.height-newy1)/m;
							yn = view.originY+view.height;
						}else if (outR&4){
							xn = newx1 + (view.originY - newy1)/m;
							yn = view.originY;
						}else if (outR&1){
							xn = view.originX;
							yn = newy1 + m*(view.originX-newx1);
						}else if (outR&2){
							xn = view.originX+view.width;
							yn = newy1 + m*(view.originX+view.width-newx1);
						}
						
						if (tmp1==outR){
							newx1 = xn;
							newy1 = yn;
							tmp1 = CohenSutherPos(newx1, newy1, view.originX,  view.originY, (view.originX+view.width), (view.originY+view.height));
						}else{
							newx2 = xn;
							newy2 = yn;
							tmp2 = CohenSutherPos(newx2, newy2, view.originX,  view.originY, (view.originX+view.width), (view.originY+view.height));
						}
						
					}
				}
			}
		}
		
		//DefineCase
		//Decide case of view and LineDetails
		//OBSOLATE
		int defineCase(float x1, float y1, float x2, float y2, Screen view, Screen world){
			int tmp1 = 0;
			int tmp2 = 0;
			//First point is inside or on line, make tmp1  = -1
			if ((x1 >= view.originX) && (x1 <= (view.originX+view.width))){
				if ((y1 >= view.originY) && (y1 <= (view.originY+view.height))){
					tmp1 = -1;
				}
			}
			//Second point is inside or on line, make tmp2 = -1
			if ((x2 >= view.originX) && (x2 <= (view.originX+view.width))){
				if ((y2 >= view.originY) && (y2 <= (view.originY+view.height))){
					tmp2 = -1;
				}
			}
			
			//Return -2 if both point is inside
			return (tmp1+tmp2);
		}
		
		//Check position regarding Cohen Sutherland Algorithm
		int CohenSutherPos(float x, float y, int minX, int minY, int maxX,int maxY){
			if (x < minX){
				if (y < minY) {
					return 5; 
				}
				else if (y > maxY) {
					return 9; 
				} 
				else {
					return 1; 
				}
			}
			else if (x > maxX) {
				if (y < minY) {
					return 6; 
				}
				else if (y > maxY) {
					return 10; 
				}
				else {
					return 2; 
				}
			}
			else {
				if (y < minY) {
					return 4; 
				}
				else if (y > maxY) {
					return 8; 
				}
				else {
					return 0; 
				}
			}
		}
};

#endif
