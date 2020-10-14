#pragma once
#include <nds.h>
#include <stdio.h>
#include <nds/arm9/sprite.h>
enum class Direction{UP, RIGHT, DOWN, LEFT};

class Tile {
	int id_;
	int x_;
	int y_;
	SpriteSize size_;
	int width_;
	bool hidden_;
	int cur_loc_;
	u16* gfx_;
public:
	Tile() = default;
	Tile(int id, int x, int y, SpriteSize size, int width, u8* tile_gfx, bool hidden);
	void move(int desX, int desY);
	void hide() { hidden_ = true; }
	void show() { hidden_ = false; }
	int getCurLoc() const { return cur_loc_; }
	void setCurLoc(int loc) { cur_loc_ = loc; }
	int getId() const { return id_; }
	void update();
	~Tile();
};