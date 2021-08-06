//
//  Specific.cpp
//  Cities Box
//
//  注 : これはmacOS向けです
//
//  Created by YotioSoft on 2019/12/01.
//

#include "Specific.hpp"

void specific::chdir(string dir) {
	::chdir(dir.c_str());
}

void specific::moveToCurrentDir() {
	chdir("./Cities-Box.app/Contents/Resources/assets");
}

bool specific::isCursorEntered(Vec2 before_cursor_pos) {
	return false;	// Mac版では何もしない
}

void specific::changeCursor() {
	return;			// Mac版では何もしない
}

String specific::getFontsDir() {
	return U"./Albus-Box.app/Contents/Resources/assets/fonts";
}
