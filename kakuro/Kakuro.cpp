#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>

#define MAX 10
#define INIT_VALUE -1
using namespace std;

class Position {
	private:
		int row;
		int col;

	public:
		Position(int c, int r) {
			col = c;
			row = r;
		}

		int get_col() {
			return col;
		}

		int get_row() {
			return row;
		}
};

class Cell {
	private:
		Position position;
		int value;
		int initial_domain[MAX];
		int initial_domain_size;
		int domain[MAX]; 
		int domain_size;

	  public: 
		Cell(int col, int row, int v, int possible_values[MAX], int possible_values_size) : position(col, row) {
			value = v;
			domain_size = possible_values_size;
			initial_domain_size = domain_size;
			for (int i = 0; i < possible_values_size; i++) {
				domain[i] = possible_values[i];
				initial_domain[i] = domain[i];
			}
		}

		Position get_position() {
			return position;
		}

		int get_value() {
			return value;
		}

		void set_value(int v) {
			value = v;
		}

		void unset_value() {
			value = INIT_VALUE;
		}

		int * get_domain() {
			return domain;
		}

		int get_domain_size() {
			return domain_size;
		}

		void show_domain() {
			for (int i = 0; i < domain_size; i++) {
				cout << domain[i];
				if (i != (domain_size - 1)) {
					cout << ", ";
				}
			}
			cout << endl;
		}

		void remove_value_domain(int value) {
			int remove_i = -1;
			for (int i = 0; i < domain_size; i++) {
				if (domain[i] == value) {
					remove_i = i;
					break;
				}
			}
			if (remove_i != -1) {
				if (remove_i != (domain_size - 1)) {
					int x = remove_i;
					while(x < domain_size){
						domain[x] = domain[x + 1];
						x++;
					}
				}
				domain_size--;
			}
		}

		void remove_value_domain_greater(int value) {
			for (int i = 0; i < domain_size; i++) {
				if (domain[i] > value) {
					remove_value_domain(domain[i]);
					break;
				}
			}
		}

		void reset_to_initial_domain() {
			domain_size = initial_domain_size;
			for (int i = 0; i < initial_domain_size; i++) {
				domain[i] = initial_domain[i];
			}
		}

		bool is_assigned() {
			if (value != INIT_VALUE) {
				return true;
			}
			return false;
		}
};

class Grid {
  private:
	int num_columns;
	int num_rows;
	Cell *cells[MAX][MAX];

