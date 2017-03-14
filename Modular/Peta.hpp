#ifndef PETA_HPP
#define PETA_HPP
#include "Drawer.hpp"
#include "Color.hpp"
#include <iostream>
#include <fstream>
#include <vector>

using namespace std;

typedef struct {
    vector<int> points;
} Building;

typedef struct {
    double h;       // angle in degrees
    double s;       // a fraction between 0 and 1
    double v;       // a fraction between 0 and 1
} hsv;

class Peta
{
public:

	static Color hsv2Color(hsv in)
	{
	    double      hh, p, q, t, ff;
	    long        i;
	    Color         out;

	    if(in.s <= 0.0) {       // < is bogus, just shuts up warnings
	        out.r = in.v;
	        out.g = in.v;
	        out.b = in.v;
	        return out;
	    }

	    hh = in.h;
	    if(hh >= 360.0) hh = 0.0;
	    hh /= 60.0;
	    i = (long)hh;
	    ff = hh - i;
	    p = in.v * (1.0 - in.s);
	    q = in.v * (1.0 - (in.s * ff));
	    t = in.v * (1.0 - (in.s * (1.0 - ff)));

	    switch(i) {
	    case 0:
	        out.r = in.v;
	        out.g = t;
	        out.b = p;
	        break;
	    case 1:
	        out.r = q;
	        out.g = in.v;
	        out.b = p;
	        break;
	    case 2:
	        out.r = p;
	        out.g = in.v;
	        out.b = t;
	        break;

	    case 3:
	        out.r = p;
	        out.g = q;
	        out.b = in.v;
	        break;
	    case 4:
	        out.r = t;
	        out.g = p;
	        out.b = in.v;
	        break;
	    case 5:
	    default:
	        out.r = in.v;
	        out.g = p;
	        out.b = q;
	        break;
	    }
	    out.r *= 255;
	    out.g *= 255;
	    out.b *= 255;
	    return out;
	}

