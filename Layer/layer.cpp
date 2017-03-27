#include <iostream>
#include <String.h>

using namespace std;

public class Layer {
	private vector<Object> vObject;
	private int nObject;
	private Point p1;
	private Point p2;
	private Point p3;
	private Point p4;
	private String nama;
	private bool show;
	public Layer(vector<Object> vObject, String nama){
		this.vObject = vObject;
		this.nObject = vObject.size();
		this.nama = nama;
		this.show = true;
		int xmin, xmax = vObject[0].getListOfTitik()[0].getX();
		int ymin, ymax = vObject[0].getListOfTitik()[0].getY();
		for (int i=1; i<this.nObject; i++){
			for (int j=1; j < this.vObject[i].nTitik;j++){
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
		this.p1.setX(xmin);
		this.p1.setY(ymin);
		this.p2.setX(xmax);
		this.p2.setY(ymin);
		this.p3.setX(xmax);
		this.p3.setY(ymax);
		this.p4.setX(xmin);
		this.p4.setY(ymax);
	}

	//GETTER
	public vector<Object> getVectorObject(){
		return lObject;
	}

	public int getNObject(){
		return nObject;
	}

	public Point getP1(){
		return p1;
	}

	public Point getP2(){
		return p2;
	}

	public Point getP3(){
		return p3;
	}

	public Point getP4(){
		return p4;
	}

	public String getNama(){
		return nama;
	}

	public bool getShow(){
		return show;
	}

	//SETTER
	public void setVectorObject(vector<Object> vObject){
		this.vObject = vObject;
	}

	public void setNObject(int nObject){
		this.nObject = nObject;
	}

	public void setP1(Point p1){
		this.p1 = p1;
	}

	public void setP2(Point p2){
		this.p2 = p2;
	}

	public void setP3(Point p3){
		this.p3 = p3;
	}

	public void setP4(Point p4){
		this.p4 = p4;
	}

	public void setNama(String nama){
		this.nama = nama;
	}

	public void setShow(bool show){
		this.show = show;
	}

	public void save(String namaFile){
		ofstream myfile;
		myfile.open(namaFile,ios::app);

		if (!myfile.fail()){
			//myfile<<"VectorObj = ";
			for (int i = 0; i < this.nObject; i++){
				myfile<<this.vObject[i].getNama()<<endl;
				myfile<<this.vObject[i].getNTitik()<<endl; 
				myfile<<this.vObject[i].getWarnaBorder().getR()<<endl;
				myfile<<this.vObject[i].getWarnaBorder().getG()<<endl;
				myfile<<this.vObject[i].getWarnaBorder().getB()<<endl;
				myfile<<this.vObject[i].getWarnaIsi().getR()<<endl;
				myfile<<this.vObject[i].getWarnaIsi().getG()<<endl;
				myfile<<this.vObject[i].getWarnaIsi().getB()<<endl;
				myfile<<this.vObject[i].getWeightBorder()<<endl;
				myfile<<this.vObject[i].getShowBorder()<<endl;
				myfile<<this.vObject[i].getShowIsi()<<endl;
				for (int j = 0; j < this.vObject[i].getListofTitik().size(); j++){
					myfile<<this.vObject[i].getListofTitik()[j].getX()<<endl;
					myfile<<this.vObject[i].getListofTitik()[j].getY()<<endl;
				}
				myfile<<"#"<<endl;
				myfile<<this.vObject[i].getShow()<<endl;
				myfile<<"$"<<endl; //ganti object 
			}
			myfile<<"@"<<endl; //object nya abis
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

	public void load(String namaFile){
		ifstream myfile;
		String line;
		myfile.open(namaFile,ios::app);
		int i = 0;
		int j = 0;
		int k = 99;
		int l = 0;
		int x;
		int y;
		String nama;
		int nTitik;
		double red;
		double green;
		double blue;
		Color warnaBorder;
		Color warnaIsi;
		int weightBorder;
		bool showBorder;
		bool showIsi;
		vector<Titik> listofTitik;
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
						nTitik = (int)line;
						j++;
					}
					else if (j == 2){
						red = (double)line;
						j++;
					}
					else if (j == 3){
						green = (double)line;
						j++;
					}
					else if (j == 4){
						blue = (double)line;
						warnaBorder = new Color(red,green,blue);
						j++;
					}
					else if (j == 5){
						red = (double)line;
						j++;
					}
					else if (j == 6){
						green = (double)line;
						j++;
					}
					else if (j == 7){
						blue = (double)line;
						warnaIsi = new Color(red,green,blue);
						j++;
					}
					else if (j == 8){
						weightBorder = (int)line;
						j++;
					}
					else if (j == 9){
						showBorder = line;
						j++;
					}
					else if (j == 10){
						showIsi = line;
						j++;
					}
					else if (j == 11){
						int idx = 0;
						int m = 0;
						while (line != "#"){ //penanda titiknya abis
							if (m == 0){
								x = line;
							}
							else if (m == 1){
								y == line;
								listofTitik[i] = new Titik(x,y);
								m = 0;
								i++;
							}
						}
					}
					else if (j == 12){
						show = line;
						this.vObject[i] = new Object(nama,nTitik,warnaBorder,warnaIsi,weightBorder,showBorder,showIsi,listofTitik,show);
					}

					if (k == 0){
						this.nObject = (int)line;
						k++;
					}
					else if (k == 1){
						if (l == 0){
							x = line;
						}
						else if (l == 1){
							y = line;
						}
						this.p1 = new Point(x,y);
						l = 0;
						k++;
					}
					else if (k == 2){
						if (l == 0){
							x = line;
						}
						else if (l == 1){
							y = line;
						}
						this.p2 = new Point(x,y);
						l = 0;
						k++;
					}
					else if (k == 3){
						if (l == 0){
							x = line;
						}
						else if (l == 1){
							y = line;
						}
						this.p3 = new Point(x,y);
						l = 0;
						k++;
					}
					else if (k == 4){
						if (l == 0){
							x = line;
						}
						else if (l == 1){
							y = line;
						}
						this.p4 = new Point(x,y);
						l = 0;
						k++;
					}
					else if (k == 5){
						this.nama = line;
						k++;
					}
					else if (k == 6){
						this.show = line;
					}
				}
			}
			myfile.close();
		}
	}

