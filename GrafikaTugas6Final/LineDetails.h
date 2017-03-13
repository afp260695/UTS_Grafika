#ifndef LINEDETAILS_H
#define LINEDETAILS_H

class LineDetails {
	public:
		double x1;
		double y1;
		double x2;
		double y2;
	
		LineDetails(double x1, double y1, double x2, double y2){
			this->x1 = x1;
			this->y1 = y1;
			this->x2 = x2;
			this->y2 = y2;
		}
		
		LineDetails(){
		}
		
		
};

#endif