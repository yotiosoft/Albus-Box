//
//  Specific.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/30.
//

#include "Specific.hpp"

void specific::chdir(string dir) {
	::_chdir(dir.c_str());
}

void specific::moveToCurrentDir() {
	return;		// 何もしない
}

bool specific::isCursorEntered(Vec2& before_cursor_pos) {
	Vec2 cursor_pos = Cursor::Pos();
	if (before_cursor_pos.x < 0 || before_cursor_pos.y < 0 || before_cursor_pos.x > Scene::Size().x || before_cursor_pos.y > Scene::Size().y) {
		if (cursor_pos.x >= 0 || cursor_pos.y >= 0 || cursor_pos.x <= Scene::Size().x || cursor_pos.y <= Scene::Size().y) {
			Cursor::RequestStyle(CursorStyle::Hidden);

			before_cursor_pos = cursor_pos;
			return true;
		}
	}
	before_cursor_pos = cursor_pos;
	return false;
}

void specific::changeCursor() {
	Cursor::RequestStyle(CursorStyle::Arrow);
}

String specific::getFontsDir() {
	return U"example/font";
}
