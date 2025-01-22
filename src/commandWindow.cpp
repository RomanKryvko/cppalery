#include "commandWindow.h"
#include "keybindings.h"
#include "window.h"
#include <sstream>
#include <iomanip>

CommandWindow::CommandWindow() = default;

CommandWindow::CommandWindow(int height, int width, int startX, int startY) : Window(height, width) {
    window = newwin(height, width, startX, startY);
}

void CommandWindow::resetSetup() {
    werase(window);
}

void CommandWindow::setMessage(const std::string& msg) {
    message = msg;
}

void CommandWindow::move(int newHeight, int newWidth, int startY, int startX) {
    resetSetup();
    // for some reason I coudn't get the mvwin to work
    delwin(window);
    window = newwin(newHeight, newWidth, startY, startX);
    resize(newHeight, newWidth);
}

void CommandWindow::resize(int newHeight, int newWidth) {
    width = newWidth;
    height = newHeight;
}

void CommandWindow::printStatus(int position, int total) {
    resetSetup();
    mvwprintw(window, 0, 1, "%s", message.c_str());
    if (total != 0) {
        int percent = position * 100 / total;
        std::ostringstream oss;
        oss << position << "/" << total << std::setw(STATUS_RULER_GAP) << percent << "%";
        std::string posString = oss.str();
        mvwprintw(window, 0, width - posString.length() - STATUS_RULER_OFFSET, "%s", posString.c_str());
    }
    wrefresh(window);
}

void CommandWindow::printHelp() {
    box(window, 0, 0);
    const std::string separator = " | ";
    const std::vector<std::array<std::string, 2>> KeySemantics = 
    {
        {std::string(1, static_cast<char>(QUIT_CHAR)), "Quit"},
        {std::string("ARROW LEFT").append(separator).append(std::string(1, static_cast<char>('h'))), "Move left"},
        {std::string("ARROW UP").append(separator).append(std::string(1, static_cast<char>('k'))), "Move up"},
        {std::string("ARROW DOWN").append(separator).append(std::string(1, static_cast<char>('j'))), "Move down"},
        {std::string("Enter | ARROW RIGHT").append(separator).append(std::string(1, static_cast<char>('l'))), "Move right"},
        {std::string(1, static_cast<char>('?')), "Display this help message"},
        {std::string("Enter | ARROW RIGHT").append(separator).append(std::string(1, static_cast<char>('l'))), "Set background (auto mode)"},
        {std::string(1, static_cast<char>('c')), "Set background (centre)"},
        {std::string(1, static_cast<char>('r')), "Set random background (fill)"},
        {std::string(1, static_cast<char>('/')), "Search for substring in filenames"},
        {std::string(1, static_cast<char>('n')), "Iterate through results forward"},
        {std::string(1, static_cast<char>('N')), "Iterate through results backwards"},
        {std::string(1, static_cast<char>('G')), "Go to bottom"},
        {std::string(1, static_cast<char>('g')).append(std::string(1, static_cast<char>('g'))), "Go to top"},
        {std::string(1, static_cast<char>('z')).append(std::string(1, static_cast<char>('z'))), "Focus scrolling"},
        {std::string(1, static_cast<char>('a')), "Sort contents by name ascending"},
        {std::string(1, static_cast<char>('d')), "Sort contents by name descending"},
        {std::string(1, static_cast<char>('H')), "Hide / show dotfiles"},
        {std::string(1, static_cast<char>('p')), "Absolute / relative path"}
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
    mvwprintw(window, j++, 2, "%s", helpStr.c_str());
    helpStr = std::string(width - 3, '-');
    mvwprintw(window, j++, 2, "%s", helpStr.c_str());

    for (auto i : KeySemantics) {
        helpStr = i[0].append(maxKeyWidth - i[0].length(), ' ').append(separator).append(i[1]);
        mvwprintw(window, j++, 2, "%s", helpStr.c_str());
    }
    wrefresh(window);
}

std::string CommandWindow::getSearchInput() {
    resetSetup();
    mvwprintw(window, 0, 1, "/");
    wrefresh(window);
    std::string str = "";
    int ch = getch();
    mvwprintw(window, 0, 2, "%s", str.c_str());
    wrefresh(window);
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
        mvwprintw(window, 0, 1, "/");
        mvwprintw(window, 0, 2, "%s", str.c_str());
        wrefresh(window);
        ch = getch();
    }
    return str;
}
