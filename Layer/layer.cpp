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
				myfile<<this.vObject[i].getWarnaBorder()<<endl;
				myfile<<this.vObject[i].getWarnaIsi()<<endl;
				myfile<<this.vObject[i].getWeightBorder()<<endl;
				myfile<<this.vObject[i].getShowBorder()<<endl;
				myfile<<this.vObject[i].getShowIsi()<<endl;
				myfile<<this.vObject[i].getListofTitik()<<endl;
				myfile<<this.vObject[i].getShow()<<endl;
				myfile<<"$"<<endl; //ganti object 
			}
			myfile<<"@"<<endl; //object nya abis
			myfile<<getNObject()<<endl;
			myfile<<getP1()<<endl;
			myfile<<getP2()<<endl;
			myfile<<getP3()<<endl;
			myfile<<getP4()<<endl;
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
		String nama;
		int nTitik;
		Color warnaBorder;
		Color warnaIsi;
		int weightBorder;
		bool showBorder;
		bool showIsi;
		vector<Titik> listofTitik;
		bool show;

		int nObject;
		Point p1;
		Point p2;
		Point p3;
		Point p4;
		String nama;
		bool show;

		if (!myfile.fail()){
			while(getline(myfile,line)){
				if(line == "$"){
					i++;
					j = 0;
				}
				else if (line == "@"){
					if (j == 0){
						nObject = (int)line;
					}
					else if (j == 1){
						p1 = line;
					}
					else if (j == 2){
						p2 = line;
					}
					else if (j == 3){
						p3 = line;
					}
					else if (j == 4){
						p4 = line;
					}
					else if (j == 5){
						nama = line;
					}
					else if (j == 6){
						show = line;
						Layer(vObject,nama);
					}

				}
				else{
					if (j == 0){
						nama = line;
					}
					else if (j == 2){
						nTitik = (int)line;
					}
					else if (j == 3){
						warnaBorder = line; //rgb
					}
					else if (j == 4){
						warnaIsi = line; //rgb
					}
					else if (j == 5){
						weightBorder = (int)line;
					}
					else if (j == 6){
						showBorder = line;
					}
					else if (j == 7){
						showIsi = line;
					}
					else if (j == 8){
						listofTitik = line;
					}
					else if (j == 9){
						show = line;
						this.vObject[i] = new Object(nama,nTitik,warnaBorder,warnaIsi,weightBorder,showBorder,showIsi,listofTitik,show);
					}

				}
			}
		}
	}

}