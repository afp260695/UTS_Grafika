#include <iostream>
#include <string>
#include <fstream>
#include "Color.hpp"
#include "Drawer.hpp"

using namespace std;

class Layer {
	private:
		vector<Object> vObject;
		int nObject;
		Point p1;
		Point p2;
		Point p3;
		Point p4;
		string nama;
		bool show;

	public:
	Layer(vector<Object> vObject, string nama){
		this->vObject = vObject;
		this->nObject = vObject.size();
		this->nama = nama;
		this->show = true;
		updateEnvelope();
	}

	//GETTER
	vector<Object> getVectorObject(){
		return vObject;
	}

	int getNObject(){
		return nObject;
	}

	Point getP1(){
		return p1;
	}

	Point getP2(){
		return p2;
	}

	Point getP3(){
		return p3;
	}

	Point getP4(){
		return p4;
	}

	string getNama(){
		return nama;
	}

	bool getShow(){
		return show;
	}

	//SETTER
	void setVectorObject(vector<Object> vObject){
		this->vObject = vObject;
	}

	void setNObject(int nObject){
		this->nObject = nObject;
	}

	void setP1(Point p1){
		this->p1 = p1;
	}

	void setP2(Point p2){
		this->p2 = p2;
	}

	void setP3(Point p3){
		this->p3 = p3;
	}

	void setP4(Point p4){
		this->p4 = p4;
	}

	void setNama(string nama){
		this->nama = nama;
	}

	void setShow(bool show){
		this->show = show;
	}

	void save(string namaFile){
		ofstream myfile;
		myfile.open(namaFile.c_str());

		if (!myfile.fail()){
			//myfile<<"VectorObj = ";
			for (int i = 0; i < this->nObject; i++){
				myfile<<this->vObject[i].getNama()<<endl;
				myfile<<this->vObject[i].getNTitik()<<endl; 
				myfile<<this->vObject[i].getWarnaBorder().getR()<<endl;
				myfile<<this->vObject[i].getWarnaBorder().getG()<<endl;
				myfile<<this->vObject[i].getWarnaBorder().getB()<<endl;
				myfile<<this->vObject[i].getWarnaIsi().getR()<<endl;
				myfile<<this->vObject[i].getWarnaIsi().getG()<<endl;
				myfile<<this->vObject[i].getWarnaIsi().getB()<<endl;
				myfile<<this->vObject[i].getWeightBorder()<<endl;
				myfile<<this->vObject[i].getShowBorder()<<endl;
				myfile<<this->vObject[i].getShowIsi()<<endl;
				for (int j = 0; j < this->vObject[i].getListOfTitik().size(); j++){
					myfile<<this->vObject[i].getListOfTitik()[j].getX()<<endl;
					myfile<<this->vObject[i].getListOfTitik()[j].getY()<<endl;
				}
				myfile<<"#"<<endl; // akhir semua titik
				myfile<<this->vObject[i].getShow()<<endl;
				myfile<<"$"<<endl; //ganti object 
			}
			myfile<<"@"<<endl; //akhir semua object
			myfile<<getNObject()<<endl;
			myfile<<getP1().getX()<<endl;
			myfile<<getP1().getY()<<endl;
			myfile<<getP2().getX()<<endl;
			myfile<<getP2().getY()<<endl;
			myfile<<getP3().getX()<<endl;
			myfile<<getP3().getY()<<endl;
			myfile<<getP4().getX()<<endl;
			myfile<<getP4().getY()<<endl;
			myfile<<getNama()<<endl;
			myfile<<getShow()<<endl;
			myfile.close();
		}
	}

