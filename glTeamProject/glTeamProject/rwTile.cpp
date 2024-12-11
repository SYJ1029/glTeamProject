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

void SetTile(int** maptile, int& tilerow, int& tilecolumn) {
	int i = -1;
	int j = -1;

	while (true) {
		i = (int)((float)rand() / RAND_MAX * (tilerow - 1));
		j = (int)((float)rand() / RAND_MAX * (tilecolumn - 1));

		if (maptile[i][j] <= 0) {
			maptile[i][j] = 1;

			break;
		}
	}

}

void ShiftTileMatrix(int** maptile, int direct, int& row, int& column) {
	int* token = (int*)malloc(column * sizeof(int));

	if (direct > 0) {
		for (int i = 0; i < column; i++) {
			token[i] = maptile[row - 1][i];
		}

		for (int i = row - 1; i > 0; i--) {
			for (int j = 0; j < column; j++) {
				maptile[i][j] = maptile[i - 1][j];
			}
		}

		for (int j = 0; j < column; j++) {
			maptile[0][j] = token[j];
		}
	}
	else if (direct < 0) {
		for (int i = 0; i < column; i++) {
			token[i] = maptile[0][i];
		}

		for (int i = 0; i < row - 1; i++) {
			for (int j = 0; j < column; j++) {
				maptile[i][j] = maptile[i + 1][j];
			}
		}

		for (int j = 0; j < column; j++) {
			maptile[row - 1][j] = token[j];
		}
	}

	free(token);
}