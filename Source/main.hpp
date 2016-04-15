#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <sstream>
#include <iomanip>
#include <string>
#include <vector>
#include <future>
#include <thread>
#include <chrono>
#include <openssl/crypto.h>
#include <openssl/sha.h>
#include <boost/filesystem.hpp>

#include "GUI.hpp"
#include "Cursor.hpp"
#include "RichText.hpp"
#include "CurlFunctions.hpp"
#include "tinydir.h"


class LogQueue
{
	private:
		std::vector<std::string> logContent;
	public:
		void push_back(std::string content);
		void set(std::string content);
		std::string returnComplexText();
};

std::string calc_sha256(char* path);
std::string str_sha256(const std::string str);
void updateMesa(int* step);
void playMesa();
void setBarPercent(int percent);
void addLog(std::string content);
void setLog(std::string content);
int main();