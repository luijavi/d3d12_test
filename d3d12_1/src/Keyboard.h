#pragma once

#include <queue>
#include <bitset>

/* Step 8: Create a class for keyboard
* This class will be a friend of window, so that it can
* interface with the Win32 messages, and it should also
* be able to interface with the game logic. Its main
* purpose is to process KeyDown, KeyUp, and Char msgs.
*/
class Keyboard
{
	// friend class of Window, so that Window can access private members
	friend class Window;
public:
	class Event
	{
	public:
		// Three types of events
		enum class Type
		{
			Press,
			Release,
			Invalid
		};
	private:
		// For every event, you've got a type, and the event stores the key of the code in the event
		Type type;
		unsigned char code;
	public:
		Event() noexcept // Default constructor
			:
			type(Type::Invalid),
			code(0u)
		{}
		Event(Type type, unsigned char code) noexcept // Constructor
			:
			type(type),
			code(code)
		{}
		// See if key is pressed, released, or invalid, and return the code
		bool IsPress() const noexcept
		{
			return type == Type::Press;
		}
		bool IsRelease() const noexcept
		{
			return type == Type::Release;
		}
		bool IsInvalid() const noexcept
		{
			return type == Type::Invalid;
		}
		unsigned char GetCode() const noexcept
		{
			return code;
		}
	};
public:
	Keyboard() = default;							// Default constructor
	Keyboard(const Keyboard&) = delete;				// Delete copy constructor
	Keyboard& operator=(const Keyboard&) = delete;	// Delete assignment constructor
	/********KEY EVENT FUNCTIONS********/
	bool KeyIsPressed(const unsigned char& keycode) const noexcept;	// Pass keycode to tell if key being pressed
	Event ReadKey() noexcept;			// Will pull an event off of event queue 
	bool KeyIsEmpty() const noexcept;	// Will check if there's any event in event queue
	void ClearKey() noexcept;			// Will clear the event queue
	
	/********CHAR EVENT FUNCTIONS********/
	char ReadChar() noexcept;			
	bool CharIsEmpty() const noexcept;	
	void FlushChar() noexcept;			
	void Flush() noexcept;				// Will flush both char events and key queue

	/****AUTOREPEAT CONTROL FUNCTIONS****/
	void EnableAutorepeat() noexcept;
	void DisableAutorepeat() noexcept;
	bool AutoRepeatIsEnabled() const noexcept;
private:
	// These methods are set to private, not meant to be used by the client - only by Window
	// They're meant to be called with a Windows Message is received
	void OnKeyPressed(const unsigned char& keycode) noexcept;	// When WM_KEYDOWN message received
	void OnKeyReleased(const unsigned char& keycode) noexcept;	// When WM_KEYUP message received
	void OnChar(const char& character) noexcept;				// When WM_CHAR message received
	void ClearState() noexcept;									// Clears bitset that contains all key states
	// Class template to clear the buffer for Event and Char
	template<typename T>										
	static void TrimBuffer(std::queue<T>& buffer) noexcept;		
private:
	// These are the private members of the Keyboard class
	static constexpr unsigned nKeys = 256u;		// Number of key, based on ~1 byte of VK codes
	static constexpr unsigned bufferSize = 16u;	// TODO: Figure out what this means
	bool autoRepeatEnabled = false;
	std::bitset<nKeys> keyStates;	// Bit flags for key states
	std::queue<Event> keyBuffer;	// queue of events (i.e., WM_ messages) - FIFO
	std::queue<char> charBuffer;	// queue of VK_ messages TODO: Verify this
};

template<typename T>
inline void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept
{
	while (buffer.size() > bufferSize)
	{
		buffer.pop();
	}
}
