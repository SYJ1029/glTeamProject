#pragma once

#include <iostream>
#include <vector>



void FileToArray(const char* filename, int** maptile, int* row, int* column);
void SaveTile(const char* filename, int** maptile);
int** InitTileArr(int** maptile, int row, int column);
void UpdateTile(int** maptile, int y, int x, int value);
