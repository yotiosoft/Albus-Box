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
	return U"./Albus-Box.app/Contents/";
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
	return U"./Albus-Box.app/Contents/data/settings/settings.json";
}

String specific::getAudioProfilesFilePath() {
	return U"./Albus-Box.app/Contents/data/settings/audio_profiles.json";
}

String specific::getLyricsDirPath() {
	return U"./Albus-Box.app/Contents/data/lyrics";
}

double specific::getDpiDist() {
	return 1.0;
}

bool specific::setWindowStyle(int x1, int y1, int x2, int y2, int w, int h) {
	return false;
}

void specific::moveWindow(Point& mouse_clicked, bool& window_moving) {
	return;		// macOSの場合、ウィンドウの移動はOS標準のタイトルバーで行うので別途対応する必要はなし
	/*
	// ウィンドウの移動
	if (MouseL.down()) {
		mouse_clicked = Cursor::Pos();
		window_moving = true;
	}
	else if (MouseL.pressed() && Window::GetState().focused && /*Cursor::GetRequestedStyle() == CursorStyle::Arrow && window_moving) {
		Window::SetPos(Point{ Cursor::ScreenPos().x / 2, Cursor::ScreenPos().y / 2 } - mouse_clicked);
	/*}
	else {
		window_moving = false;
	}*/
}

void specific::pinWindow(bool temp) {
	//SIV3D_ENGINE(s3d::Window);
	//std::cout << getHandle() << std::endl;
	return;
}
