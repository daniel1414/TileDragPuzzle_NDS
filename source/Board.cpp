#include "Board.hpp"
#include <random>
#include <algorithm>
// ------------------------------------ private mehods -----------------------//

void Board::init(u8* gfx, u16* pal, int m_width, int m_height, int t_width, SpriteSize t_size) {
	gfx_ptr_ = gfx;
	pal_ptr_ = pal;
	m_width_ = m_width;
	m_height_ = m_height;
	t_width_ = t_width;
	h_off_ = (SCREEN_WIDTH - m_width_ * t_width_) / 2; // calculate the horizontal and vertical boad offset from screen edge
	v_off_ = (SCREEN_HEIGHT - m_height_ * t_width_) / 2;
	exit_ = false;
	// fill the tiles vector
	for (int i = 0; i < m_width_ * m_height_; i++) { // 0 - 5
		int tile_x;
		int tile_y;
		locIntoPos(i, tile_x, tile_y);
		v_.push_back(std::make_unique<Tile>(i, tile_x, tile_y, t_size, t_width_, gfx_ptr_ + (i * t_width_ * t_width_), false));
	}
	mixTiles();
	v_.back()->hide(); // hide the last element
	free_loc_ = v_.back()->getCurLoc(); // position of the black square

	pos_[0] = h_off_; // x1
	pos_[1] = SCREEN_WIDTH - h_off_; // x2
	pos_[2] = v_off_; // y1
	pos_[3] = SCREEN_HEIGHT - v_off_; // y2

	dmaCopy(pal, SPRITE_PALETTE, 512);
}

void Board::moveTile(int tile_id, int dest_x, int dest_y) {
	v_[tile_id]->move(dest_x, dest_y);
	v_[tile_id]->setCurLoc(getTouchLocation(dest_x + t_width_ / 2, dest_y + t_width_ / 2));
}

void Board::victory() {
	v_[m_width_ * m_height_ - 1]->show();
	v_[m_width_ * m_height_ - 1]->update();
}

void Board::mixTiles() {
	std::vector<int>choosen_loc;
	choosen_loc.reserve(v_.size());
	std::random_device rd;
	std::mt19937 mt(rd());
	std::uniform_int_distribution<int> dist(0, v_.size() - 1);
	for (unsigned int i = 0; i < v_.size(); i++) {
		int loc = dist(mt);
		while (std::find(choosen_loc.begin(), choosen_loc.end(), loc) != choosen_loc.end()) {
			loc = dist(mt);
		}
		choosen_loc.push_back(loc);
		moveTile(i, locIntoX(loc), locIntoY(loc));
	}
}

void Board::hide() {
	for (auto & t : v_) {
		t->hide();
	}
	update();
}
// -------------------------------- public methods ------------------------------//

Board::Board(const BoardImage& image) {
	init(image.tile_gfx, image.pal, image.m_width, image.m_height, image.t_width, image.sprite_size);
}

int Board::getTouchLocation(int px, int py) const { // converts pixels into tile indexes
	int new_x = px - h_off_;
	int new_y = py - v_off_;
	new_x = floor(new_x / t_width_);
	new_y = floor(new_y / t_width_);
	return new_y * m_width_ + new_x;
}

int Board::getTileIndexInLocation(int loc) const {
	for (auto& t : v_) {
		if (t->getCurLoc() == loc)
			return t->getId();
	}
	return -1;
}