	static void DrawJalan(int x, int y) {

	    Color roadColor = *GRAY;
		Drawer::drawLine(roadColor,58,505,54,518);Drawer::drawLine(roadColor,55,505,51,518);
		Drawer::drawLine(roadColor,58,505,11,350);Drawer::drawLine(roadColor,54,505,9,350);
		Drawer::drawLine(roadColor,56,509,189,471);Drawer::drawLine(roadColor,56,504,189,466);
		Drawer::drawLine(roadColor,191,470,186,518);Drawer::drawLine(roadColor,188,470,184,518);
		Drawer::drawLine(roadColor,189,473,226,468);Drawer::drawLine(roadColor,189,468,226,463);
		Drawer::drawLine(roadColor,226,466,359,504);Drawer::drawLine(roadColor,226,464,359,501);
		Drawer::drawLine(roadColor,360,503,360,518);Drawer::drawLine(roadColor,356,503,356,518);
		Drawer::drawLine(roadColor,359,505,445,505);Drawer::drawLine(roadColor,359,501,445,501);
		Drawer::drawLine(roadColor,385,289,388,371);Drawer::drawLine(roadColor,381,289,384,371);
		Drawer::drawLine(roadColor,385,371,365,375);Drawer::drawLine(roadColor,385,371,365,375);
		Drawer::drawLine(roadColor,365,378,364,421);Drawer::drawLine(roadColor,365,374,364,418);
		Drawer::drawLine(roadColor,364,421,359,505);Drawer::drawLine(roadColor,364,418,359,501);
		Drawer::drawLine(roadColor,335,289,335,15);Drawer::drawLine(roadColor,331,289,331,15);
		Drawer::drawLine(roadColor,335,63,429,70);Drawer::drawLine(roadColor,331,63,425,70);
		Drawer::drawLine(roadColor,333,141,408,148);Drawer::drawLine(roadColor,333,138,408,144);
		Drawer::drawLine(roadColor,333,168,408,173);Drawer::drawLine(roadColor,333,164,408,169);
		Drawer::drawLine(roadColor,230,465,230,419);Drawer::drawLine(roadColor,223,465,223,419);
		Drawer::drawLine(roadColor,226,421,89,421);Drawer::drawLine(roadColor,226,416,89,416);
		Drawer::drawLine(roadColor,230,419,230,289);Drawer::drawLine(roadColor,223,419,223,289);
		Drawer::drawLine(roadColor,226,291,73,291);Drawer::drawLine(roadColor,226,288,73,288);
		Drawer::drawLine(roadColor,73,291,29,291);Drawer::drawLine(roadColor,73,288,29,288);
		Drawer::drawLine(roadColor,226,291,383,291);Drawer::drawLine(roadColor,226,288,383,288);
		Drawer::drawLine(roadColor,226,421,364,421);Drawer::drawLine(roadColor,226,418,364,418);
		Drawer::drawLine(roadColor,230,289,230,200);Drawer::drawLine(roadColor,223,289,223,200);
		Drawer::drawLine(roadColor,230,200,230,104);Drawer::drawLine(roadColor,223,200,223,104);
		Drawer::drawLine(roadColor,230,36,230,20);Drawer::drawLine(roadColor,223,36,223,20);
		Drawer::drawLine(roadColor,100,23,226,23);Drawer::drawLine(roadColor,100,19,226,19);
		Drawer::drawLine(roadColor,226,23,274,1);Drawer::drawLine(roadColor,226,19,274,0);
		Drawer::drawLine(roadColor,28,289,28,368);Drawer::drawLine(roadColor,30,289,30,368);
		Drawer::drawLine(roadColor,29,366,55,411);Drawer::drawLine(roadColor,29,369,55,414);
		Drawer::drawLine(roadColor,55,411,89,418);Drawer::drawLine(roadColor,55,414,89,420);
		Drawer::drawLine(roadColor,71,289,71,394);Drawer::drawLine(roadColor,74,289,74,394);
		Drawer::drawLine(roadColor,71,394,88,419);Drawer::drawLine(roadColor,74,394,90,419);
		Drawer::drawLine(roadColor,26,289,29,216);Drawer::drawLine(roadColor,31,289,34,216);
		Drawer::drawLine(roadColor,31,214,103,214);Drawer::drawLine(roadColor,31,219,103,219);
		Drawer::drawLine(roadColor,103,215,119,215);Drawer::drawLine(roadColor,103,218,119,218);
		Drawer::drawLine(roadColor,118,216,118,249);Drawer::drawLine(roadColor,120,216,120,249);
		Drawer::drawLine(roadColor,119,248,128,251);Drawer::drawLine(roadColor,119,250,128,254);
		Drawer::drawLine(roadColor,126,253,128,289);Drawer::drawLine(roadColor,129,253,130,289);
		Drawer::drawLine(roadColor,134,104,135,35);Drawer::drawLine(roadColor,130,104,131,35);
		Drawer::drawLine(roadColor,134,38,333,44);Drawer::drawLine(roadColor,134,34,333,40);
		Drawer::drawLine(roadColor,104,216,104,104);Drawer::drawLine(roadColor,101,216,101,104);
		Drawer::drawLine(roadColor,103,106,333,106);Drawer::drawLine(roadColor,103,103,333,103);
		Drawer::drawLine(roadColor,103,150,333,150);Drawer::drawLine(roadColor,103,146,333,146);
		Drawer::drawLine(roadColor,103,203,333,203);Drawer::drawLine(roadColor,103,199,333,199);
		Drawer::drawLine(roadColor,34,216,36,58);Drawer::drawLine(roadColor,30,216,33,58);
		Drawer::drawLine(roadColor,36,59,65,36);Drawer::drawLine(roadColor,34,56,63,34);
		Drawer::drawLine(roadColor,65,36,101,36);Drawer::drawLine(roadColor,63,34,99,34);
		Drawer::drawLine(roadColor,103,35,103,20);Drawer::drawLine(roadColor,99,35,99,20);
		Drawer::drawLine(roadColor,100,21,60,21);Drawer::drawLine(roadColor,100,19,60,19);
		Drawer::drawLine(roadColor,61,21,28,64);Drawer::drawLine(roadColor,59,19,25,61);
		Drawer::drawLine(roadColor,28,63,11,350);Drawer::drawLine(roadColor,25,63,9,350);

	}

