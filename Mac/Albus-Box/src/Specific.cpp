//
//  Specific.cpp
//  Cities Box
//
//  注 : これはmacOS向けです
//
//  Created by YotioSoft on 2019/12/01.
//

#include "Specific.hpp"

String specific::getCurrentDir() {
	return U"./Albus-Box.app/Contents/Resources/assets/";
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

String specific::getSettingFilePath() {
	return U"./Albus-Box.app/Contents/Resources/assets/data/settings/settings.json";
}

String specific::getAudioProfilesFilePath() {
	return U"./Albus-Box.app/Contents/Resources/assets/data/settings/audio_profiles.json";
}

bool specific::setWindowStyle(int x1, int y1, int x2, int y2, int w, int h) {
	return false;
}
