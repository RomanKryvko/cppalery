#include "keybindings.h"

Keybindings::Keybindings(const std::map<KeySequence, Action>& bindings) :
    bindings(bindings)
{ }

void Keybindings::addBinding(const KeySequence& sequence, Action action) {
    bindings[sequence] = action;
}

void Keybindings::handleKeyPress(Key key) {
    currentKeys.push_back(key);
    if (currentKeys.size() > maxKeySequenceLength) {
        currentKeys.pop_front();
    }

    for (const auto& [sequence, action] : bindings) {
        if (std::equal(sequence.begin(), sequence.end(), currentKeys.begin())) {
            action();
            currentKeys.clear();
            return;
        }
    }
}
