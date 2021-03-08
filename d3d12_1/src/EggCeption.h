#pragma once

#include <exception>
#include <string>

// Step 5: Create Exception class from std::exception to handle exceptions
class EggCeption : public std::exception
{
public:
	EggCeption(int line, const char* file) noexcept;
	const char* what() const noexcept override;		// Implementing the what func from std::exception
	virtual const char* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::string& GetFile() const noexcept;
	std::string GetOriginString() const noexcept;	// Formats type, line, and file into 1 string
private:
	int line;			// Line number exception was thrown from (printed from what())
	std::string file;	// File it was thrown from (printed from what())
protected:
	mutable std::string whatBuffer;	// mutable b/c what() is const, and whatBuffer needs to be set w/i what()
};