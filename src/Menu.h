#ifndef MENU_H
#define MENU_H

#include <string>
#include "Constants.h"
#include "InputManager.h"

class Menu {
public:
    Menu();
    ~Menu() = default;

    // Reset menu to initial state
    void reset();

    // Process input and return whether selection was made
    bool handleInput(const InputManager& input);

    // Get currently selected option
    int getSelectedOption() const { return selectedOption; }

    // Set number of options for current menu
    void setNumOptions(int num) { numOptions = num; }

    // Get whether back was pressed
    bool isBackPressed() const { return backPressed; }

    // Clear back pressed flag
    void clearBackPressed() { backPressed = false; }

    // Initials entry methods
    void resetInitials();
    bool handleInitialsInput(const InputManager& input);
    const std::string& getInitials() const { return initials; }
    int getCursorPosition() const { return cursorPos; }
    bool isInitialsComplete() const { return initials.length() >= Constants::INITIALS_LENGTH; }

private:
    int selectedOption;
    int numOptions;
    bool backPressed;

    // Initials entry state
    std::string initials;
    int cursorPos;
};

#endif // MENU_H
