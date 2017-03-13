#include "Screen.h"
#include "ncurses.h"
#include <fstream>
#include <iostream>
#include <list>

using namespace std;


double getRatio(int x) {
	double ret = (double) x/417;
	return ret;
}

void drawTree(int x, int y, list<LineDetails*> listLine) {
	cout<<x<<" "<<y<<endl;
	//cout<<listLine.back()<<endl;
	listLine.push_back(new LineDetails(getRatio(x), getRatio(y), getRatio(x), getRatio(y+3)));
	listLine.push_back(new LineDetails(getRatio(x-3), getRatio(y), getRatio(x), getRatio(y-6)));
	listLine.push_back(new LineDetails(getRatio(x), getRatio(y-6), getRatio(x+3), getRatio(y)));
	listLine.push_back(new LineDetails(getRatio(x+3), getRatio(y), getRatio(x-3), getRatio(y)));
	//cout<<listLine.back()<<endl;
}

int main() {
	int varZoom = 3;
	// Big screen
	// !!! width:height ratio = 4:3
	int bigScreenWidth = 415;
	int bigScreenHeight = 415;	
	Screen bigScreen = Screen(0, 0, bigScreenWidth, bigScreenHeight);
	
	// Small screen
	// !!! its size is relative to bigScreen
	int smallScreenWidth = bigScreenWidth/3;
	int smallScreenHeight = bigScreenHeight/3;	
	Screen smallScreen = Screen(bigScreenWidth, bigScreenHeight-smallScreenHeight, smallScreenWidth, smallScreenHeight);
	
	//ZOOM Screen
	// size relative to bigscreen
	int zoomScreenWidth = bigScreenWidth/3;
	int zoomScreenHeight = bigScreenHeight/3;	
	Screen zoomScreen = Screen(0, 0, zoomScreenWidth, zoomScreenHeight);
	int xa = 0; 
	int ya = 0;
	
	//List of LineDetails
	int plus=0;
	//LineDetails is 2 point defining the line.
	list<LineDetails*> listLine;
	list<LineDetails*> listTree;
	list<LineDetails*> listPath;
	list<LineDetails*> listFlower;
	bool lineDisplay = 1;
	bool treeDisplay = 1;
	bool pathDisplay = 1;
	bool flowerDisplay = 1;
	
	//read from file to built ITB depan
	fstream myfile("coordinat.txt", std::ios_base::in);
	
	list<int> intList;
	
    int a;
    while (myfile >> a)
    {
        intList.push_back(a);
    }
    
    int* temp = new int[4];
    
    int loop = 0;
    for (std::list<int>::iterator it = intList.begin(); it != intList.end(); ++it) {
		temp[loop] = *it;
		loop++;
		if (loop == 4) {
			listLine.push_back(new LineDetails(getRatio(temp[0]), getRatio(temp[1]), getRatio(temp[2]), getRatio(temp[3])));
			loop = 0;
		}
	}

	myfile.close();
	

	// DRAW TREE
	fstream treefile("coordinate_tree.txt",std::ios_base::in);
	list<int> intTreeList;
	
    int b;
    while (treefile >> b)
    {
        intTreeList.push_back(b);
    }
    
    int* tempT = new int[2];
    
    loop = 0;
    for (std::list<int>::iterator it = intTreeList.begin(); it != intTreeList.end(); ++it) {
		tempT[loop] = *it;
		loop++;
		if (loop == 2) {
			listTree.push_back(new LineDetails(getRatio(tempT[0]), getRatio(tempT[1]), getRatio(tempT[0]), getRatio(tempT[1]+5)));
			listTree.push_back(new LineDetails(getRatio(tempT[0]-5), getRatio(tempT[1]), getRatio(tempT[0]), getRatio(tempT[1]-10)));
			listTree.push_back(new LineDetails(getRatio(tempT[0]), getRatio(tempT[1]-10), getRatio(tempT[0]+5), getRatio(tempT[1])));
			listTree.push_back(new LineDetails(getRatio(tempT[0]+5), getRatio(tempT[1]), getRatio(tempT[0]-5), getRatio(tempT[1])));
			loop = 0;
		}
	}	

	treefile.close();
	
	//Draw Path
	fstream pathfile("coordinat_path.txt", std::ios_base::in);

	list<int> intListP;
	
	int c;
    while (pathfile >> c)
    {
        intListP.push_back(c);
    }

    
	loop = 0;
    for (std::list<int>::iterator it = intListP.begin(); it != intListP.end(); ++it) {
		temp[loop] = *it;
		loop++;
		if (loop == 4) {
			listPath.push_back(new LineDetails(getRatio(temp[0]), getRatio(temp[1]+3), getRatio(temp[2]), getRatio(temp[3]+3)));
			loop = 0;
		}
	}

	pathfile.close();
	//Draw FLower
	fstream flowerfile("coordinat_flower.txt", std::ios_base::in);

	list<int> intListF;
	
	int d;
    while (flowerfile >> d)
    {
        intListF.push_back(d);
    }

    
	loop = 0;
    for (std::list<int>::iterator it = intListF.begin(); it != intListF.end(); ++it) {
		temp[loop] = *it;
		loop++;
		if (loop == 4) {
			listFlower.push_back(new LineDetails(getRatio(temp[0]), getRatio(temp[1]), getRatio(temp[2]), getRatio(temp[3])));
			loop = 0;
		}
	}

	flowerfile.close();
	
	//Next Step
	system("clear"); 
	initscr();
	keypad(stdscr,TRUE);
	noecho();
	char ch;
	while(1) {
		system("clear");
		// example
		if (lineDisplay){
			for(list<LineDetails*>::iterator it = listLine.begin(); it != listLine.end(); it++) {
				bigScreen.renderLine((*it),255,255,255);
			}
			smallScreen.renderSmall(listLine, zoomScreen, bigScreen,255,255,255);			
		}
		if (treeDisplay) {
			for(list<LineDetails*>::iterator it = listTree.begin(); it != listTree.end(); it++) {
				bigScreen.renderLine((*it),0,255,0);
			}
			smallScreen.renderSmall(listTree, zoomScreen, bigScreen,0,255,0);
		}
		if (pathDisplay) {
			for(list<LineDetails*>::iterator it = listPath.begin(); it != listPath.end(); it++) {
				bigScreen.renderLine((*it),0,0,255);
			}
			smallScreen.renderSmall(listPath, zoomScreen, bigScreen,0,0,255);
		}
		if (flowerDisplay) {
			for(list<LineDetails*>::iterator it = listFlower.begin(); it != listFlower.end(); it++) {
				bigScreen.renderLine((*it),255,0,0);
			}
			smallScreen.renderSmall(listFlower, zoomScreen, bigScreen,255,0,0);
		}
		bigScreen.renderBorder();
		smallScreen.renderBorder();
		zoomScreen.renderBorder();
		ch = getch();
		//smallScreen.renderSmall(listLine, zoomScreen, bigScreen,0,0,0);
		
		//mvaddch(ya,xa,ch);		
		switch(ch){
		case 'a':
		  if (xa>0) { 
		  xa--;
		  zoomScreen.renderBorderCol(0,0,0);
		  zoomScreen.changeOrigin(xa,ya);
		  zoomScreen.renderBorder();
		  }
       		break;
		case 'd':
		//smallScreen.renderSmall(listLine, zoomScreen, bigScreen,0,0,0);
		  if (xa+zoomScreenWidth<bigScreenWidth) {
		  xa++;
		  zoomScreen.renderBorderCol(0,0,0);
		  zoomScreen.changeOrigin(xa,ya);
		  zoomScreen.renderBorder();
		  }
		break;
		case 'w':
		//smallScreen.renderSmall(listLine, zoomScreen, bigScreen,0,0,0);
		  if (ya>0) {
		  ya--;
		  zoomScreen.renderBorderCol(0,0,0);
		  zoomScreen.changeOrigin(xa,ya);
		  zoomScreen.renderBorder();
		  }
		break;
		case 's':
		//smallScreen.renderSmall(listLine, zoomScreen, bigScreen,0,0,0);
		  if (ya+zoomScreenHeight<bigScreenHeight) {
		  ya++;
		  zoomScreen.renderBorderCol(0,0,0);
		  zoomScreen.changeOrigin(xa,ya);
		  zoomScreen.renderBorder();
		  }		
		break;
		case 'i':
		//smallScreen.renderSmall(listLine, zoomScreen, bigScreen,0,0,0);
		  //3 6 9 18 60 102
		  if (varZoom == 3) {
				varZoom = 6;
		  }
		  else if (varZoom == 6) {
				varZoom = 9;
		  }
		  else if (varZoom == 9) {
				varZoom = 18;
		  }
		  else if (varZoom == 18) {
				varZoom = 60;
		  }
		  else if (varZoom == 60) {
				varZoom = 105;
		  }
		  else {
		  }
		  zoomScreenWidth = bigScreenWidth/varZoom;
		  zoomScreenHeight = bigScreenHeight/varZoom;	
		  zoomScreen = Screen(xa, ya, zoomScreenWidth, zoomScreenHeight);
		break;
		case 'o':
		//smallScreen.renderSmall(listLine, zoomScreen, bigScreen,0,0,0);
		  if (varZoom == 105) {
				varZoom = 60;
		  }
		  else if (varZoom == 60) {
				varZoom = 18;
		  }
		  else if (varZoom == 18) {
				varZoom = 9;
		  }
		  else if (varZoom == 9) {
				varZoom = 6;
		  }
		  else if (varZoom == 6) {
				varZoom = 3;
		  }
		  else {
		  }
		  zoomScreenWidth = bigScreenWidth/varZoom;
		  zoomScreenHeight = bigScreenHeight/varZoom;
		  zoomScreen = Screen(xa, ya, zoomScreenWidth, zoomScreenHeight);		
		break;
		case 'z':
			if (lineDisplay == 1){
				lineDisplay = 0;
			}
			else {
				lineDisplay = 1;
			}
		break;
		case 'x':
			if (treeDisplay == 1){
				treeDisplay = 0;
			}
			else {
				treeDisplay = 1;
			}
		break;
		case 'c':
			if (pathDisplay == 1){
				pathDisplay = 0;
			}
			else {
				pathDisplay = 1;
			}	
		break;
		case 'v':
			if (flowerDisplay == 1){
				flowerDisplay = 0;
			}
			else {
				flowerDisplay = 1;
			}	
		break;
		}
		
		refresh();
	}
	
	return 0;
}
