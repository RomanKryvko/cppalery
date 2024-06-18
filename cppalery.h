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
enum TermColors
{
	DefaultColor = 0,
	DirColor = 1,
	SelectedColor = 2
};

const int BOTTOM_OFFSET = 4;

// For some unknown reason, KEY_ENTER from ncurses lib doesn't work on my machine
const int KEY_ENTER_ALT = 10;
const int KEY_BACKSPACE_ALT = 127;
const int KEY_ESC = 27;
// Defines on which line from the egde scrolling starts
const int SCROLL_OFFSET = 2;

const int QUIT_CHAR = 'Q';
const int CENTER_CHAR = 'c';
const int FILL_CHAR = 'f';
const int HIDE_CHAR = 'h';
const int ASC_CHAR = 'a';
const int DESC_CHAR = 'd';
const int SEARCH_CHAR = '/';
const int LOOP_RESULTS_CHAR = 'N';

const std::vector<std::string> imgExtensions = {".jpg", ".webp", ".png", ".gif", ".jpeg"};

#endif