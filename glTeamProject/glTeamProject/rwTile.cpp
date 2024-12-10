#include "rwTile.h"


void FileToArray(const char* filename, int** maptile, int* row, int* column) {



}

void SaveTile(const char* filename, int** maptile) {

}

int** InitTileArr(int** maptile, int row, int column) {
	maptile = (int**)malloc(row * sizeof(int*));


	for (int i = 0; i < row; i++) {
		maptile[i] = (int*)malloc(column * sizeof(int));
		for (int j = 0; j < column; j++) {
			maptile[i][j] = 0;
		}
	}

	return maptile;
}

void UpdateTile(int** maptile, int y, int x, int value) {
	maptile[y][x] = value;
}