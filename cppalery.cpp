#include <ncurses.h>
#include <string>
#include <filesystem>
#include <signal.h>
#include <vector>

namespace fs = std::filesystem;
/* 
Current plan:
list everything, directories included
on enter go into firectory or execute (set as a wallpaper)
options: [c] - bg-center
         [f] - bg-fill
		 [q] - quit
		 [r] (or list explicitly) - set random
*/

// TODO next: highlighting
// remove dir name, leave only subdir/file name
// log and exec on arrow keys / enter key~

const double widthMultiplier = 0.99;
const double heightMultiplier = 0.90;

void resizeWindow(WINDOW* window, int& maxH, int& maxW) {
   	getmaxyx(stdscr, maxH, maxW);
	wresize(window, maxH * heightMultiplier, maxW * widthMultiplier);
}

void refreshWindowSetup(WINDOW* window, int& maxH, int& maxW) {
	werase(window);
	resizeWindow(window, maxH, maxW);
    box(window, 0, 0);
    mvwprintw(window, 0, 5, "CPPalery");
	refresh();
}

std::vector<std::string> getDirectoryContents(std::string path) {
	std::vector<std::string> res = {};
	for (const auto& entry : fs::directory_iterator(path)) {
		if (entry.is_directory()) {
			
		}
		res.push_back(entry.path().string());
	}
	return res;
}

void printDirectoryContents(WINDOW* win, std::vector<std::string> contents) {
	for(int i = 0; i < contents.size(); i++) {
		mvwprintw(win, i + 1, 1, contents[i].c_str());
		wrefresh(win);
	}
}

void loopOptions(WINDOW* win, int maxH, int maxW, char* workPath) {
	std::string str;
	std::string path = std::string(workPath);
	std::vector<std::string> dirContents = getDirectoryContents(path);
	printDirectoryContents(win, dirContents);

	while (true) {
		int ch = getch();

		// Handle window resizing
		if (ch == KEY_RESIZE) {
			refreshWindowSetup(win, maxH, maxW);
		}

		str.push_back(ch);
		if (str == "Q") break;

		std::vector<std::string> dirContents = getDirectoryContents(path);
		printDirectoryContents(win, dirContents);

		mvwprintw(win, maxH * heightMultiplier - 1, 1, str.c_str());
		wrefresh(win);
		str = "";
	}
}

int main (int argc, char** argv)
{
	if (argc < 2) {
		printf("Argument not provided. Provide a directory path.\n");
		return 1;
	}

    initscr();
	noecho();
	cbreak();
	intrflush(stdscr, FALSE);
	keypad(stdscr, TRUE);
	curs_set(0);

	int maxH, maxW;
   	getmaxyx(stdscr, maxH, maxW);

    WINDOW* win = newwin(maxH * heightMultiplier, maxW * widthMultiplier, 1, 1);
    refresh();

	refreshWindowSetup(win, maxH, maxW);
	wrefresh(win);

	loopOptions(win, maxH, maxW, argv[1]);

    endwin();
    return 0;
}
