//
//  Specific.cpp
//  Cities Box
//
//  注 : これはLinux向けです
//
//  Created by YotioSoft on 2019/12/01.
//

#include "Specific.hpp"

String specific::getCurrentDir() {
	return U"./";
}

bool specific::isCursorEntered(Vec2 before_cursor_pos) {
	return false;	// Linux版では何もしない
}

void specific::changeCursor() {
	return;			// Linux版では何もしない
}

String specific::getSettingFilePath() {
	return U"./data/settings/settings.json";
}

String specific::getAudioProfilesFilePath() {
	return U"./data/settings/audio_profiles.json";
}

String specific::getLyricsDirPath() {
	return U"./data/lyrics";
}

bool specific::setWindowStyle(int x1, int y1, int x2, int y2, int w, int h) {
	return false;
}

void specific::moveWindow(Point& mouse_clicked, bool& window_moving) {
	// ウィンドウの移動
	if (MouseL.down()) {
		mouse_clicked = Cursor::Pos();
		window_moving = true;
	}
	else if (MouseL.pressed() && Window::GetState().focused && /*Cursor::GetRequestedStyle() == CursorStyle::Arrow && */window_moving) {
		Window::SetPos(Point{ Cursor::ScreenPos().x / 2, Cursor::ScreenPos().y / 2 } - mouse_clicked);
	}
	else {
		window_moving = false;
	}
}

void specific::pinWindow(bool temp) {
	return;
}
