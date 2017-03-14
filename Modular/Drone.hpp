#ifndef DRONE_HPP
#define DRONE_HPP
#include "Drawer.hpp"
class Drone
{
public:
	Drone();
	~Drone();
	static void drawDrone (int x, int y, int width, int length, Color dcolor) {
		int wbawah = width/4;
		//badannya
		Drawer::drawLine(dcolor, x-width, y, x+width, y);
		Drawer::drawLine(dcolor, x+width, y, x+width, y+(wbawah/2));
		Drawer::drawLine(dcolor, x+width, y+(wbawah/2), x+(width-10), y+(wbawah/2));
		Drawer::drawLine(dcolor, x+(width-10), y+(wbawah/2), x+wbawah, y+length);
		Drawer::drawLine(dcolor, x+wbawah, y+length, x-wbawah, y+length);
		Drawer::drawLine(dcolor, x-wbawah, y+length, x-(width-10), y+(wbawah/2));
		Drawer::drawLine(dcolor, x-(width-10), y+(wbawah/2), x-width, y+(wbawah/2));
		Drawer::drawLine(dcolor, x-width, y+(wbawah/2), x-width, y);
		//tangan
		Drawer::drawLine(dcolor, x+width-5, y, x+width-5, y-10);
		Drawer::drawLine(dcolor, x-width, y-10, x-width+10, y-10);
		Drawer::drawLine(dcolor, x-width+5, y, x-width+5, y-10);
		Drawer::drawLine(dcolor, x+width, y-10, x+width-10, y-10);
	}
	
};

// x and y are center point


#endif