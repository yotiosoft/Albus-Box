//
//  Header.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#pragma once

#define SIV3D_WINDOWS_HIGH_DPI

#include <Siv3D.hpp> // OpenSiv3D v0.6.4
#if defined(_WIN32) || defined(_WIN64)
	#include <Siv3D/Windows/Windows.hpp>
	#include <WinUser.h>
#endif

#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <random>

#include "Enums.hpp"
#include "Structs.hpp"

#define TITLE			U"Albus Box"
<<<<<<< HEAD
#define VERSION			U"0.2.0"
=======
#define VERSION			U"0.1.3"
>>>>>>> b3d7cfe906b3ca87edb851eaebec1cf175732728
#define RELEASE_NUM		4
#define RANGE			1

using namespace std;

typedef struct Timestamp {
	int min;
	int sec;
} Timestamp;
