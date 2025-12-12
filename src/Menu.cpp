#include "Menu.h"

Menu::Menu()
    : selectedOption(0)
    , numOptions(4)
    , backPressed(false)
    , cursorPos(0) {
}

void Menu::reset() {
    selectedOption = 0;
    backPressed = false;
}

void Menu::resetInitials() {
    initials.clear();
    cursorPos = 0;
}

bool Menu::handleInput(const InputManager& input) {
    InputAction action = input.getAction();

    switch (action) {
        case InputAction::UP:
            selectedOption--;
            if (selectedOption < 0) {
                selectedOption = numOptions - 1;
            }
            break;

        case InputAction::DOWN:
            selectedOption++;
            if (selectedOption >= numOptions) {
                selectedOption = 0;
            }
            break;

        case InputAction::SELECT:
            return true; // Selection made

        case InputAction::BACK:
            backPressed = true;
            break;

        default:
            break;
    }

    return false;
}

bool Menu::handleInitialsInput(const InputManager& input) {
    // Handle text input
    if (input.hasTextInput()) {
        char c = input.getTextInput();

        if (c == '\b') {
            // Backspace
            if (!initials.empty()) {
                initials.pop_back();
                if (cursorPos > 0) cursorPos--;
            }
        } else if (initials.length() < Constants::INITIALS_LENGTH) {
            // Add letter
            initials += c;
            cursorPos = static_cast<int>(initials.length());
        }
    }

    // Handle select (confirm)
    if (input.isSelectPressed() && initials.length() == Constants::INITIALS_LENGTH) {
        return true; // Initials entry complete
    }

    // Handle back
    if (input.isBackPressed()) {
        backPressed = true;
    }

    return false;
}
