#include "InputManager.h"
#include <cstdio>
#include <cctype>

InputManager::InputManager()
    : currentAction(InputAction::NONE)
    , currentDirection(Direction::NONE)
    , selectPressed(false)
    , backPressed(false)
    , pausePressed(false)
    , quitRequested(false)
    , textInput('\0') {
}

InputManager::~InputManager() {
    shutdown();
}

bool InputManager::init() {
    // Initialize game controller subsystem
    if (SDL_InitSubSystem(SDL_INIT_GAMECONTROLLER) < 0) {
        printf("Warning: Could not initialize game controller: %s\n", SDL_GetError());
        // Continue anyway - keyboard will still work
    }

    // Load controller mappings
    loadControllerMappings();

    // Open any already-connected controllers
    int numJoysticks = SDL_NumJoysticks();
    for (int i = 0; i < numJoysticks; ++i) {
        if (SDL_IsGameController(i)) {
            addController(i);
        }
    }

    return true;
}

void InputManager::shutdown() {
    // Close all controllers
    for (auto* controller : controllers) {
        if (controller) {
            SDL_GameControllerClose(controller);
        }
    }
    controllers.clear();
}

void InputManager::loadControllerMappings() {
    // Try to load additional controller mappings
    int mappingsAdded = SDL_GameControllerAddMappingsFromFile(Constants::CONTROLLER_DB_PATH);
    if (mappingsAdded > 0) {
        printf("Loaded %d controller mappings\n", mappingsAdded);
    }
}

void InputManager::addController(int deviceIndex) {
    if (SDL_IsGameController(deviceIndex)) {
        SDL_GameController* controller = SDL_GameControllerOpen(deviceIndex);
        if (controller) {
            controllers.push_back(controller);
            const char* name = SDL_GameControllerName(controller);
            printf("Controller connected: %s\n", name ? name : "Unknown");
        }
    }
}

void InputManager::removeController(SDL_JoystickID instanceId) {
    for (auto it = controllers.begin(); it != controllers.end(); ++it) {
        SDL_Joystick* joy = SDL_GameControllerGetJoystick(*it);
        if (joy && SDL_JoystickInstanceID(joy) == instanceId) {
            printf("Controller disconnected\n");
            SDL_GameControllerClose(*it);
            controllers.erase(it);
            break;
        }
    }
}

bool InputManager::processEvents() {
    // Reset single-frame inputs
    currentAction = InputAction::NONE;
    textInput = '\0';

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                quitRequested = true;
                return false;

            case SDL_KEYDOWN:
                handleKeyboardEvent(event);
                break;

            case SDL_CONTROLLERBUTTONDOWN:
                handleControllerButtonEvent(event);
                break;

            case SDL_CONTROLLERAXISMOTION:
                handleControllerAxisEvent(event);
                break;

            case SDL_CONTROLLERDEVICEADDED:
            case SDL_CONTROLLERDEVICEREMOVED:
                handleControllerDeviceEvent(event);
                break;

            case SDL_TEXTINPUT:
                handleTextInputEvent(event);
                break;

            default:
                break;
        }
    }

    return true;
}

void InputManager::handleKeyboardEvent(const SDL_Event& event) {
    switch (event.key.keysym.sym) {
        // Direction keys - WASD
        case SDLK_w:
        case SDLK_UP:
            currentAction = InputAction::UP;
            currentDirection = Direction::UP;
            break;

        case SDLK_s:
        case SDLK_DOWN:
            currentAction = InputAction::DOWN;
            currentDirection = Direction::DOWN;
            break;

        case SDLK_a:
        case SDLK_LEFT:
            currentAction = InputAction::LEFT;
            currentDirection = Direction::LEFT;
            break;

        case SDLK_d:
        case SDLK_RIGHT:
            currentAction = InputAction::RIGHT;
            currentDirection = Direction::RIGHT;
            break;

        // Select (Enter/Space)
        case SDLK_RETURN:
        case SDLK_SPACE:
            currentAction = InputAction::SELECT;
            selectPressed = true;
            break;

        // Back (Escape)
        case SDLK_ESCAPE:
            currentAction = InputAction::BACK;
            backPressed = true;
            break;

        // Pause (P)
        case SDLK_p:
            currentAction = InputAction::PAUSE;
            pausePressed = true;
            break;

        // Backspace for initials entry
        case SDLK_BACKSPACE:
            textInput = '\b'; // Use backspace character as signal
            break;

        default:
            break;
    }
}

