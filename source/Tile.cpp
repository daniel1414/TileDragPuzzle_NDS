#include "Tile.hpp"

Tile::Tile(int id, int x, int y, SpriteSize size, int width, u8* tile_gfx, bool hidden) : id_(id), x_(x), y_(y), size_(size), width_(width), hidden_(hidden),
cur_loc_(id)
{
	gfx_ = oamAllocateGfx(&oamMain, size_, SpriteColorFormat_256Color);
	if (!gfx_)
		nocashWrite("could not allocate Tile", 512);
	oamSetGfx(&oamMain, id_, size_, SpriteColorFormat_256Color, gfx_);
	dmaCopy(tile_gfx, gfx_, width_ * width_);
	update();
}
void Tile::move(int desX, int desY) {
	x_ = desX;
	y_ = desY;
	update();
}

void Tile::update() {
	oamSet(&oamMain, id_, x_, y_, hidden_ ? 1 : 0, 0, size_, SpriteColorFormat_256Color, gfx_, -1, false, hidden_, false, false, false);
}
Tile::~Tile() {
	oamFreeGfx(&oamMain, gfx_);
}