#include <iostream>
#include <fstream>
#include "Grid.h"
using namespace std;

Grid::Grid(string path, string delimiter) {
    ifstream file(path);
    string line;
    int lineCount = 0;
    while (getline(file, line)) {
        lineCount++;
        substringValues(line, delimiter, lineCount);
    }
}

void Grid::displayInfo() {
    cout << " cols " << numColumns << endl;
    for (int i = 0; i < numColumns; i++) {
        cout << "col [" << (i + 1) << "] " << colValues[i] << endl;
    }
    cout << endl << "rows " << numRows << endl;
    for (int i = 0; i < numRows; i++) {
        cout << "row [" << (i + 1) << "] " << rowValues[i] << endl;
    }
}

void Grid::substringValues(string line, string delimiter, int lineCount) {
    size_t pos = 0;
    while ((pos = line.find(delimiter)) && (line.length() > 0)) {
        int value = atoi(line.substr(0, pos).c_str());
        if (lineCount == 1) { // row
            rowValues[numRows] = value;
            numRows += 1;
        } else if (lineCount == 2) { // column
            colValues[numColumns] = value;
            numColumns += 1;
        }
        line.erase(0, pos + delimiter.length());
    }
}