//
//  Header.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//
#pragma once

#include <Siv3D.hpp> // OpenSiv3D v0.4.3
#if defined(_WIN32) || defined(_WIN64)
	#include <Siv3D/Windows.hpp>
#endif

#include <stdio.h>
#include <fstream>
#include <algorithm>
#include <random>

#include "Enums.hpp"
#include "Structs.hpp"

#define CHIP_SIZE		64
#define TITLE			U"Albus Player"
#define VERSION			U"2.0 alpha2"
#define RELEASE_NUMBER	142
#define AROUND_TILES	4
#define DIRECTIONS		15
#define LAYER_TYPES		15

using namespace std;

class CityMap;
class Addon;
class ImagesStruct;
