#ifndef ROAD_HPP
#define ROAD_HPP

#include "Point.hpp"
#include "Drawer.hpp"
#include <iostream>

using namespace std;

class Road {
	public :
		Road() {
			for (int i = 0; i < 4; ++i) {
				P[i].setX(0);
				P[i].setY(0);
			}
		}

		Road(Point _P[]) {
			for (int i = 0; i < 4; ++i) {
				P[i].setX(_P[i].getX());
				P[i].setY(_P[i].getY());
			}
		}

		void setRoad(Point _P[]) {
			for (int i = 0; i < 4; ++i) {
				P[i].setX(_P[i].getX());
				P[i].setY(_P[i].getY());
			}
		}
		
		void drawRoad() {
			
			int xWindow = Drawer::getXWindow(), yWindow = Drawer::getYWindow();
			double ratio = Drawer::getRatio();
			bool temp;
			
			temp = Drawer::drawWhiteLine(P[0].getX(),P[0].getY(),P[1].getX(),P[1].getY());
			if (Drawer::isInsideWindow(P[0]) && Drawer::isInsideWindow(P[1])) {
		 		Point p1_inWindow(P[0].getX()-xWindow, P[0].getY()-yWindow);
		 		Point p2_inWindow(P[1].getX()-xWindow, P[1].getY()-yWindow);

	    		Drawer::drawWhiteLineInView(p1_inWindow.scale(ratio), p2_inWindow.scale(ratio));
	    	}

			temp = Drawer::drawWhiteLine(P[2].getX(),P[2].getY(),P[3].getX(),P[3].getY());
			if (Drawer::isInsideWindow(P[2]) && Drawer::isInsideWindow(P[3])) {
		 		Point p1_inWindow(P[2].getX()-xWindow, P[2].getY()-yWindow);
		 		Point p2_inWindow(P[3].getX()-xWindow, P[3].getY()-yWindow);

	    		Drawer::drawWhiteLineInView(p1_inWindow.scale(ratio), p2_inWindow.scale(ratio));
	    	}

		}

		
		
	private :
		Point P[4];
};

#endif
