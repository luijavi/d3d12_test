#include "Keyboard.h"

bool Keyboard::KeyIsPressed(const unsigned char& keycode) const noexcept
{
    return keyStates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept
{
    if (keyBuffer.size() > 0u)
    {
        Keyboard::Event e = keyBuffer.front();
        keyBuffer.pop();
        return e;
    }
    else
    {
        return Keyboard::Event();
    }
}

bool Keyboard::KeyIsEmpty() const noexcept
{
    return keyBuffer.empty();
}

void Keyboard::ClearKey() noexcept
{
    // Just default constructs the keyBuffer, and replaces all values
    keyBuffer = std::queue<Event>();
}

char Keyboard::ReadChar() noexcept
{
    if (charBuffer.size() > 0u)
    {
        unsigned char charCode = charBuffer.front();
        charBuffer.pop();
        return charCode;
    }
    else
    {
        return 0;
    }
}

bool Keyboard::CharIsEmpty() const noexcept
{
    return charBuffer.empty();
}

void Keyboard::FlushChar() noexcept
{
    charBuffer = std::queue<char>();
}

void Keyboard::Flush() noexcept
{
    ClearKey();
    FlushChar();
}

void Keyboard::EnableAutorepeat() noexcept
{
    if (!autoRepeatEnabled)
        autoRepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept
{
    if (autoRepeatEnabled)
        autoRepeatEnabled = false;
}

bool Keyboard::AutoRepeatIsEnabled() const noexcept
{
    return autoRepeatEnabled;
}

// These are the private methods for our Window class
void Keyboard::OnKeyPressed(const unsigned char& keycode) noexcept
{
    // Sets the key state to true for the keycode (b/c it's pressed)
    keyStates[keycode] = true;
    // Adds a Key Is Pressed event to the queue
    keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
    TrimBuffer(keyBuffer);
}

void Keyboard::OnKeyReleased(const unsigned char& keycode) noexcept
{
    // Sets key state for keycode to false (b/c it's not being pressed)
    keyStates[keycode] = false;
    keyBuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
    TrimBuffer(keyBuffer);
}

void Keyboard::OnChar(const char& character) noexcept
{
    charBuffer.push(character);
    TrimBuffer(charBuffer);
}

void Keyboard::ClearState() noexcept
{
    keyStates.reset();
}
