#include "Drawer.hpp"
#include "Layer.hpp"
#include <vector>

vector<Layer> vLayer;
int layerSelected;

void selectLayerByName(string nama) {
	for(int i=0;i<vLayer.size();i++) {
		if (vLayer[i].getNama() == nama) {
			layerSelected = i;
			break;
		}
	}
}

int searchLayerByName(string nama) {
	for(int i=0;i<vLayer.size();i++) {
		if (vLayer[i].getNama() == nama) {
			return i;
		}
	}
}

void deleteLayer(string namaLayer){
	int i = 0;
	int batas = vLayer.size();
	while(i < batas){
		if (vLayer[i].getNama() == namaLayer){
			vLayer.erase(vLayer.begin()+i);
			if (layerSelected == i) {
				layerSelected = -1;
			}
			break;
		}
	}
}

void addLayer(Layer layer) {
	vLayer.push_back(layer);
}

void bringFront() {
	if (layerSelected != vLayer.size()) {
		Layer temp = vLayer[layerSelected];
		vLayer[layerSelected] = vLayer[layerSelected+1];
		vLayer[layerSelected+1] = temp;
		layerSelected++;
	}
}

void bringBack() {
	if (layerSelected != 0) {
		Layer temp = vLayer[layerSelected];
		vLayer[layerSelected] = vLayer[layerSelected-1];
		vLayer[layerSelected-1] = temp;
		layerSelected--;
	}
}

void moveObjectToAnotherLayer(string nama) {
	int destLayer = searchLayerByName(nama);
	int srcLayer = layerSelected;
	Object temp = vLayer[srcLayer].getVectorObject()[vLayer[srcLayer].getObjectSelected()];
	vLayer[destLayer].addObject(temp);
	vLayer[srcLayer].deleteObject(temp.getNama());
}

int main(){
	//change object size ratio
	//change fill style
	//change border weight
	//change border style
	layerSelected = -1;
	string nama = "yonglex";
	Layer temp(nama);
	
	addLayer(temp);
	
	
	return 0;
}