	void init_cells(int possible_values[MAX], int possible_values_size) {
		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				cells[col][row] = new Cell(col, row, INIT_VALUE, possible_values, possible_values_size);
			}
		}
	}
 
  public:
	Grid(int cols, int rows, int possible_values[MAX], int possible_values_size) {
		num_columns = cols;
		num_rows = rows;
		init_cells(possible_values, possible_values_size);
	}

	int get_num_columns() {
		return num_columns;
	}

	int get_num_rows() {
		return num_rows;
	}

	void show() {
		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				cout << cells[col][row]->get_value() << " ";
			}
			cout << endl;
		}
	}

	int get_cell_value(int col, int row) {
		return cells[col][row]->get_value();
	}

	void set_cell_value(int col, int row, int value) {
		cells[col][row]->set_value(value);
	}

	bool update_domains_free_cell(int col, int row, int value, int target_col_sum, int target_row_sum) {
		
		// Moreover, the sum Srow of all the assigned variables in a row is computed // OK
		// then the maximum possible value M axval for any variable in the row is computed by subtracting Srow to the goal sum of the row.// OK 
		// All value that are greater than M axval are removed from the domains of the free variables of the row.
		// If all the variables of the row have been assigned the sum is compared to the target sum and if it is different, the assignment is declared inconsistent.

		bool result = true;
		int sum_row = 0;	
		
		for (int c = 0; c < num_columns; c++) {
			cells[c][row]->remove_value_domain(value); // remove value from domain of on the same row
			if (cells[c][row]->get_value() != INIT_VALUE) {
				sum_row += cells[c][row]->get_value();
			}
		}

		int max_row_value = target_row_sum - sum_row;
		bool is_row_assigned = true;
		for (int c = 0; c < num_columns; c++) {
			if (cells[c][row]->get_value() == INIT_VALUE) {
				is_row_assigned = false;
				cells[c][row]->remove_value_domain_greater(max_row_value);
			}
		}

		if (is_row_assigned) {
			if ((max_row_value < 0) || (sum_row != target_row_sum)) {
				if (max_row_value < 0) {
					//cout << "max_row_value < 0 " << max_row_value << endl;
				}

				if (max_row_value != target_row_sum) {
					//cout << "inconsistent --> " << sum_row << " != " << target_row_sum << endl;
				}
				result = false; // inconsistent
			}
		}

		int sum_col = 0;
		for (int r = 0; r < num_rows; r++) {
			cells[col][r]->remove_value_domain(value); // remove value from domain of on the same col
			if (cells[col][r]->get_value() != INIT_VALUE) {
				sum_col += cells[col][r]->get_value();
			}
		}
		int max_col_value = target_col_sum - sum_col;
		bool all_col_assigned = true;
		for (int r = 0; r < num_rows; r++) {
			if (cells[col][r]->get_value() == INIT_VALUE) {
				all_col_assigned = false;
				cells[col][r]->remove_value_domain_greater(max_col_value);
			}
		}
		if (all_col_assigned) {
			if ((max_col_value < 0) || (sum_col != target_col_sum)) {
				if (max_col_value < 0) {
					//cout << "max_col_value < 0 " << max_col_value << endl;
				}

				if (sum_col != target_col_sum) {
					//cout << "inconsistent --> " << sum_col << " != " << target_col_sum << endl;
				}
				result = false; // inconsistent
			}
		}
		return result;
	}

	Position get_unassigned_cell_position() {
		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				if (cells[col][row]->get_value() == INIT_VALUE) {
					return Position(col, row);
				}
			}
		}
		return Position(-1,-1);
	}

	int * get_possible_values(int col, int row) {
		return cells[col][row]->get_domain();
	}

	int get_possible_values_size(int col, int row) {
		return cells[col][row]->get_domain_size();
	}

	bool check_no_domain_empty() {
		// check if no domain is empty
		// true if no domain is empty
		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				if (cells[col][row]->get_value() == INIT_VALUE) { // unassigned
					if (cells[col][row]->get_domain_size() ==  0) { // have value in domain
						return false;
					}
				}		
			}
		}
		return true;
	}

	void show_domains(int c, int r) {
		for (int col = 0; col < num_columns; col++) {
			cout << "Cell [" << (col+1) << "," << (r+1) << "] : ";
			int *possible_values = get_possible_values(col, r);
			for (int i = 0; i < get_possible_values_size(col, r); i++)
			{
				cout << possible_values[i] << ", ";
			}
			cout << endl;
		}
		cout << endl;
		for (int row = 0; row < num_rows; row++)
		{
			cout << "Cell [" << (c+1) << "," << (row+1) << "] : ";
			int *possible_values = get_possible_values(c, row);
			for (int i = 0; i < get_possible_values_size(c, row); i++)
			{
				cout << possible_values[i] << ", ";
			}
			cout << endl;
		}

	}

	void remove_value_from_domain(int c, int r, int value) {
		for (int col = 0; col < num_columns; col++) {
			cells[col][r]->remove_value_domain(value);
		}
		for (int row = 0; row < num_rows; row++) {
			cells[c][row]->remove_value_domain(value);
		}
	}

	void unset_value(int col, int row){
		cells[col][row]->unset_value();
	}

	void recalculate_domains() {
		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				cells[col][row]->reset_to_initial_domain();
			}
		}

		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				if (cells[col][row]->is_assigned()) {
					remove_value_from_domain(col, row, cells[col][row]->get_value());
				}
			}
		}
	}
};

class Kakuro {
	private:
		Grid* grid;
		int target_col_sum[MAX];
		int target_row_sum[MAX];
		int possible_values[MAX];
		