void InputManager::handleControllerButtonEvent(const SDL_Event& event) {
    switch (event.cbutton.button) {
        // D-pad
        case SDL_CONTROLLER_BUTTON_DPAD_UP:
            currentAction = InputAction::UP;
            currentDirection = Direction::UP;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
            currentAction = InputAction::DOWN;
            currentDirection = Direction::DOWN;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
            currentAction = InputAction::LEFT;
            currentDirection = Direction::LEFT;
            break;

        case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
            currentAction = InputAction::RIGHT;
            currentDirection = Direction::RIGHT;
            break;

        // A/Cross button (Select)
        case SDL_CONTROLLER_BUTTON_A:
            currentAction = InputAction::SELECT;
            selectPressed = true;
            break;

        // B/Circle button (Back)
        case SDL_CONTROLLER_BUTTON_B:
            currentAction = InputAction::BACK;
            backPressed = true;
            break;

        // Start button (Pause/Select)
        case SDL_CONTROLLER_BUTTON_START:
            currentAction = InputAction::PAUSE;
            pausePressed = true;
            break;

        // Y/Triangle button (also select in menus)
        case SDL_CONTROLLER_BUTTON_Y:
            currentAction = InputAction::SELECT;
            selectPressed = true;
            break;

        default:
            break;
    }
}

void InputManager::handleControllerAxisEvent(const SDL_Event& event) {
    int value = event.caxis.value;

    // Apply dead zone
    if (std::abs(value) < Constants::ANALOG_DEAD_ZONE) {
        return;
    }

    switch (event.caxis.axis) {
        case SDL_CONTROLLER_AXIS_LEFTX:
        case SDL_CONTROLLER_AXIS_RIGHTX:
            if (value < -Constants::ANALOG_DEAD_ZONE) {
                currentAction = InputAction::LEFT;
                currentDirection = Direction::LEFT;
            } else if (value > Constants::ANALOG_DEAD_ZONE) {
                currentAction = InputAction::RIGHT;
                currentDirection = Direction::RIGHT;
            }
            break;

        case SDL_CONTROLLER_AXIS_LEFTY:
        case SDL_CONTROLLER_AXIS_RIGHTY:
            if (value < -Constants::ANALOG_DEAD_ZONE) {
                currentAction = InputAction::UP;
                currentDirection = Direction::UP;
            } else if (value > Constants::ANALOG_DEAD_ZONE) {
                currentAction = InputAction::DOWN;
                currentDirection = Direction::DOWN;
            }
            break;

        default:
            break;
    }
}

void InputManager::handleControllerDeviceEvent(const SDL_Event& event) {
    if (event.type == SDL_CONTROLLERDEVICEADDED) {
        addController(event.cdevice.which);
    } else if (event.type == SDL_CONTROLLERDEVICEREMOVED) {
        removeController(event.cdevice.which);
    }
}

void InputManager::handleTextInputEvent(const SDL_Event& event) {
    // Only accept single ASCII letters for initials
    if (event.text.text[0] != '\0' && event.text.text[1] == '\0') {
        char c = event.text.text[0];
        if (std::isalpha(static_cast<unsigned char>(c))) {
            textInput = static_cast<char>(std::toupper(static_cast<unsigned char>(c)));
        }
    }
}

void InputManager::clearFrameFlags() {
    selectPressed = false;
    backPressed = false;
    pausePressed = false;
    currentDirection = Direction::NONE;
}
