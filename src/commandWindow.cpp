#include "commandWindow.h"
#include "keyGlobals.h"

CommandWindow::CommandWindow() {}
CommandWindow::CommandWindow(int height, int width, int startX, int startY) {
    this->height = height;
    this->width = width;
    this->window = newwin(this->height, this->width, startX, startY);
}

void CommandWindow::resetSetup() {
    werase(this->window);
}

void CommandWindow::move(int newHeight, int newWidth, int startY, int startX) {
    resetSetup();
    // for some reason I coudn't get the mvwin to work
    delwin(this->window);
    this->window = newwin(newHeight, newWidth, startY, startX);
    this->resize(newHeight, newWidth);
}

void CommandWindow::resize(int newHeight, int newWidth) {
    this->width = newWidth;
    this->height = newHeight;
}

void CommandWindow::printStatus(int position, int total) {
    resetSetup();
    mvwprintw(this->window, 0, 1, this->info.c_str());
    if (total != 0) {
        int percent = position * 100 / total;
        std::ostringstream oss;
        oss << position << "/" << total << std::setw(STATUS_RULER_OFFSET) << percent << "%%";
        std::string posString = oss.str();
        mvwprintw(this->window, 0, this->width - posString.length(), posString.c_str());
    }
    wrefresh(this->window);
}

void CommandWindow::printHelp() {
    box(this->window, 0, 0);
    const std::string separator = " | ";
    const std::vector<std::array<std::string, 2>> KeySemantics = 
    {
        {std::string(1, static_cast<char>(QUIT_CHAR)), "Quit"},
        {std::string("ARROW LEFT").append(separator).append(std::string(1, static_cast<char>(KEY_LEFT_ALT))), "Move left"},
        {std::string("ARROW UP").append(separator).append(std::string(1, static_cast<char>(KEY_UP_ALT))), "Move up"},
        {std::string("ARROW DOWN").append(separator).append(std::string(1, static_cast<char>(KEY_DOWN_ALT))), "Move down"},
        {std::string("Enter | ARROW RIGHT").append(separator).append(std::string(1, static_cast<char>(KEY_RIGHT_ALT))), "Move right"},
        {std::string(1, static_cast<char>(HELP_CHAR)), "Display this help message"},
        {std::string("Enter | ARROW RIGHT").append(separator).append(std::string(1, static_cast<char>(KEY_RIGHT_ALT))), "Set background (fill)"},
        {std::string(1, static_cast<char>(CENTER_CHAR)), "Set background (centre)"},
        {std::string(1, static_cast<char>(RANDOM_CHAR)), "Set random background (fill)"},
        {std::string(1, static_cast<char>(SEARCH_CHAR)), "Search for substring in filenames"},
        {std::string(1, static_cast<char>(LOOP_RESULTS_CHAR)), "Iterate through results forward"},
        {std::string(1, static_cast<char>(LOOP_RESULTS_CHAR_BACK)), "Iterate through results backwards"},
        {std::string(1, static_cast<char>(KEY_MOVE_BOTTOM)), "Go to bottom"},
        {std::string(1, static_cast<char>(KEY_MOVE_TOP)).append(std::string(1, static_cast<char>(KEY_MOVE_TOP))), "Go to top"},
        {std::string(1, static_cast<char>(ASC_CHAR)), "Sort contents by name ascending"},
        {std::string(1, static_cast<char>(DESC_CHAR)), "Sort contents by name descending"},
        {std::string(1, static_cast<char>(HIDE_CHAR)), "Hide / show dotfiles"},
        {std::string(1, static_cast<char>(PATH_CHAR)), "Absolute / relative path"}
    };

    std::string helpStr;
    int maxKeyWidth = 0;
    int j = 2;
    for (auto i : KeySemantics) {
        if (i[0].length() > maxKeyWidth)
            maxKeyWidth = i[0].length();
    }

    std::string keysHeader = "key(s)";
    helpStr.append(keysHeader).append(maxKeyWidth - keysHeader.length(), ' ').append(separator).append("command");
    mvwprintw(this->window, j++, 2, helpStr.c_str());
    helpStr = std::string(width - 3, '-');
    mvwprintw(this->window, j++, 2, helpStr.c_str());

    for (auto i : KeySemantics) {
        helpStr = i[0].append(maxKeyWidth - i[0].length(), ' ').append(separator).append(i[1]);
        mvwprintw(this->window, j++, 2, helpStr.c_str());
    }
    wrefresh(window);
}

std::string CommandWindow::getSearchInput() {
    resetSetup();
    mvwprintw(this->window, 0, 1, "/");
    wrefresh(this->window);
    std::string str = "";
    int ch = getch();
    mvwprintw(this->window, 0, 2, str.c_str());
    wrefresh(this->window);
    while (ch != KEY_ENTER_ALT && ch != KEY_ENTER) {
        if (ch == KEY_DC || ch == '\b' || ch == KEY_BACKSPACE || ch == KEY_BACKSPACE_ALT) {
            if (str.empty()) {
                break;
            }
            else {
                str.pop_back();
            }
        }
        else if (ch == KEY_ESC) {
            break;
        }
        else {
            str.push_back(ch);
        }
        resetSetup();
        mvwprintw(this->window, 0, 1, "/");
        mvwprintw(this->window, 0, 2, str.c_str());
        wrefresh(this->window);
        ch = getch();
    }
    return str;
}
