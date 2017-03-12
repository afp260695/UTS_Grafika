#ifndef TREE_HPP
#define TREE_HPP

#include "Point.hpp"
#include "Drawer.hpp"

class Tree {
	public :
		Tree() {
			P.setX(0);
			P.setY(0);
		}

		Tree(Point _P) {
			P.setX(_P.getX());
			P.setY(_P.getY());
		}

		void setTree(Point _P) {
			P.setX(_P.getX());
			P.setY(_P.getY());
		}
		
		void drawTree() {
			Drawer::drawCircle(P.getX(),P.getY(),10);
		}

		
		
	private :
		Point P;
};

#endif
