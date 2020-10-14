#include <nds.h>
#include <stdio.h>
#include <nds/arm9/sprite.h>
#include <nds/arm9/console.h>
#include <nds/input.h>

#include <big_ben.h>
#include <bus.h>
#include <castle_garden.h>
#include <chess.h>

#include <fishNchips.h>
#include <london_eye.h>
#include <ship.h>
#include <tower_bridge.h>

#include <fisterra.h>
#include <rising_sun.h>
#include <setting_sun.h>
#include <small_beach.h>

#include <road.h>
#include <me.h>
#include <workout_park.h>
#include <giewont.h>

#include "Board.hpp"
#include "Menu.hpp"

enum class GameState{MAIN_MENU, BOARD_GAMEPLAY};

void init(std::vector<std::pair<std::string, int>>& diff_count, std::vector<BoardImage>& images);
void addPatka(std::vector<std::pair<std::string, int>>& diff_count, std::vector<BoardImage>& images);
void delPatka(std::vector<std::pair<std::string, int>>& diff_count, std::vector<BoardImage>& images);

int main(void) {

	videoSetMode(MODE_0_2D);
	vramSetBankA(VRAM_A_MAIN_SPRITE_0x06400000);
	oamInit(&oamMain, SpriteMapping_1D_128, false);
	lcdMainOnBottom();
	consoleDemoInit();

	std::vector<BoardImage> images; // do zape³nienia rêcznie
	std::vector<std::pair<std::string, int>> diff_count;
	init(diff_count, images);

	GameState gameState = GameState::MAIN_MENU;
	std::vector<BoardImage>::const_iterator cur_board;
	touchPosition touch;
	int keys;

	while(1) {
		lcdMainOnTop();
		Menu menu;
		menu.mainMenu(diff_count, images, 0);
		while (gameState == GameState::MAIN_MENU) {
			touchRead(&touch);
			scanKeys();
			keys = keysDown();
			if (keys) {
				menu.mainMenu(diff_count, images, keys);
			}
			if (menu.exit()) {
				cur_board = menu.getBoard();
				gameState = GameState::BOARD_GAMEPLAY;
			}
			if (touch.px) {
				if (touch.px > 252 && touch.py > 187) {
					if (diff_count.size() < 4) {
						addPatka(diff_count, images);
						menu.mainMenu(diff_count, images, 0);
					}
				}
				if (touch.px < 5 && touch.py < 5) {
					if (diff_count.size() > 3) {
						delPatka(diff_count, images);
						menu.setState(MenuState::CHOOSE_DIFFICULTY);
						menu.setBoard(-1);
						menu.mainMenu(diff_count, images, KEY_DOWN);
					}
				}
			}
			swiWaitForVBlank();
		 } 
		Board board(*cur_board);

		lcdMainOnBottom();
		consoleClear();
		board.update();

		while (gameState == GameState::BOARD_GAMEPLAY) {
			touchRead(&touch);
			scanKeys();
			keys = keysDown();
			if (touch.px || touch.py) {
				board.touchAction(touch);
			}
			if (keys) {
				board.keyAction(keys);
			}
			if (board.exit()) {
				gameState = GameState::MAIN_MENU;
			}
			swiWaitForVBlank();
			oamUpdate(&oamMain);
		}
		swiWaitForVBlank();
		oamUpdate(&oamMain);
	}

	return 0;
}

void init(std::vector<std::pair<std::string, int>>& diff_count, std::vector<BoardImage>& images) {
	// Easy
	images.push_back({ (u8*)big_benTiles, (u16*)big_benPal, 3, 2, 64, SpriteSize_64x64, "Big Ben" });
	images.push_back({ (u8*)castle_gardenTiles, (u16*)castle_gardenPal, 3, 2, 64, SpriteSize_64x64, "Castle Garden" });
	images.push_back({ (u8*)chessTiles, (u16*)chessPal, 3, 2, 64, SpriteSize_64x64, "Awesome Chess" });
	images.push_back({ (u8*)busTiles, (u16*)busPal, 3, 2, 64, SpriteSize_64x64, "Red Bus" });
	//// Medium
	images.push_back({ (u8*)fishNchipsTiles, (u16*)fishNchipsPal, 4, 3, 64, SpriteSize_64x64, "Have Some Fish" });
	images.push_back({ (u8*)london_eyeTiles, (u16*)london_eyePal, 4, 3, 64, SpriteSize_64x64, "London Eye" });
	images.push_back({ (u8*)shipTiles, (u16*)shipPal, 4, 3, 64, SpriteSize_64x64, "Some Nice Ship" });
	images.push_back({ (u8*)tower_bridgeTiles, (u16*)tower_bridgePal, 4, 3, 64, SpriteSize_64x64, "The Towerest of Bridges" });

	//// Hard
	images.push_back({ (u8*)fisterraTiles, (u16*)fisterraPal, 6, 4, 32, SpriteSize_32x32, "Fisterra in Spain" });
	images.push_back({ (u8*)rising_sunTiles, (u16*)rising_sunPal, 6, 4, 32, SpriteSize_32x32, "Rising Sun" });
	images.push_back({ (u8*)setting_sunTiles, (u16*)setting_sunPal, 6, 4, 32, SpriteSize_32x32, "Setting Sun" });
	images.push_back({ (u8*)small_beachTiles, (u16*)small_beachPal, 6, 4, 32, SpriteSize_32x32, "Small Beach" });

	diff_count.push_back(std::make_pair<std::string, int>("         Take It Easy", 4));
	diff_count.push_back(std::make_pair<std::string, int>("        Take It Medium", 4));
	diff_count.push_back(std::make_pair<std::string, int>("         Thats too far", 4));

}

void addPatka(std::vector<std::pair<std::string, int>>& diff_count, std::vector<BoardImage>& images) {
	// very hidden hard
	images.push_back({ (u8*)roadTiles, (u16*)roadPal, 8, 6, 32, SpriteSize_32x32, "The Way" });
	images.push_back({ (u8*)meTiles, (u16*)mePal, 8, 6, 32, SpriteSize_32x32, "Can You See Me?" });
	images.push_back({ (u8*)giewontTiles, (u16*)giewontPal, 8, 6, 32, SpriteSize_32x32, "Trip To Giewont" });
	images.push_back({ (u8*)workout_parkTiles, (u16*)workout_parkPal, 8, 6, 32, SpriteSize_32x32, "Some workout?" });

	diff_count.push_back(std::make_pair<std::string, int>("       That's ridiculous", 4));
}

void delPatka(std::vector<std::pair<std::string, int>>& diff_count, std::vector<BoardImage>& images) {
	for (int i = 0; i < diff_count[3].second; i++) {
		images.pop_back();
	}
	diff_count.pop_back();
}