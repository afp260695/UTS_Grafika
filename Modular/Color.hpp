#ifndef COLOR_HPP
#define COLOR_HPP

class Color
{
	public:
		Color(double _r, double _g, double _b) {
			setColor(_r, _g, _b);
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

	private:
		double r;       // between 0 and 255
	    double g;       // between 0 and 255
	    double b;       // between 0 and 255
	    static const Color WHITE;
    	static const Color GRAY;
    	static const Color YELLOW;
    	static const Color BLACK;
    	static const Color GREEN;
    	static const Color RED;
    	static const Color RED2;
    	static const Color MAGENTA;
    	static const Color MAGENTA2;
    	static const Color DARKGRAY;
	
};

const Color Color::WHITE = new Color(255,255,255);
const Color Color::GRAY = new Color(128, 128, 128);
const Color Color::YELLOW = new Color(255, 255, 0);
const Color Color::BLACK = new Color(0, 0, 0);
const Color Color::GREEN = new Color(0, 255, 0);
const Color Color::RED = new Color(255, 0, 0);
const Color Color::RED2 = new Color(254, 0, 0);
const Color Color::MAGENTA = new Color(255, 0, 255);
const Color Color::MAGENTA2 = new Color(255, 0, 254);
const Color Color::DARKGRAY = new Color(112, 128, 144);

#endif