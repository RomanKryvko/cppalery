#include "cppalery.h"

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

bool caseInsensitiveCompare(const fs::directory_entry& a, const fs::directory_entry& b) {
    std::string aName = a.path().filename().string();
    std::string bName = b.path().filename().string();

    std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
    std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);

    return aName < bName;
}

bool caseInsensitiveCompareDesc(const fs::directory_entry& a, const fs::directory_entry& b) {
    std::string aName = a.path().filename().string();
    std::string bName = b.path().filename().string();

    std::transform(aName.begin(), aName.end(), aName.begin(), ::tolower);
    std::transform(bName.begin(), bName.end(), bName.begin(), ::tolower);

    return aName > bName;
}

void formatDir(std::vector<fs::directory_entry> &dirContents, bool nameAsc = true, bool hideDots = true) {
	if (hideDots) {
		for (auto it = dirContents.begin(); it != dirContents.end();) {
			std::string fileName = it->path().filename().string();
			if (fileName[0] == '.') {
				it = dirContents.erase(it);
			} else {
				++it;
			}
		}
	}
	if (nameAsc) {
		std::sort(dirContents.begin(), dirContents.end(), caseInsensitiveCompare);
		return;
	}
	if (!nameAsc) {
		std::sort(dirContents.begin(), dirContents.end(), caseInsensitiveCompareDesc);
		return;
	}
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

void resetScrolling(int &ceiling, int &floor, int maxH, int dirSize) {
	int maxDisplayedDirEntries = maxH * heightMultiplier - 2;
	ceiling = 0;
	floor = (dirSize < maxDisplayedDirEntries) ? dirSize : maxDisplayedDirEntries;
}

void focusScrolling(int &ceiling, int &floor, int selection, int maxH, int dirSize) {
	int maxDisplayedDirEntries = maxH * heightMultiplier - 2;
	int adjustedSelection = selection - scrollOffset;
	ceiling = (adjustedSelection > 0) ? adjustedSelection : 0;
	if (ceiling == 0) {
		floor = (dirSize < maxDisplayedDirEntries) ? dirSize : maxDisplayedDirEntries;
	}
	else if (dirSize == ceiling + maxDisplayedDirEntries) {
		floor = dirSize;
	}
	else if (dirSize < ceiling + maxDisplayedDirEntries) {
		ceiling = dirSize - maxDisplayedDirEntries;
		floor = dirSize;
	}
	else if (dirSize > ceiling + maxDisplayedDirEntries) {
		floor = ceiling + maxDisplayedDirEntries;
	}
}

bool inString(std::string haystack, std::string needle) {
	std::transform(haystack.begin(), haystack.end(), haystack.begin(), ::tolower);
	std::transform(needle.begin(), needle.end(), needle.begin(), ::tolower);

	return haystack.find(needle) != std::string::npos;
}

void resetWindowSetup(WINDOW *window, int &maxH, int &maxW) {
	werase(window);
	getmaxyx(stdscr, maxH, maxW);
	wresize(window, maxH * heightMultiplier, maxW * widthMultiplier);
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

int findEntryInDirectory(std::vector<fs::directory_entry> contents, const std::string &str) {
	char firstChar = std::tolower(str[0]);
	int high = contents.size() - 1;
	int low = 0;
	formatDir(contents);
	while (low <= high) {
		int mid = low + (high - low) / 2;
		std::string entryStr = contents[mid].path().filename().string();
		char currentChar = std::tolower(entryStr[0]);
		if (currentChar == firstChar) {
			if (inString(entryStr, str)) {
				return mid;
			}
		}
		if (currentChar > firstChar) {
			high = mid - 1;
		}
		else {
			low = mid + 1;
		}
	}
	return -1;
}

/**
 * @brief Renders directory contents and a selected file. Returns number of entries in the directory.
 *
 * @param win pointer to the window
 * @param contents vector with all the directories' entries
 * @param selection selected directory entry
 * @param maxH max number of rows in the window
 * @param maxW max number of columns in the window
 * @param ceiling index of the highest directory entry to be listed
 * @param floor index of the lowest directory entry to be listed
 * @return int number of files and subdirectories in directory
 */
int printDirectoryContents(WINDOW *win, const std::vector<fs::directory_entry> &contents, std::string dirName, int selection, int maxH, int maxW, int ceiling, int floor) {
	resetWindowSetup(win, maxH, maxW);
	int dirSize = contents.size();

	std::string filesStr = std::string(" Entries total: ").append(std::to_string(dirSize)).append(" ");
	mvwprintw(win, maxH * heightMultiplier - 1, maxW * widthMultiplier - strlen(filesStr.c_str()) - 2, filesStr.c_str());

	mvwprintw(win, 0, 20, dirName.c_str());

	if (selection == 0) {
		mvwprintw(win, maxH * heightMultiplier - 1, 1, "TOP");
	}
	if (selection == dirSize - 1) {
		mvwprintw(win, maxH * heightMultiplier - 1, 1, "BOT");
	}

	for (int i = ceiling, j = 0; i < floor; i++, j++) {
		const std::string entryStr = contents[i].path().filename();
		const char *entryCStr = entryStr.c_str();

		if (i == selection) {
			wattron(win, COLOR_PAIR(termColors::SelectedColor));
			mvwprintw(win, j + 1, 1, entryCStr);
			wattroff(win, COLOR_PAIR(termColors::SelectedColor));
		}
		else if (contents[i].is_directory()) {
			wattron(win, COLOR_PAIR(termColors::DirColor));
			mvwprintw(win, j + 1, 1, entryCStr);
			wattroff(win, COLOR_PAIR(termColors::DirColor));
		}
		else {
			mvwprintw(win, j + 1, 1, entryCStr);
		}
	}

	wrefresh(win);
	return dirSize;
}

void loopOptions(WINDOW *win, int maxH, int maxW, fs::path workPath) {
	// Selected directory entry
	int selection = 0;
	// Highest entry to be displayed
	int ceiling = 0;
	std::vector<fs::directory_entry> currentDirContents = getDirectoryContents(workPath.string());
	bool isOrderAscending = true;
	bool hideDotfiles = true;
	formatDir(currentDirContents, isOrderAscending, hideDotfiles);
	int dirSize = currentDirContents.size();
	// Substract 2 for window borders
	int maxDisplayedDirEntries = maxH * heightMultiplier - 2;
	// Lowest entry to be displayed
	int floor = (dirSize < maxDisplayedDirEntries) ? dirSize : maxDisplayedDirEntries;
	std::string dirName = workPath.string();
	// Print initial state before user presses any key
	dirSize = printDirectoryContents(win, currentDirContents, dirName, selection, maxH, maxW, ceiling, floor);
	std::string str;
	while (true) {
		currentDirContents = getDirectoryContents(workPath.string());
		formatDir(currentDirContents, isOrderAscending, hideDotfiles);
		int ch = getch();

		switch (ch) {
			case KEY_RESIZE: {
				erase();
				resetWindowSetup(win, maxH, maxW);
				resetScrolling(ceiling, floor, maxH, currentDirContents.size());
				break;
			}

			case KEY_UP: {
				selection = (selection > 0) ? selection - 1 : selection;
				if (selection < ceiling + scrollOffset) {
					scrollUp(ceiling, floor);
				}
				break;
			}

			case KEY_DOWN: {
				selection = (selection < dirSize - 1) ? selection + 1 : selection;
				if (selection > floor - scrollOffset) {
					scrollDown(ceiling, floor, dirSize);
				}
				break;
			}

			case KEY_LEFT: {
				workPath = (fs::canonical(workPath).has_parent_path()) ? fs::canonical(workPath / "..") : workPath;
				currentDirContents = getDirectoryContents(workPath.string());
				formatDir(currentDirContents, isOrderAscending, hideDotfiles);
				selection = 0;
				resetScrolling(ceiling, floor, maxH, currentDirContents.size());
				break;
			}

			case KEY_ENTER_CUSTOM:
			case KEY_ENTER:
			case KEY_RIGHT: {
				fs::path currentFilePath = currentDirContents[selection].path();
				if (currentDirContents[selection].is_directory()) {
					workPath = fs::canonical(currentFilePath);
					currentDirContents = getDirectoryContents(workPath.string());
					formatDir(currentDirContents, isOrderAscending, hideDotfiles);
					selection = 0;
				}
				resetScrolling(ceiling, floor, maxH, currentDirContents.size());
				// Set background image
				if (count(imgExtensions.begin(), imgExtensions.end(), currentFilePath.extension())) {
					runFeh(currentFilePath.string());
				}
				break;
			}
			default:
				break;
		}

		str.push_back(ch);
		if (str == quitChar)
			break;
		if (str == centerChar) {
			if (count(imgExtensions.begin(), imgExtensions.end(), currentDirContents[selection].path().extension()))
			{
				runFeh(currentDirContents[selection].path().string(), "c");
			}
		}
		if (str == fillChar) {
			if (count(imgExtensions.begin(), imgExtensions.end(), currentDirContents[selection].path().extension()))
			{
				runFeh(currentDirContents[selection].path().string(), "f");
			}
		}
		if (str == hideChar) {
			hideDotfiles = !hideDotfiles;
			if (!hideDotfiles) {
				currentDirContents = getDirectoryContents(workPath);
			}
		}
		if (str == ascChar) {
			isOrderAscending = true;
		}
		if (str == descChar) {
			isOrderAscending = false;
		}
		if (str == searchChar) {
			mvwprintw(win, maxH * heightMultiplier - 1, 1, "/");
			wrefresh(win);
			str = "";
			int ch = getch();
			mvwprintw(win, maxH * heightMultiplier - 1, 1, str.c_str());
			wrefresh(win);
			while(ch != KEY_ENTER_CUSTOM) {
				switch (ch) {
					case KEY_DC:
					case '\b':
					case KEY_BACKSPACE:
					case KEY_BACKSPACE_ALT: {
						if (str.empty()) {
							break;
						}
						else {
							str.pop_back();
						}
						break;
					}
					default: {
						str.push_back(ch);
						break;
					}
				}
				box(win, 0, 0);
				mvwprintw(win, maxH * heightMultiplier - 1, 1, str.c_str());
				wrefresh(win);
				ch = getch();
			}
			int foundIdx = findEntryInDirectory(currentDirContents, str);
			if (foundIdx >= 0) {
				selection = foundIdx;
				focusScrolling(ceiling, floor, selection, maxH, dirSize);
				wrefresh(win);
			}
		}

		dirName = workPath.string();
		formatDir(currentDirContents, isOrderAscending, hideDotfiles);
		dirSize = printDirectoryContents(win, currentDirContents, dirName, selection, maxH, maxW, ceiling, floor);
		int floor = (dirSize < maxDisplayedDirEntries) ? dirSize : maxDisplayedDirEntries;

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

	resetWindowSetup(win, maxH, maxW);
	wrefresh(win);

	loopOptions(win, maxH, maxW, path.string());

	endwin();
	return 0;
}
