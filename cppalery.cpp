#include <ncurses.h>
#include <string.h>
#include <filesystem>
#include <vector>
#include <sstream>

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

enum termColors {
	DefaultColor = 0,
	DirColor = 1,
	SelectedColor = 2
};

const double widthMultiplier = 0.99;
const double heightMultiplier = 0.90;
const std::string quitChar = "Q";

void resizeWindow(WINDOW* window, int& maxH, int& maxW) {
   	getmaxyx(stdscr, maxH, maxW);
	wresize(window, maxH * heightMultiplier, maxW * widthMultiplier);
}

void refreshWindowSetup(WINDOW* window, int& maxH, int& maxW) {
	werase(window);
	resizeWindow(window, maxH, maxW);
    box(window, 0, 0);
    mvwprintw(window, 0, 5, " CPPalery ");
	refresh();
}

std::vector<fs::directory_entry> getDirectoryContents(const std::string& path) {
	std::vector<fs::directory_entry> res = {};
	for (const auto& entry : fs::directory_iterator(path)) {
		res.push_back(entry);
	}
	return res;
}

/**
 * @brief Renders directory contents and a selected file. Returns number of entries in the directory.
 * 
 * @param win pointer to the window
 * @param path path to the directory to be listed
 * @param selection selected directory entry
 * @return int number of files and subdirectories in directory
 */
int printDirectoryContents(WINDOW* win, const std::string& path, int selection, int maxH, int maxW) {
	std::vector<fs::directory_entry> contents = getDirectoryContents(path);
	int dirSize = contents.size();
	std::string filesStr = std::string(" Entries total: ").append(std::to_string(dirSize)).append(" ");

	for(int i = 0; i < dirSize; i++) {
		const char* entryStr = contents[i].path().filename().c_str();

		if(i == selection) {
			wattron(win, COLOR_PAIR(termColors::SelectedColor));
			mvwprintw(win, i + 1, 1, entryStr);
			wattroff(win, COLOR_PAIR(termColors::SelectedColor));
			continue;
		}

		if(contents[i].is_directory()) {
			wattron(win, COLOR_PAIR(termColors::DirColor));
			mvwprintw(win, i + 1, 1, entryStr);
			wattroff(win, COLOR_PAIR(termColors::DirColor));
		}
		else {
			mvwprintw(win, i + 1, 1, entryStr);
		}
	}

	mvwprintw(win, maxH * heightMultiplier - 1, maxW * widthMultiplier - strlen(filesStr.c_str()) - 2, filesStr.c_str());
	wrefresh(win);
	return dirSize;
}

void loopOptions(WINDOW* win, int maxH, int maxW, char* workPath) {
	std::string path = std::string(workPath);

	// Selected directory entry
	int selection = 0;
	// Print initial state before user presses any key
	int dirSize = printDirectoryContents(win, path, selection, maxH, maxW);

	std::string str;
	while (true) {
		std::vector<fs::directory_entry> currentDirContents = getDirectoryContents(path);
		int ch = getch();

		switch (ch) {
			// Handle window resizing
			case KEY_RESIZE: {
				refreshWindowSetup(win, maxH, maxW);
				break;
			}

			case KEY_UP: {
				selection = (selection > 0) ? selection - 1 : selection;
				refreshWindowSetup(win, maxH, maxW);
				mvwprintw(win, maxH * heightMultiplier - 1, 1, "UP");
				break;
			}
			case KEY_DOWN: {
				selection = (selection < dirSize - 1) ? selection + 1 : selection;
				refreshWindowSetup(win, maxH, maxW);
				mvwprintw(win, maxH * heightMultiplier - 1, 1, "DOWN");
				break;
			}
			case KEY_LEFT: {
				path = (fs::absolute(path).has_parent_path()) ? fs::absolute(path).parent_path().string() : path;
				selection = 0;
				refreshWindowSetup(win, maxH, maxW);
				mvwprintw(win, maxH * heightMultiplier - 1, 1, "LEFT");
				break;
			}
			case KEY_RIGHT: {
				if(currentDirContents[selection].is_directory()) {
					path = fs::absolute(currentDirContents[selection].path()).string();
				}
				selection = 0;
				refreshWindowSetup(win, maxH, maxW);
				//mvwprintw(win, maxH * heightMultiplier - 4, maxW * widthMultiplier - 15, currentDirContents[selection].path().string().c_str());
				mvwprintw(win, maxH * heightMultiplier - 1, 1, "RIGHT");
				break;
			}
			default:
				break;
		}
		// Handle window resizing
		if (ch == KEY_RESIZE) {
			refreshWindowSetup(win, maxH, maxW);
		}

		str.push_back(ch);
		if (str == quitChar) break;

		dirSize = printDirectoryContents(win, path, selection, maxH, maxW);

		//mvwprintw(win, maxH * heightMultiplier - 2, maxW * widthMultiplier - 15, path.c_str());
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
	start_color();
	init_pair(termColors::DirColor, COLOR_WHITE, COLOR_BLUE);
	init_pair(termColors::SelectedColor, COLOR_BLACK, COLOR_WHITE);
	use_default_colors();
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
