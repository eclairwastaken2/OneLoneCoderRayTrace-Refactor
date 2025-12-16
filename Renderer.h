#pragma once
#include <string>
#include <Windows.h>
#include "Player.h"

void RenderFrame(
	wchar_t* screen, 
	HANDLE console, 
	const std::wstring& map, 
	const Player& player, 
	float elapsedTime
); 