#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H

#include <deque>
#include <functional>
#include <map>

static constexpr short KEY_ENTER_ALT = 10;
static constexpr short KEY_BACKSPACE_ALT = 127;
static constexpr short KEY_ESC = 27;
static constexpr short QUIT_CHAR = 'Q';

class Keybindings {
public:
    using KeySequence = std::vector<short>;
    using Action = std::function<void()>;

    Keybindings(const std::map<KeySequence, Action>& bindings);

    void addBinding(const KeySequence& sequence, Action action);

    void handleKeyPress(char key);

private:
    std::map<KeySequence, Action> bindings;
    std::deque<char> currentKeys;
    static constexpr size_t maxKeySequenceLength = 3;
};

#endif
