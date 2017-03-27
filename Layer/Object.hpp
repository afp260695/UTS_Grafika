#ifndef OBJECT_HPP
#define OBJECT_HPP

#include <string>
#include "Point.hpp"
#include "Color.hpp"
#include <vector>

using namespace std;
class Object
{
	public:

		Object() {
			nama = "";
			nTitik = 0;
			weightBorder = 0;
			showBorder = true;
			showIsi = true;
			show = true;
		}
		
		Object(string s, int t, Color c1, Color c2, int wb, bool sb, bool si, vector<Point> l, bool sh) {
			nama = s;
			nTitik = t;
			warnaBorder = c1;
			warnaIsi = c2;
			weightBorder = wb;
			showBorder = sb;
			showIsi = si;
			listOfTitik = l;
			show = sh;
		}
		
		void moveObject(int dx, int dy){
			for(int i=0;i<nTitik;i++) {
				listOfTitik[i].setX(listOfTitik[i].getX()+dx);
				listOfTitik[i].setY(listOfTitik[i].getY()+dy);
			}
		}
		
		string getNama(){
			return nama;
		}
		
		int getNTitik(){
			return nTitik;
		}
		
		Color getWarnaBorder(){
			return warnaBorder;
		}
		
		Color getWarnaIsi(){
			return warnaIsi;
		}
		
		int getWeightBorder(){
			return weightBorder;
		}
		
		bool getShowBorder(){
			return showBorder;
		}
		
		bool getShowIsi(){
			return showIsi;
		}
		
		vector<Point> getlistOfTitik(){
			return listOfTitik;
		}
		
		bool getShow(){
			return show;
		}
		
		void setNama(string s){
			nama = s;
		}
		
		void setNTitik(int n){
			nTitik = n;
		}
		
		void setWarnaBorder(Color c){
			warnaBorder = c;
		}
		
		void setWarnaIsi(Color c){
			warnaIsi = c;
		}
		
		void setWeightBorder(int w){
			weightBorder = w;
		}
		
		void setShowBorder(bool b){
			showBorder = b;
		}
		
		void setShowIsi(bool b){
			showIsi = b;
		}
		
		void setlistOfTitik(vector<Point> v){
			listOfTitik = v;
		}
		
		void setShow(bool b){
			show = b;
		}
	
	private:
		string nama;
		int nTitik;
		Color warnaBorder;
		Color warnaIsi;
		int weightBorder;
		bool showBorder;
		bool showIsi;
		vector<Point> listOfTitik;
		bool show;
	    
};

#endif