void Board::touchAction(touchPosition& touch) {
	// check, if the touch is within the board area
	if (touch.px > pos_[0] && touch.px < pos_[1] &&
		touch.py > pos_[2] && touch.py < pos_[3])
	{	
		while (touch.px || touch.py) { // do things while the stick touches the screen and the stick touches tiles next to the empty space
			if (canMove(touch.px, touch.py)) { // tapped a viable option
				bool holding = true;
				int touch_loc_bef = getTouchLocation(touch.px, touch.py);
				int cur_holding_tile_index = getTileIndexInLocation(touch_loc_bef);
				while (holding) {
					if (touch.px || touch.py) { // still holding the tile
						int x1, x2, y1, y2;		// limits 
						if (free_loc_ == touch_loc_bef + m_width_) { // the free loc is beneath
							x1 = locIntoX(touch_loc_bef);
							x2 = x1;
							y1 = locIntoY(touch_loc_bef);
							y2 = locIntoY(free_loc_);
						}
						else if (free_loc_ == touch_loc_bef - m_width_) { // the free loc is above
							x1 = locIntoX(touch_loc_bef);
							x2 = x1;
							y1 = locIntoY(free_loc_);
							y2 = locIntoY(touch_loc_bef);
						}
						else if (free_loc_ == touch_loc_bef + 1) { // the free loc is on the right
							x1 = locIntoX(touch_loc_bef);
							x2 = locIntoX(free_loc_);
							y1 = locIntoY(touch_loc_bef);
							y2 = y1;
						}
						else { // te free loc is on the left
							x1 = locIntoX(free_loc_);
							x2 = locIntoX(touch_loc_bef);
							y1 = locIntoY(touch_loc_bef);
							y2 = y1;
						}
						int new_x = touch.px - t_width_ / 2;
						int new_y = touch.py - t_width_ / 2;
						if (new_x < x1)
							new_x = x1;
						if (new_x > x2)
							new_x = x2;
						if (new_y < y1)
							new_y = y1;
						if (new_y > y2)
							new_y = y2;
						moveTile(cur_holding_tile_index, new_x, new_y);
					}
					else { // dropped the tile 
						int x, y;
						int cur_hol_tile_loc = v_[cur_holding_tile_index]->getCurLoc();
						locIntoPos(cur_hol_tile_loc, x, y);
						moveTile(cur_holding_tile_index, x, y);
						holding = false;
						if (cur_hol_tile_loc != touch_loc_bef) {
							free_loc_ = touch_loc_bef;
							locIntoPos(free_loc_, x, y);
							moveTile(v_.size() - 1, x, y); // move the hidden tile
							// check for victory
							if (victoryCheck()) {
								victory();
							}
						}
					}
					swiWaitForVBlank();
					oamUpdate(&oamMain);
					touchRead(&touch);
				}
			}
			touchRead(&touch);
		}
	}
}
void Board::keyAction(int keys) {
	int new_x, new_y;
	locIntoPos(free_loc_, new_x, new_y);
	if (keys & KEY_UP) {
		if ((free_loc_ / m_width_) < m_height_ - 1) { // the free loction is not on bottom egde
			moveTile(getTileIndexInLocation(free_loc_ + m_width_), new_x, new_y); // move the tile up
			locIntoPos(free_loc_ + m_width_, new_x, new_y);
			moveTile(v_.size() - 1, new_x, new_y);
			free_loc_ = free_loc_ + m_width_;
		
		}
	}
	else if (keys & KEY_RIGHT) {
		if ((free_loc_ % m_width_ ) > 0) { // x has to be greater than 0
			moveTile(getTileIndexInLocation(free_loc_ - 1), new_x, new_y);
			locIntoPos(free_loc_ - 1, new_x, new_y);
			moveTile(v_.size() - 1, new_x, new_y);
			free_loc_ = free_loc_ - 1;
		}
	}
	else if (keys & KEY_DOWN) { // free location cannot be on top edge
		if ((free_loc_ / m_width_) > 0) { // y has to be greater than 0
			moveTile(getTileIndexInLocation(free_loc_ - m_width_), new_x, new_y); // move tile from above down
			locIntoPos(free_loc_ - m_width_, new_x, new_y);
			moveTile(v_.size() - 1, new_x, new_y);
			free_loc_ = free_loc_ - m_width_;
		}
	}
	else if (keys & KEY_LEFT) {
		if ((free_loc_ % m_width_ ) < m_width_ - 1) { // the free location is not on the right egde
			moveTile(getTileIndexInLocation(free_loc_ + 1), new_x, new_y);
			locIntoPos(free_loc_ + 1, new_x, new_y);
			moveTile(v_.size() - 1, new_x, new_y);
			free_loc_ = free_loc_ + 1;
		}
	}
	if (victoryCheck()) {
		victory();
	}
	if (keys & KEY_B) {
		exit_ = true;
		hide();
		clear();
	}
}
void Board::locIntoPos(int loc, int& new_x, int& new_y) {
	new_x = locIntoX(loc);
	new_y = locIntoY(loc);
}

