#pragma once

class Grid {
	public:
		Grid();
		~Grid();
	private:
		int lines;
		int columns;
		int possibleMinValue;
		int possibleMaxValue;
};