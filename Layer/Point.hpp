#ifndef POINT_HPP
#define POiNT_HPP

class Point
{
	public:

		Point() {
			x = 0;
			y = 0;
		}
		
		Point(int a, int b){
			x = a;
			y = b;
		}
		
		int getX(){
			return x;
		}
		
		int getY(){
			return y;
		}
		
		void setX(int a){
			x = a;
		}
		
		void setY(int b){
			y = b;
		}
		
	private:
		int x;
		int y;
};

#endif
