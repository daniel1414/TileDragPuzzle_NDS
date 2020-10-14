#pragma once
#include <vector>
#include <tgmath.h>
#include <memory>
#include <string>

#include <nds.h>
#include <stdio.h>
#include <nds/arm9/sprite.h>
#include "Tile.hpp"

struct BoardImage {
	u8* tile_gfx;
	u16* pal;
	int m_width;
	int m_height;
	int t_width;
	SpriteSize sprite_size;
	std::string name;
};

class Board {
	u8* gfx_ptr_;
	u16* pal_ptr_;
	int m_width_;
	int m_height_;
	int t_width_;
	int pos_[4]; // x1, x2, y1, x2
	int free_loc_;
	int h_off_;
	int v_off_;
	std::vector<std::unique_ptr<Tile>> v_;
	bool exit_;

	void init(u8* gfx, u16* pal, int m_width, int m_height, int t_width, SpriteSize t_size);
	void clear() {v_.clear();}
	void moveTile(int tile_id, int dest_x, int dest_y);
	void victory();
	void mixTiles();
	void hide();
public:
	Board(u8* gfx, u16* pal, int m_width, int m_height, int t_width, SpriteSize t_size){
		init(gfx, pal, m_width, m_height, t_width, t_size);
	}
	Board(const BoardImage& image);

	void touchAction(touchPosition& touch);
	void keyAction(int keys);
	void locIntoPos(int loc, int& new_x, int& new_y);
	
	int locIntoX(int loc) const;
	int locIntoY(int loc) const;
	bool canMove(int px, int py) const;
	bool victoryCheck();
	void update();
	void print() const;
	bool exit();

	const int* getPos() const { return pos_; }
	int getTouchLocation(int px, int py) const;
	int getTileIndexInLocation(int loc) const;
};