#ifndef COLOR_HPP
#define COLOR_HPP

class Color
{
	public:
		/*static const Color WHITE;
    	static const Color GRAY;
    	static const Color YELLOW;
    	static const Color BLACK;
    	static const Color GREEN;
    	static const Color RED;
    	static const Color RED2;
    	static const Color MAGENTA;
    	static const Color MAGENTA2;
    	static const Color DARKGRAY;*/
    	/*static Color WHITE;
    	static Color GRAY;
    	static Color YELLOW;
    	static Color BLACK;
    	static Color GREEN;
    	static Color RED;
    	static Color RED2;
    	static Color MAGENTA;
    	static Color MAGENTA2;
    	static Color DARKGRAY;*/
		// inline Color(double _r, double _g, double _b):r(_r),g(_g),b(_b) {

		// }

		Color(double _r, double _g, double _b) {
			r = _r;
			g = _g;
			b = _b;
		}


		Color() {
			setColor(0, 0, 0);
		}

		bool operator== (const Color &c) {
	        return (r == c.r && g == c.g && b == c.b);
	    }

	    void operator= (const Color &c) {
	    	r = c.r;
	    	g = c.g;
	    	b = c.b;
	    }

	    void setColor(double _r, double _g, double _b) {
	    	r = _r;
	    	g = _g;
	    	b = _b;
	    }

	    double getR() {
	    	return r;
	    }

	    double getG() {
	    	return g;
	    }

	    double getB() {
	    	return b;
	    }

		double r;       // between 0 and 255
	    double g;       // between 0 and 255
	    double b;       // between 0 and 255
		
};

#endif



	/*static const Color WHITE = Color(255,255,255);
    	static const Color GRAY = Color(128, 128, 128);
    	static const Color YELLOW = Color(255, 255, 0);
    	static const Color BLACK = Color(0, 0, 0);
    	static const Color GREEN = Color(0, 255, 0);
    	static const Color RED = Color(255, 0, 0);
    	static const Color RED2 = Color(254, 0, 0);
    	static const Color MAGENTA = Color(255, 0, 255);
    	static const Color MAGENTA2 = Color(255, 0, 254);
    	static const Color DARKGRAY = Color(112, 128, 144);
*/