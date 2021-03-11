#pragma once
#include <queue>

class Mouse
{
	friend class Window;
public:
	class Event
	{
	public:
		// Type of event
		enum class Type
		{
			// TODO: Add a middle button? Maybe side button?
			LPress,
			LRelease,
			RPress,
			RRelease,
			WheelUp,
			WheelDown,
			Move,
			Enter,
			Leave,
			Invalid
		};
	private:
		Type type;
		bool leftIsPressed;		// state at time event happened
		bool rightIsPressed;	// state at time event happened
		int x;					// state at time event happened
		int y;					// state at time event happened
	public:
		Event() noexcept
			:
			type(Type::Invalid),
			leftIsPressed(false),
			rightIsPressed(false),
			x(0),
			y(0)
		{}
		Event(Type type, const Mouse& parent) noexcept
			:
			type(type),
			leftIsPressed(parent.leftIsPressed),
			rightIsPressed(parent.rightIsPressed),
			x(parent.x),
			y(parent.y)
		{}
	public:
		bool IsValid() const noexcept
		{
			return type != Type::Invalid;
		}
		Type GetType() const noexcept
		{
			return type;
		}
		std::pair<int, int> GetPos() const noexcept
		{
			return { x, y };
		}
		int GetXPos() const noexcept
		{
			return x;
		}
		int GetYPos() const noexcept
		{
			return y;
		}
		bool LeftIsPressed() const noexcept
		{
			return leftIsPressed;
		}
		bool RightIsPressed() const noexcept
		{
			return rightIsPressed;
		}
	};
public:
	Mouse() = default;
	Mouse(const Mouse&) = delete;
	Mouse& operator=(const Mouse&) = delete;
	std::pair<int, int> GetPos() const noexcept;
	int GetXPos() const noexcept;
	int GetYPos() const noexcept;
	bool IsInWindow() const noexcept;
	bool LeftIsPressed() const noexcept;
	bool RightIsPressed() const noexcept;
	Mouse::Event Read() noexcept;
	bool IsEmpty() const noexcept
	{
		return buffer.empty();
	}
	void Flush() noexcept;
private:
	// Methods for actually handing the Windows messages for mouse component
	void OnMouseMove(int newX, int newY) noexcept;
	void OnMouseLeave() noexcept;
	void OnMouseEnter() noexcept;
	void OnLeftPressed(int x, int y) noexcept;
	void OnLeftReleased(int x, int y) noexcept;
	void OnRightPressed(int x, int y) noexcept;
	void OnRightReleased(int x, int y) noexcept;
	void OnWheelDown(int x, int y) noexcept;
	void OnWheelUp(int x, int y) noexcept;
	void TrimBuffer() noexcept;
private:
	static constexpr unsigned int bufferSize = 16u;
	bool leftIsPressed = false;		// state variable
	bool rightIsPressed = false;	// state variable
	bool isInWindow = false;
	int x;							// x position state that we'll be saving
	int y;							// y position state that we'll be saving
	std::queue<Event> buffer;
};