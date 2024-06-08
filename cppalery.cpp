#include <ncurses.h>
#include <string.h>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <iostream>
#include <cstdlib>

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
// TODO: sorting, optimization, display current dir name, search, alt keybindings for navigating
enum termColors
{
	DefaultColor = 0,
	DirColor = 1,
	SelectedColor = 2
};

const double widthMultiplier = 0.99;
const double heightMultiplier = 0.90;

// For some unknown reason, KEY_ENTER from ncurses lib doesn't work on my machine
#define KEY_ENTER_CUSTOM 10

// Defines on which line from the egde scrolling starts
const int scrollOffset = 2;

const std::string quitChar = "Q";
const std::string centerChar = "c";
const std::string fillChar = "f";

const std::vector<std::string> imgExtensions = {".jpg", ".webp", ".png", ".gif", ".jpeg"};
const std::string fehCenter = "feh --bg-center ";
const std::string fehFill = "feh --bg-fill ";

void runFeh(std::string fileName, std::string mode = "f")
{
	std::string command;
	if (mode == "f")
	{
		command = fehFill + fileName;
	}
	else
	{
		command = fehCenter + fileName;
	}

	system(command.c_str());
}

int getDirectorySize(fs::path path) {
	int i = 0;
	for (const auto &entry : fs::directory_iterator(path))
	{
		i++;
	}
	return i;
}

void scrollUp(int& ceiling, int& floor) {
	if (ceiling > 0) {
		ceiling--;
		floor--;
	}
}

void scrollDown(int &ceiling, int &floor, int dirSize) {
	if (floor < dirSize) {
		ceiling++;
		floor++;
	}
}

void resetScrolling(int &ceiling, int &floor, fs::path path, int maxH) {
	int dirSize = getDirectorySize(path);
	int dirEntryLines = maxH * heightMultiplier - 2;
	ceiling = 0;
	floor = (dirSize < dirEntryLines) ? dirSize : dirEntryLines;
}

void resizeWindow(WINDOW *window, int &maxH, int &maxW)
{
	getmaxyx(stdscr, maxH, maxW);
	wresize(window, maxH * heightMultiplier, maxW * widthMultiplier);
}

void refreshWindowSetup(WINDOW *window, int &maxH, int &maxW)
{
	werase(window);
	resizeWindow(window, maxH, maxW);
	box(window, 0, 0);
	mvwprintw(window, 0, 5, " CPPalery ");
	refresh();
}

