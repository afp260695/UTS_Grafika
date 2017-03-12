#ifndef POINT_HPP
#define POINT_HPP

class Point {
	public :
		Point() {
			x = y = 0;
		}

		Point(int _x, int _y) {
			x = _x; y = _y;
		}
		
		void setX(int _x) {
			x = _x;
		}

		void setY(int _y) {
			y = _y;
		}

		int getX() {
			return x;
		}
		int getY() {
			return y;
		}

		Point scale(double r) {
			double newX = x*r, newY = y*r;
			return Point((int)newX, (int)newY);
		}

	private :
		int x, y;
};

#endif
