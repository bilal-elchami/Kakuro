#include <iostream>
#include <fstream>
using namespace std;

class Grid {
  private:
	int numColumns;
	int numRows;
	int targetColValues[10];
	int targetRowValues[10];
	void substringValues(string line, string delimiter, int lineCount) {
		size_t pos = 0;
		while ((pos = line.find(delimiter)) && (line.length() > 0)) {
			int value = atoi(line.substr(0, pos).c_str());
			if (lineCount == 1) { // row
				targetRowValues[numRows] = value;
				numRows += 1;
			} else if (lineCount == 2) { // column
				targetColValues[numColumns] = value;
				numColumns += 1;
			}
			line.erase(0, pos + delimiter.length());
		}
	}

  public:
	void initialize(string path, string delimiter) {
		ifstream file(path);
		string line;
		numRows = 0;
		numColumns = 0;
		int lineCount = 0;
		while (getline(file, line)) {
			lineCount++;
			substringValues(line, delimiter, lineCount);
		}
	}

	void displayInfo() {
		cout << "Number of columns : " << getNumColumns() << endl;
		cout << "Number of rows : " << getNumRows() << endl;
		cout << "Col target sum : ";
		for (int c = 0; c < numColumns; c++) {
			cout << "col[" << (c + 1) << "] = " << targetColValues[c];
			if (c < (numColumns - 1)) {
				cout << " , ";
			}
		}
		cout << endl;
		cout << "Row target sum : ";
		for (int r = 0; r < numRows; r++) {
			cout << "row[" << (r + 1) << "] = " << targetRowValues[r];
			if (r < (numRows - 1)) {
				cout << " , ";
			}
		}
		cout << endl;
	}

	int getNumColumns() {
		return numColumns;
	}

	int getNumRows() {
		return numRows;
	}

	int getColTargetSum(int col) {
		return targetColValues[col - 1];
	}

	int getRowTargetSum(int row) {
		return targetRowValues[row - 1];
	}

};

class Kakuro {
  private:
	Grid grid;
	int possibleMinValue;
	int possibleMaxValue;
	int values[10][10]; // here we should set the values to --> values[grid.col][grid.rows]
	int getValue(int col, int row) {
		return values[col-1][row-1];
	}
	void setValue(int col, int row, int value) {
		values[col-1][row-1] = value;
	}

  public:
	void initialize(string path, string delimiter, int minValue, int maxValue) {
		grid.initialize(path, delimiter);
		possibleMinValue = minValue;
		possibleMaxValue = maxValue;
		for (int row = 1; row <= grid.getNumRows(); row++) {
			for (int col = 1; col <= grid.getNumColumns(); col++) {
				setValue(col, row, possibleMinValue);
			}
		}
	}

	void showGameInfo() {
		grid.displayInfo();
		cout << "Min possible value : " << possibleMinValue << endl;
		cout << "Max possible value : " << possibleMaxValue << endl;
	}

	void show() {
		for (int row = 0; row <= grid.getNumRows(); row++) {
			for (int col = 0; col <= grid.getNumColumns(); col++) {
				string value = "";
				if ((col == 0) && (row == 0)) {
					value = " ";
				} else if (col == 0) {
					value = to_string(grid.getRowTargetSum(row));
				} else if (row == 0) {
					value = to_string(grid.getColTargetSum(col));
				} else {
					value = to_string(getValue(col, row));
				}
				cout << value << " | ";
			}
			cout << endl;
			if (row == 0) {
				for (int c = 0; c <= grid.getNumColumns(); c++) {
					cout << "----";
				}
				cout << endl;
			}
		}
	}

	void solve() {
		cout << endl;
		cout << " ------- SOLVING -------" << endl;
		cout << endl;
	}
};

int main() {
	Kakuro kakuro;
	kakuro.initialize("C:\\Users\\eliea\\Desktop\\grid.txt", ";", 0, 10);
	kakuro.showGameInfo();
	kakuro.show();
	kakuro.solve();
	kakuro.show();
}