	static void drawPohon(int x, int y, int size) {
	    Drawer::drawLine(*WHITE, x+size-1-(size-1)/2, y, x, y+size-1);
	    Drawer::drawLine(*WHITE, x+(size-1)/2, y, x+size-1, y+size-1);
	    Drawer::drawLine(*WHITE, x, y+size-1, x+size-1, y+size-1);
	    Drawer::floodFill(x+(size-1)/2, y+(size-1)/2, *WHITE, *GREEN);
	}

	static void drawAllPohon() {
	    drawPohon(26,18,13);
		drawPohon(50,50,9);
		drawPohon(50,68,9);
		drawPohon(40,81,8);
		drawPohon(40,68,9);
		drawPohon(99,91,9);
		drawPohon(110,78,14);
		drawPohon(43,110,10);
		drawPohon(43,128,10);
		drawPohon(79,176,13);
		drawPohon(160,111,13);
		drawPohon(160,128,13);
		drawPohon(160,156,13);
		drawPohon(160,179,13);
		drawPohon(48,224,16);
		drawPohon(40,263,21);
		drawPohon(243,20,13);
		drawPohon(270,20,13);
		drawPohon(291,20,13);
		drawPohon(313,20,13);
		drawPohon(396,20,36);
		drawPohon(421,165,20);
		drawPohon(314,154,13);
		drawPohon(314,179,13);
		drawPohon(314,110,13);
		drawPohon(314,128,13);
		drawPohon(416,220,24);
		drawPohon(410,271,30);
		drawPohon(395,328,40);
		drawPohon(386,386,45);
		drawPohon(345,109,15);
	}

	static void drawBangunan(vector<Building> &B) {
	    double hue = 0;
	    for (int i = 0; i < B.size() ; i++) {
	        int j;
	        int x_min = 99999999;
	        int x_max = -1;
	        int y_min = 99999999;
	        int y_max = -1;
	        int x_average = 0;
	        int y_average = 0;
	        for (j= 0; j < (B[i].points.size()-2); j += 2) {
	            Drawer::drawLine(*WHITE, B[i].points[j], B[i].points[j+1], B[i].points[j+2], B[i].points[j+3]);
	            x_min = min(x_min,min(B[i].points[j],B[i].points[j+2]));
	            y_min = min(y_min,min(B[i].points[j+1],B[i].points[j+3]));
	            x_max = max(x_max,max(B[i].points[j], B[i].points[j+2]));
	            y_max = max(y_max, max(B[i].points[j+1], B[i].points[j+3]));
	            
	        }

	        x_average = (x_max+x_min) / 2;
	        y_average = (y_max+y_min) / 2;
	        hsv buildingColor = {hue, 0.7, 0.8};
	        Drawer::floodFill(x_average, y_average, *WHITE, hsv2Color(buildingColor));
	        hue += 10;
	        if (hue > 360) {
	            hue = 0;
	        }
	        
	    }    
	}

	static void readBangunanFromFile (ifstream& myfile, vector<Building> &B) {
	    myfile.open ("bangunan.txt");

	    char c;
	    char num[3]; int countchar = 0; int countline = 0;
	    int temp;
	    Building pointTemp;
	    int i = 0;

		while (!myfile.eof() ) {
	        myfile.get(c);
	        if (c == '*') {
	            B.push_back(pointTemp);
	            pointTemp.points.clear();
	            myfile.get(c);
	        } else if ((c == ',') || (c == '|')) {
	        } else {
	       
	            num[countchar] = c;
	            if (countchar < 2) {
	                countchar++;
	            } else {
	                countchar = 0;
	                temp = atoi(num);
	                pointTemp.points.push_back(temp);
	                i++;
	            }
	        }
		}

	    myfile.close();
	}	
};

#endif