#include <stdio.h>

// Waktu Run, langsung export ke file  coordinate_tree.txt
// ./treeLocator > coordinate_tree.txt

int main () {
	int i;


	// KIRI BAWAH
	for (i = 1; i<7; i++) {
		printf("%d 405\n",i*10+5);
	}

	for (i = 1; i<14; i++) {
		printf("%d 385\n",i*10+5);
	}

	for (i = 1; i<14; i++) {
		printf("%d 365\n",i*10+5);
	}
	for (i = 1; i<10; i++) {
		printf("%d 345\n",i*10+5);
	}
	for (i = 1; i<10; i++) {
		printf("%d 325\n",i*10+5);
	}

	for (i = 1; i<3; i++) {
		printf("%d 305\n",i*10+5);
	}

	for (i = 1; i<3; i++) {
		printf("%d 285\n",i*10+5);
	}
	
	printf("15 265\n");
	printf("15 245\n");
	printf("15 205\n");

	for (i = 1; i<6; i++) {
		printf("%d 225\n",i*10+5);
	}

	printf("15 185\n");
	printf("45 185\n");
	printf("15 165\n");


	// BAWAH LABTEK
	for (i = 11; i<26; i++) {
		printf("%d 230\n",i*10+5);
	}

	// ATAS LABTEK
	for (i = 11; i<26; i++) {
		printf("%d 155\n",i*10+5);
	}	

	// KANAN BAWAH
	for (i = 28; i<36; i++) {
		printf("%d 405\n",i*10+5);
	}

	for (i = 22; i<36; i++) {
		printf("%d 385\n",i*10+5);
	}

	for (i = 22; i<36; i++) {
		printf("%d 365\n",i*10+5);
	}	

	for (i = 30; i<36; i++) {
		printf("%d 345\n",i*10+5);
	}

	for (i = 32; i<36; i++) {
		printf("%d 325\n",i*10+5);
	}


	
	return 0;
}