std::vector<fs::directory_entry> getDirectoryContents(const std::string &path)
{
	std::vector<fs::directory_entry> res;
	for (const auto &entry : fs::directory_iterator(path))
	{
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
int printDirectoryContents(WINDOW *win, const std::string &path, int selection, int maxH, int maxW, int ceiling, int floor)
{
	std::vector<fs::directory_entry> contents = getDirectoryContents(path);
	int dirSize = contents.size();
	std::string filesStr = std::string(" Entries total: ").append(std::to_string(dirSize)).append(" ");

	for (int i = ceiling, j = 0; i < floor; i++, j++)
	{
		const std::string entryStr = contents[i].path().filename();
		const char *entryCStr = entryStr.c_str();

		if (i == selection)
		{
			wattron(win, COLOR_PAIR(termColors::SelectedColor));
			mvwprintw(win, j + 1, 1, entryCStr);
			wattroff(win, COLOR_PAIR(termColors::SelectedColor));
			continue;
		}

		if (contents[i].is_directory())
		{
			wattron(win, COLOR_PAIR(termColors::DirColor));
			mvwprintw(win, j + 1, 1, entryCStr);
			wattroff(win, COLOR_PAIR(termColors::DirColor));
		}
		else
		{
			mvwprintw(win, j + 1, 1, entryCStr);
		}
	}

	mvwprintw(win, maxH * heightMultiplier - 1, maxW * widthMultiplier - strlen(filesStr.c_str()) - 2, filesStr.c_str());
	wrefresh(win);
	return dirSize;
}

void loopOptions(WINDOW *win, int maxH, int maxW, fs::path workPath)
{
	// Selected directory entry
	int selection = 0;
	// Highest entry to be displayed
	int ceiling = 0;
	int dirSize = getDirectorySize(workPath);
	// Substract 2 for window borders
	int dirEntryLines = maxH * heightMultiplier - 2;
	// Lowest entry to be displayed
	int floor = (dirSize < dirEntryLines) ? dirSize : dirEntryLines;
	// Print initial state before user presses any key
	dirSize = printDirectoryContents(win, workPath.string(), selection, maxH, maxW, ceiling, floor);
	std::string str;
	while (true)
	{
		std::vector<fs::directory_entry> currentDirContents = getDirectoryContents(workPath.string());
		int ch = getch();

		switch (ch) {
		// Handle window resizing
		case KEY_RESIZE:
		{
			refreshWindowSetup(win, maxH, maxW);
			break;
		}

		case KEY_UP:
		{
			selection = (selection > 0) ? selection - 1 : selection;
			if (selection < ceiling + scrollOffset) {
				scrollUp(ceiling, floor);
			}
			refreshWindowSetup(win, maxH, maxW);
			if (selection == 0) {
				mvwprintw(win, maxH * heightMultiplier - 1, 1, "TOP");
			}
			break;
		}
		case KEY_DOWN:
		{
			selection = (selection < dirSize - 1) ? selection + 1 : selection;
			if (selection > floor - scrollOffset) {
				scrollDown(ceiling, floor, dirSize);
			}
			refreshWindowSetup(win, maxH, maxW);
			if (selection == dirSize - 1) {
				mvwprintw(win, maxH * heightMultiplier - 1, 1, "BOT");
			}
			break;
		}
		case KEY_LEFT:
		{
			workPath = (fs::canonical(workPath).has_parent_path()) ? fs::canonical(workPath / "..") : workPath;
			selection = 0;
			resetScrolling(ceiling, floor, workPath, maxH);
			refreshWindowSetup(win, maxH, maxW);
			break;
		}
		case KEY_ENTER_CUSTOM:
		case KEY_RIGHT:
		{
			fs::path currentFilePath = currentDirContents[selection].path();
			if (currentDirContents[selection].is_directory())
			{
				workPath = fs::canonical(currentFilePath);
				selection = 0;
			}
			resetScrolling(ceiling, floor, workPath, maxH);
			// Set background image
			if (count(imgExtensions.begin(), imgExtensions.end(), currentFilePath.extension()))
			{
				runFeh(currentFilePath.string());
			}
			refreshWindowSetup(win, maxH, maxW);
			break;
		}
		default:
			break;
		}
		// Handle window resizing
		if (ch == KEY_RESIZE)
		{
			refreshWindowSetup(win, maxH, maxW);
		}

		str.push_back(ch);
		if (str == quitChar)
			break;
		if (str == centerChar)
		{
			if (count(imgExtensions.begin(), imgExtensions.end(), currentDirContents[selection].path().extension()))
			{
				runFeh(currentDirContents[selection].path().string(), "c");
			}
		}
		if (str == fillChar)
		{
			if (count(imgExtensions.begin(), imgExtensions.end(), currentDirContents[selection].path().extension()))
			{
				runFeh(currentDirContents[selection].path().string(), "f");
			}
		}

		dirSize = printDirectoryContents(win, workPath.string(), selection, maxH, maxW, ceiling, floor);
		int floor = (dirSize < dirEntryLines) ? dirSize : dirEntryLines;

		wrefresh(win);
		str = "";
	}
}

int main(int argc, char **argv)
{
	if (argc < 2)
	{
		printf("Argument not provided. Provide a directory path.\n");
		return 1;
	}

	fs::path path;

	try
	{
		path = fs::canonical(argv[1]);
	}
	catch (const std::exception &ex)
	{
		std::cerr << ex.what() << "\n";
	}

	if (!fs::exists(path))
	{
		printf("Provided path not valid.\n");
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

	WINDOW *win = newwin(maxH * heightMultiplier, maxW * widthMultiplier, 1, 1);
	refresh();

	refreshWindowSetup(win, maxH, maxW);
	wrefresh(win);

	loopOptions(win, maxH, maxW, path.string());

	endwin();
	return 0;
}