int Board::locIntoX(int loc) const {
	return (loc % m_width_) * t_width_ + h_off_;
}

int Board::locIntoY(int loc) const {
	return floor(loc / m_width_) * t_width_ + v_off_;
}

bool Board::canMove(int px, int py) const {
	int loc = getTouchLocation(px, py);
	for (unsigned int t = 0; t < v_.size(); t++) {
		if (loc == v_[t]->getCurLoc()) { // found the tile
			// check if it has the empty neighbour
			int loc_x = loc % m_width_;
			int loc_y = floor(loc / m_width_);
			if (loc_x == 0) { // left egde
				if (loc_y == 0) { // left top corner
					if (free_loc_ == 1 || free_loc_ == m_width_)
						return true;
					else
						return false;
				}
				else if (loc_y == m_height_ - 1) { // left bottom corner
					if (free_loc_ == loc + 1 || free_loc_ == loc - m_width_)
						return true;
					else
						return false;
				}
				else { // left edge without corners
					if (free_loc_ == loc + 1 || free_loc_ == loc + m_width_ || free_loc_ == loc - m_width_)
						return true;
					else
						return false;
				}
			}
			else if (loc_x == m_width_ - 1) { // right edge
				if (loc_y == 0) { // right top corner
					if (free_loc_ == loc - 1 || free_loc_ == loc + m_width_)
						return true;
					else
						return false;
				}
				else if (loc_y == m_height_ - 1) { // right bottom corner
					if (free_loc_ == loc - 1 || free_loc_ == loc - m_width_)
						return true;
					else
						return false;
				}
				else { // right edge without corners
					if (free_loc_ == loc - 1 || free_loc_ == loc - m_width_ || free_loc_ == loc + m_width_)
						return true;
					else
						return false;
				}
			}
			else if (loc_y == 0) { // top edge
				if (free_loc_ == loc - 1 || free_loc_ == loc + 1 || free_loc_ == loc + m_width_)
					return true;
				else
					return false;
			}
			else if (loc_y == m_height_ - 1) { // bottom edge
				if (free_loc_ == loc - 1 || free_loc_ == loc + 1 || free_loc_ == loc - m_width_)
					return true;
				else
					return false;
			}
			else { // the loc is somewhere inside
				if (free_loc_ == loc - 1 || free_loc_ == loc + 1 || free_loc_ == loc + m_width_ || free_loc_ == loc - m_width_)
					return true;
				else
					return false;
			}
		}
	}
	return false;
}

bool Board::victoryCheck() {
	for (auto& t : v_) {
		if (t->getId() != t->getCurLoc())
			return false;
	}
	exit_ = true;
	return true;
}

void Board::update() {
	for (auto& t : v_) {
		t->update();
	}
}

void Board::print() const {
	consoleClear();
	for (int r = 0; r < m_height_; r++) {
		for (int c = 0; c < m_width_; c++) {
			iprintf("%d %d    ", v_[r*m_width_ + c]->getId(), v_[r*m_width_ + c]->getCurLoc());
		}
		iprintf("\n");
	}
}

bool Board::exit() {
	if (exit_) {
		exit_ = false;
		return true;
	}
	else
		return false;
}