#include <algorithm>
#include <array>
#include <string_view>
#include <Windows.h>

#include "imgui.h"
#include "InputUtil.h"

struct Key {
    constexpr Key(std::string_view name, int code) : name{ name }, code{ code } {  }

    std::string_view name;
    int code;
};

// indices must match KeyBind::KeyCode enum
static constexpr auto keyMap = std::to_array<Key>({
    { "'", VK_OEM_7 },
    { ",", VK_OEM_COMMA },
    { "-", VK_OEM_MINUS },
    { ".", VK_OEM_PERIOD },
    { "/", VK_OEM_2 },
    { "0", '0' },
    { "1", '1' },
    { "2", '2' },
    { "3", '3' },
    { "4", '4' },
    { "5", '5' },
    { "6", '6' },
    { "7", '7' },
    { "8", '8' },
    { "9", '9' },
    { ";", VK_OEM_1 },
    { "=", VK_OEM_PLUS },
    { "A", 'A' },
    { "ADD", VK_ADD },
    { "B", 'B' },
    { "BACKSPACE", VK_BACK },
    { "C", 'C' },
    { "CAPSLOCK", VK_CAPITAL },
    { "D", 'D' },
    { "DECIMAL", VK_DECIMAL },
    { "DELETE", VK_DELETE },
    { "DIVIDE", VK_DIVIDE },
    { "DOWN", VK_DOWN },
    { "E", 'E' },
    { "END", VK_END },
    { "ENTER", VK_RETURN },
    { "F", 'F' },
    { "F1", VK_F1 },
    { "F10", VK_F10 },
    { "F11", VK_F11 },
    { "F12", VK_F12 },
    { "F2", VK_F2 },
    { "F3", VK_F3 },
    { "F4", VK_F4 },
    { "F5", VK_F5 },
    { "F6", VK_F6 },
    { "F7", VK_F7 },
    { "F8", VK_F8 },
    { "F9", VK_F9 },
    { "G", 'G' },
    { "H", 'H' },
    { "HOME", VK_HOME },
    { "I", 'I' },
    { "INSERT", VK_INSERT },
    { "J", 'J' },
    { "K", 'K' },
    { "L", 'L' },
    { "LALT", VK_LMENU },
    { "LCTRL", VK_LCONTROL },
    { "LEFT", VK_LEFT },
    { "LSHIFT", VK_LSHIFT },
    { "M", 'M' },
    { "MOUSE1", VK_LBUTTON },
    { "MOUSE2", VK_RBUTTON },
    { "MOUSE3", VK_MBUTTON },
    { "MOUSE4", VK_XBUTTON1 },
    { "MOUSE5", VK_XBUTTON2 },
    { "MULTIPLY", VK_MULTIPLY },
    { "MWHEEL_DOWN", VK_MBUTTON },
    { "MWHEEL_UP", VK_MBUTTON },
    { "N", 'N' },
    { "NONE", 0 },
    { "NUMPAD_0", VK_NUMPAD0 },
    { "NUMPAD_1", VK_NUMPAD1 },
    { "NUMPAD_2", VK_NUMPAD2 },
    { "NUMPAD_3", VK_NUMPAD3 },
    { "NUMPAD_4", VK_NUMPAD4 },
    { "NUMPAD_5", VK_NUMPAD5 },
    { "NUMPAD_6", VK_NUMPAD6 },
    { "NUMPAD_7", VK_NUMPAD7 },
    { "NUMPAD_8", VK_NUMPAD8 },
    { "NUMPAD_9", VK_NUMPAD9 },
    { "O", 'O' },
    { "P", 'P' },
    { "PAGE_DOWN", VK_NEXT },
    { "PAGE_UP", VK_PRIOR },
    { "Q", 'Q' },
    { "R", 'R' },
    { "RALT", VK_RMENU },
    { "RCTRL", VK_RCONTROL },
    { "RIGHT", VK_RIGHT },
    { "RSHIFT", VK_RSHIFT },
    { "S", 'S' },
    { "SPACE", VK_SPACE },
    { "SUBTRACT", VK_SUBTRACT },
    { "T", 'T' },
    { "TAB", VK_TAB },
    { "U", 'U' },
    { "UP", VK_UP },
    { "V", 'V' },
    { "W", 'W' },
    { "X", 'X' },
    { "Y", 'Y' },
    { "Z", 'Z' },
    { "[", VK_OEM_4 },
    { "\\", VK_OEM_5 },
    { "]", VK_OEM_6 },
    { "`", VK_OEM_3 }
    });

static_assert(keyMap.size() == KeyBind::MAX);
static_assert(std::ranges::is_sorted(keyMap, {}, & Key::name));

KeyBind::KeyBind(KeyCode keyCode) noexcept : keyCode{ static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE } {}

