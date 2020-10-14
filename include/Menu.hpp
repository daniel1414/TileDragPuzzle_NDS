#pragma once
#include "Board.hpp"
#include <vector>
#include <string>

enum class MenuState { CHOOSE_DIFFICULTY, CHOOSE_BOARD };

class Menu {
	bool exit_menu;
	int level;
	int which_board;
	MenuState menu_state;
	int offset;
	std::vector<BoardImage>::const_iterator board;
public:
	Menu() : exit_menu(false), level(0), which_board(-1), menu_state(MenuState::CHOOSE_DIFFICULTY) {};
	void mainMenu(const std::vector<std::pair<std::string, int>>& diff_count, const std::vector<BoardImage>& images, int keys);
	void printDifficulties(const std::vector<std::pair<std::string, int>>& diff_count);
	void printAvailableBoards(std::vector<BoardImage>::const_iterator& it, int count, int start_line);
	void printCursor(int height);
	void print(const std::vector<std::pair<std::string, int>>& diff_count, std::vector<BoardImage>::const_iterator& start_point);
	bool exit() {
		if (exit_menu) {
			exit_menu = false;
			return true;
		}
		else
			return false;
	}
	void setState(MenuState state) { menu_state = state; }
	void setBoard(int b) { which_board = b; }
	std::vector<BoardImage>::const_iterator getBoard() const {return board;}
};