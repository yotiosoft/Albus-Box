#include "AlbusBox.hpp"

pair<bool, FilePath> FileOpen() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = { {U"音声ファイル", {U"mp3", U"ogg", U"wav", U"m4a"}} };
	String filePathTemp;
	if (const auto open = Dialog::OpenFile(ff)) {
		return pair<bool, FilePath>(true, open.value());
	}

	return pair<bool, FilePath>(false, FilePath());
}

bool ExitButton(Font& font16, Color& button_close_color, Texture& window_close_icon) {
	// ウィンドウを閉じるボタン
	if (OS == "Windows") {
		if (NeumorphismUI::CircleButton(Scene::Width()-30, 30, 15, window_close_icon)) {
			return true;
		}
	}
	else if (OS == "Mac") {
		if (NeumorphismUI::CircleButton(25, 25, 8, U"", font16, button_close_color)) {
			return true;
		}
	}
	
	return false;
}

void exit(Array<Audio>& audio_files_list, int track_in_list) {
	audio_files_list[track_in_list].stop();

	for (auto af : audio_files_list)
		af.release();
	
	System::Exit();
}

bool AlbusBoxSetting(Player& player, Font& font16B, Color& button_close_color, Texture& window_close_icon, Color& font_color) {
	// ボタン用アイコン
	Texture return_button_icon(Icon(IconFont::Return, 20));		// 戻る
	
	// アイコン
	Texture volume_icon(Icon(IconFont::Volume, 30));
	
	// アイコンの位置
	const int icon_left_x = 50;
	const int icon_top_y = 100;
	
	// 音量調整バー
	double volume = player.getVolume();
	double volume_before = volume;
	NeumorphismUI::Slider volume_bar(volume, Vec2{ icon_left_x+100, icon_top_y }, Scene::Width()-icon_left_x-100, 30);
	
	// ボタンの位置
	Point return_button_pos;
	if (OS == "Windows") {
		return_button_pos = Point(30, 40);
	}
	else if (OS == "Mac") {
		return_button_pos = Point(45, 10);
	}
	
	// マウスクリックした地点の記録用
	Point mouse_clicked;
	bool window_moving = false;
	
	while (System::Update()) {
		// 画面上部のボタン群
		// 閉じるボタン
		if (ExitButton(font16B, button_close_color, window_close_icon)) {
			return true;
		}
		// もどる
		if (NeumorphismUI::RectButton(return_button_pos, Vec2(40, 40), return_button_icon)) {
			return false;
		}
		
		// 画面タイトル
		font16B(U"設定").draw(Arg::center(Scene::Width()/2, 30), Color(font_color));
		
		// 各設定項目
		// 音量
		volume_icon.draw(icon_left_x, icon_top_y, Color(font_color));
		volume = volume_bar.draw();
		if (volume != volume_before) {
			volume_before = volume;
			player.changeVolumeTo(volume);
		}
		
		// ウィンドウの移動
		if (MouseL.down()) {
			mouse_clicked = Cursor::Pos();
			window_moving = true;
		}
		else if (MouseL.pressed() && Cursor::GetRequestedStyle() != CursorStyle::Hand && window_moving) {
			Window::SetPos(Cursor::ScreenPos() - mouse_clicked);
		}
		else {
			window_moving = false;
		}
	}
	return true;
}

