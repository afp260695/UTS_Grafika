#ifndef BUILDING_HPP
#define BUILDING_HPP

#include "Point.hpp"
#include "Drawer.hpp"

class Building {
	public :
		Building() {
			for (int i = 0; i < 4; ++i) {
				P[i].setX(0);
				P[i].setY(0);
			}
		}

		Building(Point _P[]) {
			for (int i = 0; i < 4; ++i) {
				P[i].setX(_P[i].getX());
				P[i].setY(_P[i].getY());
			}
		}

		void setBuilding(Point _P[]) {
			for (int i = 0; i < 4; ++i) {
				P[i].setX(_P[i].getX());
				P[i].setY(_P[i].getY());
			}
		}
		
		void drawBuilding() {
			int xWindow = Drawer::getXWindow(), yWindow = Drawer::getYWindow();
			double ratio = Drawer::getRatio();

			bool temp;
			for (int i = 0; i < 3; ++i) {
			 	temp = Drawer::drawWhiteLine(P[i].getX(),P[i].getY(),P[i+1].getX(),P[i+1].getY());
			 	if (Drawer::isInsideWindow(P[i]) && Drawer::isInsideWindow(P[i+1])) {
			 		Point p1_inWindow(P[i].getX()-xWindow, P[i].getY()-yWindow);
			 		Point p2_inWindow(P[i+1].getX()-xWindow, P[i+1].getY()-yWindow);

		    		Drawer::drawWhiteLineInView(p1_inWindow.scale(ratio), p2_inWindow.scale(ratio));
		    	}

			}
			
			temp = Drawer::drawWhiteLine(P[3].getX(),P[3].getY(),P[0].getX(),P[0].getY());
			if (Drawer::isInsideWindow(P[0]) && Drawer::isInsideWindow(P[3])) {
		 		Point p1_inWindow(P[3].getX()-xWindow, P[3].getY()-yWindow);
		 		Point p2_inWindow(P[0].getX()-xWindow, P[0].getY()-yWindow);

	    		Drawer::drawWhiteLineInView(p1_inWindow.scale(ratio), p2_inWindow.scale(ratio));
	    	}
		}

		
	private :
		Point P[4];
};

#endif
