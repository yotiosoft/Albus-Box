//
//  Specific.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/30.
//

#include <Specific.hpp>

#define DPI_STANDARD	96

double before_dpi_dist = 0.0;
HWND hWnd = 0;
bool before_pin_window = false;

Array<FileStruct> specific::getAllFilesName(string folder_path, string extension) {
	using namespace filesystem;
	directory_iterator iter(folder_path), end;
	error_code err;

	Array<string> s;

	Array<FileStruct> ret_str;

	for (; iter != end && !err; iter.increment(err)) {
		const directory_entry entry = *iter;

		string entry_str = replaceString(entry.path().string(), "\\", "/");
		s.push_back(entry_str);
		Array<string> spl = splitUTF8(entry_str, "/");
		
		// サブディレクトリの中身も検索
		if (spl.back().find(".") == string::npos) {
			Array<FileStruct> sub_dir_str = getAllFilesName(entry_str, extension);
			copy(sub_dir_str.begin(), sub_dir_str.end(), back_inserter(ret_str));
		}

		FileStruct new_fs;
		if (extension.size() == 0 || entry_str.find(extension) != string::npos) {
			new_fs.file_name = spl.back();
			new_fs.file_path = entry_str;
			new_fs.folder_name = spl.end()[-2];

			for (int i = 0; i < spl.size() - 1; i++) {
				new_fs.folder_path += spl[i] + "/";
			}

			ret_str.push_back(new_fs);
		}
	}

	return ret_str;
}

void specific::chdir(string dir) {
	::_chdir(dir.c_str());
}

String specific::getCurrentDir() {
	return U"./";		// 何もしない
}

String specific::getSettingFilePath() {
	return U"./data/settings/settings.json";
}

String specific::getAudioProfilesFilePath() {
	return U"./data/settings/audio_profiles.json";
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

double specific::getDpiDist() {
	return (double)GetDpiForWindow(hWnd) / DPI_STANDARD;
}

bool specific::setWindowStyle(int x1, int y1, int x2, int y2, int w, int h) {
	// ウィンドウハンドルを取得（Siv3D）
	if (hWnd == 0) {
		hWnd = static_cast<HWND>(s3d::Platform::Windows::Window::GetHWND());
	}

	double dpi_dist = getDpiDist();
	if (dpi_dist == before_dpi_dist) {
		return false;
	}
	before_dpi_dist = dpi_dist;

	// 角丸長方形の生成
	auto hRegion = CreateRoundRectRgn(x1, y1, x2 * dpi_dist, y2 * dpi_dist, w, h);
	// 角丸長方形をウィンドウの形に適用
	SetWindowRgn(hWnd, hRegion, 1);

	return true;
}

void specific::pinWindow(const bool pin) {
	if (pin != before_pin_window) {
		if (pin) {
			SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		else {
			SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_SHOWWINDOW);
		}
		before_pin_window = pin;
	}
}

void specific::moveWindow(Point& mouse_clicked, bool& window_moving) {
	// ウィンドウの移動
	if (MouseL.down()) {
		mouse_clicked = { (int)(Cursor::Pos().x * getDpiDist()), (int)(Cursor::Pos().y * getDpiDist()) };
		window_moving = true;
	}
	else if (MouseL.pressed() && Window::GetState().focused && /*Cursor::GetRequestedStyle() == CursorStyle::Arrow && */window_moving) {
		Window::SetPos(Cursor::ScreenPos() - mouse_clicked);
	}
	else {
		window_moving = false;
	}
}