void AlbusBox() {
	Scene::SetBackground(DEFAULT_BACKGROUND_COLOR);
	Window::SetStyle(WindowStyle::Frameless);
	Window::Resize(400, 640);

	specific::setWindowStyle(0, 0, 400, 640, 40, 40);

	Font font13(13, U"{}/NotoSansCJKjp/NotoSansCJKjp-Regular.otf"_fmt(specific::getFontsDir()));
	Font font16(16, U"{}/NotoSansCJKjp/NotoSansCJKjp-Regular.otf"_fmt(specific::getFontsDir()));
	Font font16B(16, U"{}/NotoSansCJKjp/NotoSansCJKjp-Bold.otf"_fmt(specific::getFontsDir()));

	// シークバー
	double play_pos = 0.0;		// シークバーの初期値
	int slider_width = Scene::Width() - 100;
	NeumorphismUI::Slider slider(play_pos, Vec2{ 50, Scene::Height() / 2 + 100 }, Scene::Width() - 100, 30);

	bool playing = false;	// 再生中
	bool before_playing = playing;

	// マウスクリックした地点の記録用
	Point mouse_clicked;
	bool window_moving = false;

	// アイコン
	Texture play_icon(Icon(IconFont::Play, 30));
	Texture pause_icon(Icon(IconFont::Pause, 30));

	Texture prev_icon(Icon(IconFont::Prev, 20));
	Texture next_icon(Icon(IconFont::Next, 20));

	Texture setting_icon(Icon(IconFont::Setting, 20));
	Texture fileopen_icon(Icon(IconFont::FileOpen, 20));

	Texture window_close_icon(Icon(IconFont::Times, 20));

	// ボタンの位置
	Point setting_button_pos;
	Point fileopen_button_pos;
	if (OS == "Windows") {
		setting_button_pos = Point(30, 30);
		fileopen_button_pos = Point(80, 30);
	}
	else if (OS == "Mac") {
		fileopen_button_pos = Point(Scene::Width() - 80, 30);
		setting_button_pos = Point(Scene::Width() - 30, 30);
	}

	// ボタン押下検知用
	bool prev_button_pressed = false;
	bool next_button_pressed = false;

	// 色
	Color font_color(Palette::Gray);
	Color button_close_color(Color(242, 38, 19));

	// サムネ用
	const int thumbnail_size = 260;
	Image thumbnail_image(thumbnail_size, thumbnail_size, Palette::White);
	Color thumbnail_color1(Palette::Orangered);
	Color thumbnail_color2(Palette::Mediumpurple);

	for (int y = 0; y < thumbnail_image.height(); y++) {
		for (int x = 0; x < thumbnail_image.width(); x++) {
			double t = (double)(x + y) / (thumbnail_image.width() + thumbnail_image.height() - 2);
			thumbnail_image[y][x] = thumbnail_color1.lerp(thumbnail_color2, t);
		}
	}
	RenderTexture thumbnail_texture(thumbnail_image);
	Texture thunbnail_image_texture(thumbnail_image);
	Circle thumbnail_circle(Scene::Width() / 2, Scene::Height() / 3, thumbnail_size / 2);

	//Wave wave = audio_file.getWave();
	//cout << wave.size() << " " << audio_file.samples() << endl;
	/*
	for (int t=0; t<wave.size(); t++) {
		wave[t].left = 0.3;
		wave[t].right = 0.3;
	}
	audio_file = Audio(wave);
	*/

	FFTResult fft;
	
	// プレイヤーの用意
	Player player;

	while (System::Update()) {
		// 画面上部のボタン群
		bool isExitButtonPushed = ExitButton(font16, button_close_color, window_close_icon);
		if (isExitButtonPushed)
			break;

		// 画面上部のボタン群
		// 設定へ画面遷移
		if (NeumorphismUI::CircleButton(setting_button_pos, 20, setting_icon)) {
			if (AlbusBoxSetting(player, font16B, button_close_color, window_close_icon, font_color)) {
				break;		// 閉じるボタンが押されたらループを抜ける
			}
		}
		
		// ファイルを開く
		if (NeumorphismUI::CircleButton(fileopen_button_pos, 20, fileopen_icon)) {
			auto file_open = FileOpen();
			if (file_open.first) {
				player.open(file_open.second);
			}
		}

		// サムネイル
		NeumorphismUI::NeumorphismCircle(Scene::Width() / 2, Scene::Height() / 3, thumbnail_size / 2 + 10, false);

		// 波形を表示
		LineString fft_line;
		if (playing && player.isOpened()) {
			ScopedRenderTarget2D target(thumbnail_texture);
			thunbnail_image_texture.draw(0, 0);
			player.fft(fft);
			int fft_size = 800;
			int box_size = 10;
			int row_boxes = 20;
			for (int i = 0; i < row_boxes; i++) {
				double size = Pow(fft.buffer[i * (double)fft_size / row_boxes], 0.6f) * 2000;
				int fft_box_count = size / box_size;
				for (int j = 0; j < fft_box_count; j++) {
					Rect(Arg::center(i * thumbnail_texture.width() / row_boxes + box_size / 2, thumbnail_texture.height() - j * box_size - box_size / 2), box_size * 3 / 4, box_size * 3 / 4).draw(Color(200, 200, 200, 200));
				}
			}
		}
		else {
			ScopedRenderTarget2D target(thumbnail_texture);
			thunbnail_image_texture.draw(0, 0);
		}

		thumbnail_circle(thumbnail_texture(0, 0, thumbnail_size, thumbnail_size)).draw();

		// タイトル
		cMes(font16, player.getTitle(), Point(0, Scene::Height() / 3 + 120), Size(Scene::Width(), 100), font_color);

		// シークバー
		if (!slider.isSliderMoving()) {
			slider.setValueNoAnimetion(player.getPlayPosNorm());
			//cout << (double)audio_file.posSample()/audio_file.samples() << endl;
		}
		play_pos = slider.draw();
		if (slider.isSliderLeftReleased()) {
			player.seekTo(play_pos);
		}

		// 再生位置表示
		font13(U"{}:{:0>2}"_fmt(player.getPlayPosTimeMin(), player.getPlayPosTimeSec())).draw(40, Scene::Height() / 2 + 140, font_color);
		font13(U"{}:{:0>2}"_fmt(player.getTotalTimeMin(), player.getTotalTimeSec())).draw(Arg::topRight = Point(Scene::Width() - 40, Scene::Height() / 2 + 140), font_color);

		// トラック移動ボタン
		prev_button_pressed = NeumorphismUI::CircleButton(Scene::Width() / 4 - 10, Scene::Height() - 100, 30, prev_icon);
		next_button_pressed = NeumorphismUI::CircleButton(Scene::Width() - Scene::Width() / 4 + 10, Scene::Height() - 100, 30, next_icon);
		
		if (prev_button_pressed)
			player.previous();
		if (next_button_pressed)
			player.next();

		// 再生/一時停止ボタン
		NeumorphismUI::CircleSwitch(Scene::Width() / 2, Scene::Height() - 100, 40, playing, play_icon, pause_icon);
		if (before_playing != playing) {
			if (playing)
				player.play();
			else
				player.pause();
			
			before_playing = playing;
		}
		
		playing = player.playing();
		
		// ウィンドウの移動
		if (MouseL.down()) {
			mouse_clicked = Cursor::Pos();
			window_moving = true;
		}
		else if (MouseL.pressed() && Cursor::GetRequestedStyle() != CursorStyle::Hand && window_moving) {
			Window::SetPos(Cursor::ScreenPos() - mouse_clicked);
		}
		else {
			window_moving = false;
		}
	}
}
