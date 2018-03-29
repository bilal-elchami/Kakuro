#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string>
#include <stack>

using namespace std;

#define MAX 100
#define INIT_VALUE -1

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
				result = false; // inconsistent
			}
		}
		return result;
	}

	Position get_unassigned_cell_position() {
		int unassigned_col = -1;
		int unassigned_row = -1;
		int min_possible_value_size = -1;
		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				if (cells[col][row]->get_value() == INIT_VALUE) {
					int possible_value_size = cells[col][row]->get_domain_size();
					if ((min_possible_value_size == -1) || (possible_value_size < min_possible_value_size)) {
						unassigned_col = col;
						unassigned_row = row;
						min_possible_value_size = possible_value_size;
					}
				}
			}
		}
		return Position(unassigned_col, unassigned_row);
	}

	int get_unassigned_cell_count() {
		int count = 0;
		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				if (cells[col][row]->get_value() == INIT_VALUE) {
					count++;
				}
			}
		}
		return count;
	}

	int * get_possible_values(int col, int row) {
		return cells[col][row]->get_domain();
	}

	int get_possible_values_size(int col, int row) {
		return cells[col][row]->get_domain_size();
	}

	bool check_no_domain_empty() {
		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				if (cells[col][row]->get_value() == INIT_VALUE) {
					// cell not assigned
					if (cells[col][row]->get_domain_size() ==  0) {
						// don't have value in domain
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
			for (int i = 0; i < get_possible_values_size(col, r); i++) {
				cout << possible_values[i] << ", ";
			}
			cout << endl;
		}
		cout << endl;
		for (int row = 0; row < num_rows; row++) {
			cout << "Cell [" << (c+1) << "," << (row+1) << "] : ";
			int *possible_values = get_possible_values(c, row);
			for (int i = 0; i < get_possible_values_size(c, row); i++) {
				cout << possible_values[i] << ", ";
			}
			cout << endl;
		}
	}

	void show_domain() {
		for (int col = 0; col < get_num_columns(); col++) {
			for (int row = 0; row < get_num_rows(); row++) {
				cout << "Cell [" << col << "," << row << "]: ";
				int *possible_values = get_possible_values(col, row);
				for (int i = 0; i < get_possible_values_size(col, row); i++)
				{
					cout << possible_values[i] << ", ";
				}
				cout << endl;
			}
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

	int get_number_free_variable() {
		int sum = 0;
		for (int col = 0; col < num_columns; col++) {
			for (int row = 0; row < num_rows; row++) {
				if (cells[col][row]->get_value() == INIT_VALUE) {
					sum++;
				}
			}
		}
		return sum;
	}

	int get_row_sum(int row) {
		int result = 0;
		for (int col = 0; col < num_columns; col++) {
			result += cells[col][row]->get_value();
		}
		return result;
	}
	
	int get_col_sum(int col) {
		int result = 0;
		for (int row = 0; row < num_rows; row++) {
			result += cells[col][row]->get_value();
		}
		return result;
	}

	void reset() {
		for (int col = 0; col < get_num_columns(); col++) {
			for (int row = 0; row < get_num_rows(); row++) {
				unset_value(col, row);
			}
		}
		recalculate_domains();
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

		bool is_finished() {
			for (int col = 0; col < grid->get_num_columns(); col++) {
				if (grid->get_col_sum(col) != target_col_sum[col]) {
					return false;
				}
			}
			for (int row = 0; row < grid->get_num_rows(); row++) {
				if (grid->get_row_sum(row) != target_row_sum[row]) {
					return false;
				}
			}

			return (grid->get_number_free_variable() == 0);
		}

		int score_sample() {
			int size = 20;
			do {
				Position unassigned_cell_possition = grid->get_unassigned_cell_position();
				int col = unassigned_cell_possition.get_col();
				int row = unassigned_cell_possition.get_row();
				if ((col == -1) && (row == -1)) {
					return -1;
				}
				int * possible_values = grid->get_possible_values(col, row);
				int possible_values_count = grid->get_possible_values_size(col, row);

				int rnd_i = rand() % (possible_values_count + 1);
				rnd_i = rnd_i - 1;
				int random_value = possible_values[rnd_i];

				// grid->set_cell_value(col, row, random_value);

				// bool consistent = grid->update_domains_free_cell(col, row, random_value, target_col_sum[col], target_row_sum[row]);
				// bool no_domain_empty = grid->check_no_domain_empty();
				
				// if (!no_domain_empty || !consistent) {
					//return 1 + grid->get_unassigned_cell_count();
				//}
 
				if (grid->get_unassigned_cell_count() == 0) {
					return 0;
				}
				if (size > 20) {
					return 1 + grid->get_unassigned_cell_count();
				}
				size++; 

			} while (true);
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
			srand(time(NULL));
		}

		void show() {
			for (int col = -1; col < grid->get_num_columns(); col++) {
				for (int row = -1; row < grid->get_num_rows(); row++) {
					if ((col == -1) && (row == -1)) {
						cout << "  ";
					} else if (row == -1) {
						cout << target_col_sum[col]; 
					} else if (col == -1) {
						cout << target_row_sum[row];
					} else {
						cout << format_value(grid->get_cell_value(col, row)) << " ";
					}
					cout << "\t";
				}
				cout << endl;
			}
		}

		int sample() {
			do {
				Position unassigned_cell_possition = grid->get_unassigned_cell_position();
				int col = unassigned_cell_possition.get_col();
				int row = unassigned_cell_possition.get_row();
				if ((col == -1) && (row == -1)) {
					return -1;
				}
				int * possible_values = grid->get_possible_values(col, row);
				int possible_values_count = grid->get_possible_values_size(col, row);

				int rnd_i = rand() % (possible_values_count + 1);
				rnd_i = rnd_i - 1;
				int random_value = possible_values[rnd_i];
				
				grid->set_cell_value(col, row, random_value);
				bool consistent = grid->update_domains_free_cell(col, row, random_value, target_col_sum[col], target_row_sum[row]);
				bool no_domain_empty = grid->check_no_domain_empty();

				if (!no_domain_empty || !consistent) {
					return 1 + grid->get_unassigned_cell_count();
				}
 
				if (grid->get_unassigned_cell_count() == 0) {
					return 0;
				}

			} while (true);
		}
		
		bool iterative_sampling() {
			int tries = 10000000;
			do {
				if (sample() == 0) {
					return true;
				}
				tries--;
				grid->reset();
			} while (tries > 0);
			return false;
		}

		int meta_monte_carlo() {
			int best_score = grid->get_unassigned_cell_count();
			do {
				Position unassigned_cell_possition = grid->get_unassigned_cell_position();
				int col = unassigned_cell_possition.get_col();
				int row = unassigned_cell_possition.get_row();
				if ((col == -1) && (row == -1)) {
					return -1;
				}				
				int * possible_values = grid->get_possible_values(col, row);
				int possible_values_count = grid->get_possible_values_size(col, row);
				int possible_values_copy[MAX];
				for (int j = 0; j < possible_values_count; j++) {
					possible_values_copy[j] = possible_values[j];
				}
				int best_value = -1;
				for (int i = 0; i < possible_values_count; i++) {
					int value = possible_values_copy[i];
					if (best_value == -1) {
						best_value = value;
					}
					grid->set_cell_value(col, row, value);
					bool consistent = grid->update_domains_free_cell(col, row, value, target_col_sum[col], target_row_sum[row]);
					bool no_domain_empty = grid->check_no_domain_empty();
					int score = -1;
					if (!no_domain_empty || !consistent) {
						score = 1 + grid->get_unassigned_cell_count();
					} else {
						score = score_sample();
					}
					if (score < best_score) {
						best_score = score;
						best_value = value;
					}
					grid->unset_value(col, row);
					grid->recalculate_domains();
				}
				
				grid->set_cell_value(col, row, best_value);
				bool consistent = grid->update_domains_free_cell(col, row, best_value, target_col_sum[col], target_row_sum[row]);
				bool no_domain_empty = grid->check_no_domain_empty();
				if (!no_domain_empty || !consistent) {
					return 1 + grid->get_unassigned_cell_count();
				}
				
				if (((grid->get_unassigned_cell_count() == 0) || (best_score == 0)) && consistent){
					return 0;
				}
				
			} while (true);		
		}

		bool iterative_meta_monte_carlo() {
			int tries = 1000000000;
			do {
				if (meta_monte_carlo() == 0) {
					return true;
				}
				tries--;
				grid->reset();
			} while (tries > 0);
			return false;
		}
		
		bool forward_checking() {
			Position unassigned_cell_possition = grid->get_unassigned_cell_position();
			int col = unassigned_cell_possition.get_col();
			int row = unassigned_cell_possition.get_row();
			if ((col == -1) && (row == -1)) {
				return true;
			}
			int * possible_values = grid->get_possible_values(col, row);
			int possible_values_count = grid->get_possible_values_size(col, row);
			int possible_values_copy[MAX];
			for (int j = 0; j < possible_values_count; j++) {
				possible_values_copy[j] = possible_values[j];
			}
			for (int i = 0; i < possible_values_count; i++) {
				grid->set_cell_value(col, row, possible_values_copy[i]);
				bool consistent = grid->update_domains_free_cell(col, row, possible_values_copy[i], target_col_sum[col], target_row_sum[row]);
				bool no_domain_empty = grid->check_no_domain_empty();
				if (no_domain_empty && consistent) {
					if (forward_checking()) {
						return true;
					}
				} else {
					grid->unset_value(col, row);
				}
			}
			grid->unset_value(col, row);
			grid->recalculate_domains();
			return false;
		}

		void solve(int algorithm) {
			cout << "Initial grid : " << endl;
			show();

			clock_t start;
			start = clock();

			bool algo = false;
			string algo_str = "";
			if (algorithm == 1) {
				algo = forward_checking();
				algo_str = "forward checking";
			} else if(algorithm == 2) {
				algo_str = "iterative sampling";
				algo = iterative_sampling();
			} else if (algorithm == 3) {
				algo_str = "iterative meta monte carlo";
				algo = iterative_meta_monte_carlo();
			} else if (algorithm == 4) {
				solve(1);
				grid->reset();
				solve(2);
				grid->reset();
				solve(3);
			}

			if (algorithm < 4) {
				cout << endl << "-- Solving using " << algo_str << "'s algorithm --" << endl << endl;
				if (algo) {
					cout << "Solution found" << endl;
					show();
				} else {
					cout << "Solution not found" << endl;
				}

				double duration = (clock() - start) / (double)CLOCKS_PER_SEC;
				cout << "Time elapsed " << to_string(duration) << " s" << endl;
				cout << endl << endl << endl << endl << endl << endl << endl;
			}

		}

};

void run_experiment(string grid, int _iterations, int algo) {
	int iterations = _iterations;
	string algo_str = "";
	clock_t start;
	start = clock();
	if (algo == 1) {
		algo_str = "forward checking";
	}
	if (algo == 2) {
		algo_str = "iterative sampling";
	}
	if (algo == 3) {
		algo_str = "iterative meta monte carlo";
	}
	cout << "Solving grid [" << grid << "] " << iterations << " times in " << algo_str << " algorithm " << endl;

	while (iterations > 0) {
		Kakuro kakuro("grids/" + grid + ".txt", ";");
		if (algo == 1) {
			kakuro.forward_checking();
		}
		if (algo == 2) {
			kakuro.iterative_sampling();
		}
		if (algo == 3) {
			kakuro.iterative_meta_monte_carlo();
		}
		cout << ".";
		iterations--;
	}
	double duration = (clock() - start) / (double) CLOCKS_PER_SEC;
	cout << "Time elapsed " << duration << " s" << endl << endl;
}

void experiments() {
	int iterations = 100;
	
	int algo_count = 3;	
	int algo[3] = {1, 2, 3};

	// string grids[8] = {"grid2x2", "grid3x3", "grid4x4", "grid5x5_1", "grid5x5_2", "grid5x5_3", "grid6x6_1", "grid6x6_2"};
	// string grids[5] = {"grid2x2", "grid3x3", "grid4x4", "grid5x5_1", "grid6x6_1"};
	string grids[4] = {"grid2x2", "grid3x3", "grid4x4", "grid5x5_1"};
	int grid_count = 4;

	for (int a = 0; a < algo_count; a++) {
		for (int i = 0; i < grid_count; i++) {
			run_experiment(grids[i], iterations, algo[a]);
		}
	}
}

int main() {
	experiments();

	while (true) {
		string path = "";
		cout << "Enter grid file name : ";
		cin >> path;
		path = "grids/" + path + ".txt";
		string delimiter;
		cout << "Enter delimiter : ";
		cin >> delimiter;
		int algo;
		cout << "Choose an algorithm between " << endl;
		cout << " * Forward Checking (1)" << endl;
		cout << " * Iterative sampling (2)" << endl;
		cout << " * Iterative meta monte carlo (3)" << endl;
		cout << " * All above (4)" << endl;
		cin >> algo;
		cout << endl;

		Kakuro kakuro(path, delimiter);
		kakuro.solve(algo);
		cout << endl << endl;
	}
}