	void load(string namaFile){
		ifstream myfile;
		string line;
		myfile.open(namaFile.c_str());
		int i = 0;
		int j = 0;
		int k = 99;
		int l = 0;
		int x;
		int y;
		string nama;
		int nPoint;
		double red;
		double green;
		double blue;
		Color warnaBorder;
		Color warnaIsi;
		int weightBorder;
		bool showBorder;
		bool showIsi;
		vector<Point> listofPoint;
		bool show;

		if (!myfile.fail()){
			while(getline(myfile,line)){
				if(line == "$"){ //penanda ganti object
					i++;
					j = 0;
				}
				else if (line == "@"){ //penanda objectnya abis
					k = 0;
				}
				else{
					if (j == 0){
						nama = line;
						j++;
					}
					else if (j == 1){
						nPoint = stoi(line);
						j++;
					}
					else if (j == 2){
						red = stod(line);
						j++;
					}
					else if (j == 3){
						green = stod(line);
						j++;
					}
					else if (j == 4){
						blue = stod(line);
						warnaBorder.setColor(red,green,blue);
						j++;
					}
					else if (j == 5){
						red = stod(line);
						j++;
					}
					else if (j == 6){
						green = stod(line);
						j++;
					}
					else if (j == 7){
						blue = stod(line);
						warnaIsi.setColor(red,green,blue);
						j++;
					}
					else if (j == 8){
						weightBorder = stoi(line);
						j++;
					}
					else if (j == 9){
						showBorder = toBool(line);
						j++;
					}
					else if (j == 10){
						showIsi = toBool(line);
						j++;
					}
					else if (j == 11){
						int idx = 0;
						int m = 0;
						int l = 0;
						while (line != "#"){ //penanda Pointnya abis
							if (m == 0){
								x = stoi(line);
							}
							else if (m == 1){
								y == stoi(line);
								listofPoint[l].setX(x);
								listofPoint[l].setY(y);
								m = 0;
								l++;
							}
						}
					}
					else if (j == 12){
						show = toBool(line);
						//this->vObject[i] = new Object(nama,nPoint,warnaBorder,warnaIsi,weightBorder,showBorder,showIsi,listofPoint,show);
						this->vObject[i].setNama(nama);
						this->vObject[i].setNTitik(nPoint);
						this->vObject[i].setWarnaBorder(warnaBorder);
						this->vObject[i].setWarnaIsi(warnaIsi);
						this->vObject[i].setWeightBorder(weightBorder);
						this->vObject[i].setShowBorder(showBorder);
						this->vObject[i].setShowIsi(showIsi);
						this->vObject[i].setListOfTitik(listofPoint);
						this->vObject[i].setShow(show);
						j++;
					}

					if (k == 0){
						this->nObject = stoi(line);
						k++;
					}
					else if (k == 1){
						if (l == 0){
							x = stoi(line);
						}
						else if (l == 1){
							y = stoi(line);
						}
						this->p1.setX(x);
						this->p1.setY(y);
						l = 0;
						k++;
					}
					else if (k == 2){
						if (l == 0){
							x = stoi(line);
						}
						else if (l == 1){
							y = stoi(line);
						}
						this->p2.setX(x);
						this->p2.setY(y);
						l = 0;
						k++;
					}
					else if (k == 3){
						if (l == 0){
							x = stoi(line);
						}
						else if (l == 1){
							y = stoi(line);
						}
						this->p3.setX(x);
						this->p3.setY(y);
						l = 0;
						k++;
					}
					else if (k == 4){
						if (l == 0){
							x = stoi(line);
						}
						else if (l == 1){
							y = stoi(line);
						}
						this->p4.setX(x);
						this->p4.setY(y);
						l = 0;
						k++;
					}
					else if (k == 5){
						this->nama = line;
						k++;
					}
					else if (k == 6){
						this->show = toBool(line);
					}
				}
			}
			myfile.close();
		}
	}

	void addObject(Object object){
		this->vObject.push_back(object);
		this->nObject += 1;

		//update evelope
		updateEnvelope();
	}

	void deleteObject(string namaObject){
		int i = 0;
		while(i < this->nObject){
			if (vObject[i].getNama() == namaObject){
				this->vObject.erase(vObject.begin()+i);
				this->nObject -=1;
				break;
			}
		}
		//update evelope
		updateEnvelope();
		
	}

	bool toBool(string str){
		if (str == "1"){
			return true;
		}
		else{
			return false;
		}
	}

	void updateEnvelope(){
		int xmin, xmax = vObject[0].getListOfTitik()[0].getX();
		int ymin, ymax = vObject[0].getListOfTitik()[0].getY();
		for (int i=1; i<this->nObject; i++){
			for (int j=1; j < this->vObject[i].getNTitik();j++){
				if (xmin > vObject[i].getListOfTitik()[j].getX()){
					xmin = vObject[i].getListOfTitik()[j].getX();
				}
				if (xmax < vObject[i].getListOfTitik()[j].getX()){
					xmax = vObject[i].getListOfTitik()[j].getX();
				}
				if (ymin > vObject[i].getListOfTitik()[j].getY()){
					ymin = vObject[i].getListOfTitik()[j].getY();
				}
				if (ymax < vObject[i].getListOfTitik()[j].getY()){
					ymax = vObject[i].getListOfTitik()[j].getY();
				}
			}
		}
		this->p1.setX(xmin);
		this->p1.setY(ymin);
		this->p2.setX(xmax);
		this->p2.setY(ymin);
		this->p3.setX(xmax);
		this->p3.setY(ymax);
		this->p4.setX(xmin);
		this->p4.setY(ymax);
	}

};