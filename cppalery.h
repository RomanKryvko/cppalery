#include <ncurses.h>
#include <string.h>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>

namespace fs = std::filesystem;

#ifndef CPPALERY_H
#define CPPALERY_H
enum termColors
{
	DefaultColor = 0,
	DirColor = 1,
	SelectedColor = 2
};

const double widthMultiplier = 0.99;
const double heightMultiplier = 0.90;

// For some unknown reason, KEY_ENTER from ncurses lib doesn't work on my machine
const int KEY_ENTER_CUSTOM = 10;

// Defines on which line from the egde scrolling starts
const int scrollOffset = 2;

const std::string quitChar = "Q";
const std::string centerChar = "c";
const std::string fillChar = "f";
const std::string hideChar = "h";
const std::string ascChar = "a";
const std::string descChar = "d";

const std::vector<std::string> imgExtensions = {".jpg", ".webp", ".png", ".gif", ".jpeg"};
const std::string fehCenter = "feh --bg-center ";
const std::string fehFill = "feh --bg-fill ";

#endif