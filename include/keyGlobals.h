#ifndef KEY_GLOBALS_H
#define KEY_GLOBALS_H

// For some unknown reason, KEY_ENTER from ncurses lib doesn't work on my machine
const int KEY_ENTER_ALT = 10;
const int KEY_BACKSPACE_ALT = 127;
const int KEY_ESC = 27;

const int QUIT_CHAR = 'Q';
const int CENTER_CHAR = 'c';
const int FILL_CHAR = 'f';
const int HIDE_CHAR = 'H';
const int ASC_CHAR = 'a';
const int DESC_CHAR = 'd';
const int SEARCH_CHAR = '/';
const int LOOP_RESULTS_CHAR = 'N';
const int KEY_LEFT_ALT = 'h';
const int KEY_RIGHT_ALT = 'l';
const int KEY_UP_ALT = 'k';
const int KEY_DOWN_ALT = 'j';

const int KEY_MOVE_TOP = 'g';
const int KEY_MOVE_BOTTOM = 'G';

#endif