KeyBind::KeyBind(const char* keyName) noexcept
{
    if (const auto it = std::ranges::lower_bound(keyMap, keyName, {}, &Key::name); it != keyMap.end() && it->name == keyName)
        keyCode = static_cast<KeyCode>(std::distance(keyMap.begin(), it));
    else
        keyCode = KeyCode::NONE;
}

const char* KeyBind::toString() const noexcept
{
    return keyMap[static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE].name.data();
}

const int KeyBind::toInt() const noexcept
{
    return keyMap[static_cast<std::size_t>(keyCode) < keyMap.size() ? keyCode : KeyCode::NONE].code; //Key::code
}

bool KeyBind::isPressed() const noexcept
{
    if (keyCode == KeyCode::NONE)
        return false;

    if (keyCode == KeyCode::MOUSEWHEEL_DOWN)
        return ImGui::GetIO().MouseWheel < 0.0f;

    if (keyCode == KeyCode::MOUSEWHEEL_UP)
        return ImGui::GetIO().MouseWheel > 0.0f;

    if (keyCode >= KeyCode::MOUSE1 && keyCode <= KeyCode::MOUSE5)
        return ImGui::IsMouseClicked(keyMap[keyCode].code);

    return static_cast<std::size_t>(keyCode) < keyMap.size() && ImGui::IsKeyPressed(static_cast<ImGuiKey>(keyMap[keyCode].code), false);
}

bool KeyBind::isDown() const noexcept
{
    if (keyCode == KeyCode::NONE)
        return false;

    if (keyCode == KeyCode::MOUSEWHEEL_DOWN)
        return ImGui::GetIO().MouseWheel < 0.0f;

    if (keyCode == KeyCode::MOUSEWHEEL_UP)
        return ImGui::GetIO().MouseWheel > 0.0f;

    if (keyCode >= KeyCode::MOUSE1 && keyCode <= KeyCode::MOUSE5)
        return ImGui::IsMouseDown(keyMap[keyCode].code);

    return static_cast<std::size_t>(keyCode) < keyMap.size() && ImGui::IsKeyDown(static_cast<ImGuiKey>(keyMap[keyCode].code));
}

bool KeyBind::setToKey(int code) noexcept
{
    for (std::size_t i = 0; i < keyMap.size(); ++i) {
        if (keyMap[i].code == code) {
            keyCode = static_cast<KeyCode>(i);
            return true;
        }
    }
    keyCode = KeyCode::NONE; // Default to NONE if the code is not found
    return false;
}

bool KeyBind::setToPressedKey() noexcept
{
    if (ImGui::IsKeyPressed(static_cast<ImGuiKey>(ImGui::GetIO().KeyMap[ImGuiKey_Escape]))) {
        keyCode = KeyCode::NONE;
        return true;
    }
    else if (ImGui::GetIO().MouseWheel < 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_DOWN;
        return true;
    }
    else if (ImGui::GetIO().MouseWheel > 0.0f) {
        keyCode = KeyCode::MOUSEWHEEL_UP;
        return true;
    }
    else if (GetKeyState(keyMap[LSHIFT].code) & 0x8000) {
        keyCode = KeyCode::LSHIFT;
        return true;
    }
    else if (GetKeyState(keyMap[LALT].code) & 0x8000) {
        keyCode = KeyCode::LALT;
        return true;
    }
    else if (GetKeyState(keyMap[LCTRL].code) & 0x8000) {
        keyCode = KeyCode::LCTRL;
        return true;
    }
    else if (GetKeyState(keyMap[MOUSE4].code) & 0x8000) {
        keyCode = KeyCode::MOUSE4;
        return true;
    }
    else if (GetKeyState(keyMap[MOUSE5].code) & 0x8000) {
        keyCode = KeyCode::MOUSE5;
        return true;
    }

    for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().MouseDown); ++i) {
        if (ImGui::IsMouseClicked(i)) {
            keyCode = KeyCode(KeyCode::MOUSE1 + i);
            return true;
        }
    }

    for (int i = 0; i < IM_ARRAYSIZE(ImGui::GetIO().KeysDown); ++i) {
        if (!ImGui::IsKeyPressed(static_cast<ImGuiKey>(i)))
            continue;

        if (const auto it = std::ranges::find(keyMap, i, &Key::code); it != keyMap.end()) {
            keyCode = static_cast<KeyCode>(std::distance(keyMap.begin(), it));
            // Treat AltGr as RALT
            if (keyCode == KeyCode::LCTRL && ImGui::IsKeyPressed(static_cast<ImGuiKey>(keyMap[KeyCode::RALT].code)))
                keyCode = KeyCode::RALT;
            return true;
        }
    }
    return false;
}

void KeyBindToggle::handleToggle() noexcept
{
    if (isPressed())
        toggledOn = !toggledOn;
}
