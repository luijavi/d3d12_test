#include "EggCeption.h"
#include <sstream>

EggCeption::EggCeption(int line, const char* file) noexcept
	:
	line (line),
	file (file)
{}

const char* EggCeption::what() const noexcept
{
	std::ostringstream strStream;
	strStream << GetType() << std::endl
			  << GetOriginString();
	whatBuf = strStream.str();
	return whatBuf.c_str();
}

const char* EggCeption::GetType() const noexcept
{
	return "EggCeption!";
}

int EggCeption::GetLine() const noexcept
{
	return line;
}

const std::string& EggCeption::GetFile() const noexcept
{
	return file;
}

std::string EggCeption::GetOriginString() const noexcept
{
	std::ostringstream strStream;
	strStream << "[File] " << file << std::endl
			  << "[Line] " << line;
	return strStream.str();
}