		int substring_target_values(string line, string delimiter, int line_count) {
			size_t pos = 0;
			int counter = 0;
			while ((pos = line.find(delimiter)) && (line.length() > 0)) {
				int value = atoi(line.substr(0, pos).c_str());
				if (line_count == 1) {
					target_col_sum[counter] = value;
				} else if (line_count == 2) {
					target_row_sum[counter] = value;
				} else if (line_count == 3) {
					possible_values[counter] = value;
				}
				counter++;
				line.erase(0, pos + delimiter.length());
			}
			return counter;
		}

		string format_value(int value) {
			string result = to_string(value);
			if (value == INIT_VALUE) {
				result = ".";
			}
			return result;
		}

	  public:
	  	Kakuro(string path, string delimiter) {
			ifstream file(path);
			string line;
			int line_count = 0;
			int num_columns = 0;
			int num_rows = 0;
			int possible_values_size = 0;
			while (getline(file, line)) {
				line_count++;
				if (line_count == 1) {
					num_columns = substring_target_values(line, delimiter, line_count);
				} else if (line_count == 2) {
					num_rows = substring_target_values(line, delimiter, line_count);
				} else if (line_count == 3) {
					possible_values_size = substring_target_values(line, delimiter, line_count);
				}
			}
			grid = new Grid(num_columns, num_rows, possible_values, possible_values_size);
		}

		void show() {
			for (int col = -1; col < grid->get_num_columns(); col++) {
				for (int row = -1; row < grid->get_num_rows(); row++) {
					if ((col == -1) && (row == -1)) {
						cout << "  ";
					} else if (row == -1) {
						cout << target_col_sum[col]; 
					} else if (col == -1) {
						cout << target_col_sum[row];
					} else {
						cout << format_value(grid->get_cell_value(col, row)) << " ";
					}
					cout << " ";
				}
				cout << endl;
			}
		}

		bool forward_checking() {
			Position unassigned_cell_possition = grid->get_unassigned_cell_position();
			int col = unassigned_cell_possition.get_col();
			int row = unassigned_cell_possition.get_row();
			if ((col == -1) && (row == -1)) {
				//cout << "no unassigned cell found" << endl;
				return true;
			}
			int * possible_values = grid->get_possible_values(col, row);
			int possible_values_count = grid->get_possible_values_size(col, row);
			int possible_values_copy[MAX];
			for (int j = 0; j < possible_values_count; j++) {
				possible_values_copy[j] = possible_values[j];
			}
			for (int i = 0; i < possible_values_count; i++) {
				//cout << endl << "SIZE " << grid->get_possible_values_size(col, row) << endl;
				//cout << "SETTING VALUE of CELL ["  << (col + 1) << "," << (row + 1) << "] = " << possible_values_copy[i] << endl;
				grid->set_cell_value(col, row, possible_values_copy[i]);
				bool consistent = grid->update_domains_free_cell(col, row, possible_values_copy[i], target_col_sum[col], target_row_sum[row]);
				bool no_domain_empty = grid->check_no_domain_empty();
				//show();
				//grid->show_domains(col, row);
				if (no_domain_empty && consistent) {
					if (forward_checking()) {
						return true;
					}
				} else {
					if (!no_domain_empty) {
						//cout << "There is a domain empty of the unassigned value" << endl;
					}
					if (possible_values_copy[i]==8) {
						//cout << "";
					}
					grid->unset_value(col, row);
					/*cout << "domains recalculated" << endl;
					cout << "New Domain: ";
					grid->show_domains(col, row);*/
					//return false;
				}
			}
			grid->unset_value(col, row);
			grid->recalculate_domains();
			return false;
		}
		void solve() {
			forward_checking();
		}
};

int main2() {
	Kakuro kakuro("..\\grid-2.txt", ";");
	cout << "-----------------INITIAL-------------" << endl;
	kakuro.show();
	cout << "-----------------SOLVING-------------" << endl;
	kakuro.solve();
	kakuro.show();
	return 0;
}
