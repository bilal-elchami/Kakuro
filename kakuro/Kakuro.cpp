#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
using namespace std;

class Cell {
	private:
		int row;
		int col;

	public: 
		void init(int col_value, int row_value) {
			col = col_value;
			row = row_value;
		}

		int get_col() {
			return col;
		}

		int get_row() {
			return row;
		}
};

class Grid {
  private:
	int num_columns;
	int num_rows;
	int target_col_sum[10];
	int target_row_sum[10];
	void substring_values(string line, string delimiter, int line_count) {
		size_t pos = 0;
		while ((pos = line.find(delimiter)) && (line.length() > 0)) {
			int value = atoi(line.substr(0, pos).c_str());
			if (line_count == 1) { // row
				  target_row_sum[num_rows] = value;
				  num_rows += 1;
			  } else if (line_count == 2) { // column
				target_col_sum[num_columns] = value;
				num_columns += 1;
			}
			line.erase(0, pos + delimiter.length());
		}
	}
  
  public:
	void initialize(string path, string delimiter) {
		ifstream file(path);
		string line;
		num_rows = 0;
		num_columns = 0;
		int line_count = 0;
		while (getline(file, line)) {
			line_count++;
			substring_values(line, delimiter, line_count);
		}
	}

	void display_info() {
		cout << "Number of columns : " << num_columns << endl;
		cout << "Number of rows : " << num_rows << endl;
		cout << "Col target sum : ";
		for (int c = 0; c < num_columns; c++) {
			cout << "col[" << (c + 1) << "] = " << target_col_sum[c];
			if (c < (num_columns - 1)) {
				cout << " , ";
			}
		}
		cout << endl;
		cout << "Row target sum : ";
		for (int r = 0; r < num_rows; r++) {
			cout << "row[" << (r + 1) << "] = " << target_row_sum[r];
			if (r < (num_rows - 1)) {
				cout << " , ";
			}
		}
		cout << endl;
	}

	int get_num_columns() {
		return num_columns;
	}

	int get_num_rows() {
		return num_rows;
	}

	int get_col_target_sum(int col) {
		return target_col_sum[col - 1];
	}

	int get_row_target_sum(int row) {
		return target_row_sum[row - 1];
	}

};

class Kakuro {
  private:
	Grid grid;
	int init_value = -1;
	int possible_min_value = 0;
	int possible_max_value = 0;
	int values[10][10];

	int get_value(int col, int row) {
		return values[col-1][row-1];
	}
	
	void set_value(int col, int row, int value) {
		values[col-1][row-1] = value;
	}
	
	int get_sum_row(int row) {
		int sum = 0;
		for (int col = 0; col < grid.get_num_columns(); col++) {
			sum += values[col][row - 1];
		}
		return sum;
	}
	
	int get_sum_col(int col) {
		int sum = 0;
		for (int row = 0; row < grid.get_num_rows(); row++) {
			sum += values[col-1][row];
		}
		return sum;
	}

	int get_random_value(int min_value, int max_value) {
		return rand() % (max_value - min_value) + min_value;
	}

	bool is_finish_game() {
		for (int row = 1; row <= grid.get_num_rows(); row++) {
			if (get_sum_row(row) != grid.get_row_target_sum(row)) {
				return false;
			}
		}
		for (int col = 1; col <= grid.get_num_columns(); col++) {
			if (get_sum_col(col) != grid.get_col_target_sum(col)) {
				return false;
			}
		}
		return true;
	}

	string format_value(int value) {
		string result = to_string(value);
		if (result.length() == 1) {
			result = "0" + result;
		}
		return result;
	}

  public:
	void initialize(string path, string delimiter, int minValue, int maxValue) {
		srand(time(NULL));
		grid.initialize(path, delimiter);
		possible_min_value = minValue;
		possible_max_value = maxValue;
		for (int row = 1; row <= grid.get_num_rows(); row++) {
			for (int col = 1; col <= grid.get_num_columns(); col++) {
				set_value(col, row, init_value);
			}
		}
	}

	void show_game_info() {
		grid.display_info();
		cout << "Min possible value : " << possible_min_value << endl;
		cout << "Max possible value : " << possible_max_value << endl;
	}

	void show() {
		for (int row = 0; row <= grid.get_num_rows(); row++) {
			for (int col = 0; col <= grid.get_num_columns(); col++) {
				string value = "";
				if ((col == 0) && (row == 0)) {
					value = "  ";
				} else if (col == 0) {
					value = format_value(grid.get_row_target_sum(row));
				} else if (row == 0) {
					value = format_value(grid.get_col_target_sum(col));
				} else {
					value = format_value(get_value(col, row));
				}
				cout << value << "  |  ";
			}
			cout << endl;
			if (row == 0) {
				for (int c = 0; c <= grid.get_num_columns(); c++) {
					cout << "-------";
				}
				cout << endl;
			}
		}
	}

	Cell get_unassigned_cell() {
		for (int row = 0; row < grid.get_num_rows(); row++) {
			for (int col = 0; col < grid.get_num_columns(); col++) {
				if (values[col][row] == init_value) {
					Cell unassigned_cell;
					unassigned_cell.init(col, row);
					return unassigned_cell;
				}
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
	kakuro.show_game_info();
	kakuro.show();
	kakuro.solve();
	kakuro.show();
}
