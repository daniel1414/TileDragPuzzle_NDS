#include "Menu.hpp"


void Menu::mainMenu(const std::vector<std::pair<std::string, int>>& diff_count, const std::vector<BoardImage>& images, int keys) {

	// choosing a level
	if (menu_state == MenuState::CHOOSE_DIFFICULTY) {
		if (keys & KEY_UP) {
			level--;
			if (level < 0)
				level = diff_count.size() - 1;
		}
		if (keys & KEY_DOWN) {
			level++;
			if (level > (int)diff_count.size() - 1)
				level = 0;
		}
		if (keys & KEY_A) {
			if (diff_count[level].second) { // if there is no board available, you can't go there
				menu_state = MenuState::CHOOSE_BOARD;
				which_board = 0;
			}
		}
	}
	// choosing a board
	else if (menu_state == MenuState::CHOOSE_BOARD) {
		if (keys & KEY_UP) {
			which_board--;
			if (which_board < 0)
				which_board = diff_count[level].second - 1;
		}
		if (keys & KEY_DOWN) {
			which_board++;
			if (which_board > diff_count[level].second - 1)
				which_board = 0;
		}
		if (keys & KEY_B) {
			menu_state = MenuState::CHOOSE_DIFFICULTY;
			which_board = -1;
		}
		if (keys & KEY_A) {
			menu_state = MenuState::CHOOSE_DIFFICULTY;
			exit_menu = true;
		}
	}
	// foolproof
	offset = 0;
	// calculate how much boards there are before the given level
	for (int i = 0; i < level; i++) {
		offset += diff_count[i].second;
	}
	auto boards_to_print_startpoint = images.cbegin();
	boards_to_print_startpoint += offset;
	print(diff_count, boards_to_print_startpoint);
	
	board = images.cbegin() + offset + which_board;
}
void Menu::print(const std::vector<std::pair<std::string, int>>& diff_count, std::vector<BoardImage>::const_iterator& start_point) {
	consoleClear();
	printDifficulties(diff_count);
	printAvailableBoards(start_point, diff_count[level].second, diff_count.size()*2 + 4);
}
void Menu::printDifficulties(const std::vector<std::pair<std::string, int>>& diff_count) {
	// iprintfing
	u16 i = 0;
	for (const auto& d : diff_count) {
		iprintf(("\x1b[%d;0H" + d.first).c_str(), i * 2 + 2);
		i++;
	}
	printCursor(level * 2 + 2);
	iprintf("\x1b[%d;0H ------------------------------", i*2 + 2);
}
void Menu::printAvailableBoards(std::vector<BoardImage>::const_iterator& it, int count, int start_line) {
	for (int i = 0; i < count; i++) {
		iprintf(("\x1b[%d;0H   " + it[i].name).c_str(), i * 2 + start_line);
	}
	if (which_board != -1)
		printCursor(which_board * 2 + start_line);
}
void Menu::printCursor(int height) {
	iprintf("\x1b[%d;0H > ", height);
}