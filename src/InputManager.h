#ifndef INPUTMANAGER_H
#define INPUTMANAGER_H

#include <SDL2/SDL.h>
#include <vector>
#include <memory>
#include "Constants.h"

class InputManager {
public:
    InputManager();
    ~InputManager();

    // Initialize input system
    bool init();

    // Shutdown input system
    void shutdown();

    // Process all pending input events
    // Returns false if quit event received
    bool processEvents();

    // Get the current input action (for menus)
    InputAction getAction() const { return currentAction; }

    // Get direction input (for gameplay)
    Direction getDirection() const { return currentDirection; }

    // Check if specific keys/buttons were just pressed
    bool isSelectPressed() const { return selectPressed; }
    bool isBackPressed() const { return backPressed; }
    bool isPausePressed() const { return pausePressed; }
    bool isQuitRequested() const { return quitRequested; }

    // Get text input for initials entry
    char getTextInput() const { return textInput; }
    bool hasTextInput() const { return textInput != '\0'; }

    // Clear single-frame flags (call at end of frame)
    void clearFrameFlags();

    // Check if any controller is connected
    bool hasController() const { return !controllers.empty(); }

private:
    // Handle different event types
    void handleKeyboardEvent(const SDL_Event& event);
    void handleControllerButtonEvent(const SDL_Event& event);
    void handleControllerAxisEvent(const SDL_Event& event);
    void handleControllerDeviceEvent(const SDL_Event& event);
    void handleTextInputEvent(const SDL_Event& event);

    // Add a controller
    void addController(int deviceIndex);

    // Remove a controller
    void removeController(SDL_JoystickID instanceId);

    // Load controller mappings from database file
    void loadControllerMappings();

    // Connected game controllers
    std::vector<SDL_GameController*> controllers;

    // Current input state
    InputAction currentAction;
    Direction currentDirection;

    // Single-frame button presses
    bool selectPressed;
    bool backPressed;
    bool pausePressed;
    bool quitRequested;

    // Text input
    char textInput;
};

#endif // INPUTMANAGER_H
