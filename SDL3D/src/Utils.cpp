//// Copyright 2016 Carl Hewett
////
//// This file is part of SDL3D.
////
//// SDL3D is free software: you can redistribute it and/or modify
//// it under the terms of the GNU General Public License as published by
//// the Free Software Foundation, either version 3 of the License, or
//// (at your option) any later version.
////
//// SDL3D is distributed in the hope that it will be useful,
//// but WITHOUT ANY WARRANTY; without even the implied warranty of
//// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//// GNU General Public License for more details.
////
//// You should have received a copy of the GNU General Public License
//// along with SDL3D. If not, see <http://www.gnu.org/licenses/>.
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

#include <Utils.hpp>
#include <Definitions.hpp>

#include <SDL.h> // For quitting
#include <SDL_mixer.h> // For quitting
#include <fstream>

#include <sstream> // For std::getLine()

namespace Utils
{
std::ofstream gLogFile(LOG_FILE, std::ios::app); // Evil global

void closeLogFile() // Log file opens by itself, but doesn't close by itself
{
	gLogFile.close();
}

void clearDataOutput()
{
	if(gLogFile.is_open()) // Close the file
		gLogFile.close();

	// Open in non-append mode
	std::ofstream dataFile(LOG_FILE); // No error checking necessairy
	dataFile << "";
	dataFile.close();

	gLogFile.open(LOG_FILE, std::ios::app); // Reopen the file
}

void directly_logprint(const std::string& msg, int line, const char* file)
{
	gLogFile << msg << '\n';

#ifndef NDEBUG // Debug
	bool parametersDefined = false;

	if(file != 0) // If file is defined
	{
		gLogFile << "----- from file: " << file << '\n';
		parametersDefined = true;
	}

	if(line != -1) // If line is defined
	{
		gLogFile << "----- at line: " << line << '\n';
		parametersDefined = true;
	}

	if(parametersDefined) // Add a newline if you outputted something, for prettyness.
		gLogFile << '\n';

	gLogFile.flush(); // Whatever happens, flush out the buffer so we get logs even if it exploded: this is debug!
#endif
}

void directly_warn(const std::string& msg, int line, const char* file)
{
	std::string fullString = "\nWarning: " + msg; // Newline for looks
	directly_logprint(fullString, line, file);
}

// This quits the game, so don't expect to be able to do other things after calling this (including logging)!
void directly_crash(const std::string& msg, int line, const char* file)
{
	std::string fullString = "\nCrash: " + msg;

	directly_logprint(fullString, line, file);

	closeLogFile();

	// Lets be at least a bit nice
	SDL_Quit();
	Mix_CloseAudio();

	exit(1); // Not the best
}

// Only call when you know an SDL function failed
void directly_crashFromSDL(const std::string& msg, int line, const char* file)
{
	std::string sdlError = SDL_GetError();

	Utils::directly_logprint("\n" + msg, line, file);

	if(!sdlError.empty())
		Utils::directly_crash("SDL error: " + sdlError); // We already showed the line number and file up top
	else
		Utils::directly_crash("No SDL error"); // We already showed the line number and file up top

	SDL_ClearError();
}

// Does checks and returns the contents of the file
// If it failed, it returns an empty string
std::string getFileContents(const std::string& filePath)
{
	std::ifstream fileStream(filePath, std::ios::in | std::ios::binary);
	if(fileStream)
	{
		std::string contents;

		fileStream.seekg(0, std::ios::end);
		contents.resize((int)fileStream.tellg());
		fileStream.seekg(0, std::ios::beg);
		fileStream.read(&contents[0], contents.size());
		fileStream.close();
		return(contents);
	} else // Can't open the file!
	{
		std::string crashLog = "'" + filePath + "' doesn't exist or cannot be opened!";
		CRASH(crashLog);
		return std::string();
	}
}

// String splitting
std::vector<std::string>& splitString(const std::string& s, char delim, std::vector<std::string>& elems)
{
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

std::vector<std::string> splitString(const std::string& s, char delim)
{
	std::vector<std::string> elems;
	splitString(s, delim, elems);
	return elems;
}
}