	public void addObject(Object object){
		this.vObject.push_back(Object);
		this.nObject += 1;

		//update evelope
		int xmin, xmax = vObject[0].getListOfTitik()[0].getX();
		int ymin, ymax = vObject[0].getListOfTitik()[0].getY();
		for (int i=1; i<this.nObject; i++){
			for (int j=1; j < this.vObject[i].nTitik;j++){
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
		this.p1.setX(xmin);
		this.p1.setY(ymin);
		this.p2.setX(xmax);
		this.p2.setY(ymin);
		this.p3.setX(xmax);
		this.p3.setY(ymax);
		this.p4.setX(xmin);
		this.p4.setY(ymax);
	}

	public void deleteObject(String nama){
		int i = 0;
		while(i < this.nObject){
			if (vObject[i].getNama == nama){
				this.vObject.erase(i);
				this.nObject -=1;
				break;
			}
		}
		//update evelope
		int xmin, xmax = vObject[0].getListOfTitik()[0].getX();
		int ymin, ymax = vObject[0].getListOfTitik()[0].getY();
		for (int i=1; i<this.nObject; i++){
			for (int j=1; j < this.vObject[i].nTitik;j++){
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
		this.p1.setX(xmin);
		this.p1.setY(ymin);
		this.p2.setX(xmax);
		this.p2.setY(ymin);
		this.p3.setX(xmax);
		this.p3.setY(ymax);
		this.p4.setX(xmin);
		this.p4.setY(ymax);
	}

}