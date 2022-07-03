#include "AlbusBox.hpp"

bool pin_window = false;

pair<bool, FilePath> AudioFileOpen() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = { {U"音声ファイルとプレイリスト", {U"mp3", U"ogg", U"wav", U"m4a", U"playlist"}} };
	String filePathTemp;
	if (const auto open = Dialog::OpenFile(ff)) {
		return pair<bool, FilePath>(true, open.value());
	}

	return pair<bool, FilePath>(false, FilePath());
}

pair<bool, FilePath> OpenPlayList() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = { {U"プレイリスト", {U"playlist"}} };
	String filePathTemp;
	if (const auto open = Dialog::OpenFile(ff)) {
		return pair<bool, FilePath>(true, open.value());
	}

	return pair<bool, FilePath>(false, FilePath());
}

pair<bool, FilePath> ImageFileOpen() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = { {U"画像ファイル", {U"png", U"jpg", U"jpeg", U"bmp", U"gif", U"tga", U"ppm", U"pgm", U"pbm", U"pnm", U"webp"}} };
	String filePathTemp;
	if (const auto open = Dialog::OpenFile(ff)) {
		return pair<bool, FilePath>(true, open.value());
	}

	return pair<bool, FilePath>(false, FilePath());
}

bool ExitButton(Color& button_close_color, Texture& window_close_icon, bool& onAnyButton) {
#if defined(_WIN32) || defined(_WIN64)
		// 最前面表示ボタン
		Font small_font = FontAsset(U"icon_font");
		if (pin_window) {
			NeumorphismUI::CircleSwitch(Scene::Width() - 70, 30, 15, pin_window, U"\U000F0404", small_font, onAnyButton);
		}
		else {
			NeumorphismUI::CircleSwitch(Scene::Width() - 70, 30, 15, pin_window, U"\U000F0403", small_font, onAnyButton);
		}
		specific::pinWindow(pin_window);

		// ウィンドウを閉じるボタン
		if (NeumorphismUI::CircleButton(Scene::Width()-30, 30, 15, window_close_icon, onAnyButton)) {
			return true;
		}
#endif
	
	return false;
}

// 再生リスト
bool playListView(Player& player, Color& button_close_color, Texture& window_close_icon, Color& font_color)
{
	// ボタン用アイコン
	Texture return_button_icon{ Icon(IconFont::Return), 20 };		// 戻る
	Texture fileopen_icon{ Icon(IconFont::Plus), 20 };				// ファイルを追加
	Texture listopen_button_icon{ Icon(IconFont::FileOpen), 20 };	// リストを読み込み
	Texture save_button_icon{ Icon(IconFont::Save), 20 };			// 保存

	Texture play_icon{ Icon(IconFont::Play), 20 };			// 再生
	Texture stop_icon{ Icon(IconFont::Stop), 20 };			// 停止

	// ボタンの位置
	Point return_button_pos;
	return_button_pos = Point(10, 10);
	Point fileopen_button_pos = Point(Scene::Width()-50, Scene::Height()-50);
	Point listopen_button_pos = Point(Scene::Width() / 2 - 30, 70);
	Point save_button_pos = Point(Scene::Width() / 2 + 30, 70);

	// シークバー
	double play_pos = 0.0;		// シークバーの初期値
	int slider_width = Scene::Width() - 40 - 100 - 30;
	NeumorphismUI::Slider slider(play_pos, Vec2{ 0, 0 }, slider_width, 20);

	// リストビューを作成
	pair<Array<String>, Array<bool>> title_list = player.getTitleList();

	// リスト表示用RenderTexture
	RenderTexture listview_texture(Scene::Width(), Scene::Height()-100, Color(DEFAULT_BACKGROUND_COLOR));
	RenderTexture title_texture(250, 40, Color(DEFAULT_BACKGROUND_COLOR));
	Mat3x2 mat, mat_mouse, mat_title;
	double scroll_y = 0.0, scroll_y_before = 0.0;
	const int list_element_h = 80;
	const int list_element_margin = list_element_h + 10;

	Array<bool> before_playing(title_list.first.size(), false);
	before_playing = title_list.second;
	int target_audio = -1;

	// マウスクリックした地点の記録用
	Point mouse_clicked;
	bool window_moving = false;
	
	bool onAnyButton, dpi_update = true;

	while (System::Update()) {
		onAnyButton = false;
		
		title_list = player.getTitleList();
		before_playing = title_list.second;

		// 画面上部のボタン群
		// 閉じるボタン
		if (ExitButton(button_close_color, window_close_icon, onAnyButton)) {
			return true;
		}
		// もどる
		if (NeumorphismUI::RectButton(return_button_pos, Vec2(40, 40), return_button_icon, onAnyButton)) {
			return false;
		}

		// リストを開くボタン
		if (NeumorphismUI::CircleButton(listopen_button_pos, 20, listopen_button_icon, onAnyButton)) {
			pair<bool, FilePath> playlist = OpenPlayList();
			if (playlist.first) {
				player.loadPlayList(playlist.second);
			}
		}
		// 保存ボタン
		if (NeumorphismUI::CircleButton(save_button_pos, 20, save_button_icon, onAnyButton)) {
			player.savePlayList();
		}

		// 画面タイトル
		FontAsset(U"title")(U"プレイリスト").draw(Arg::center(Scene::Width() / 2, 30), Color(font_color));

		// リストを表示
		scroll_y += Mouse::Wheel() * 10;
		if (scroll_y < 0.0) {
			scroll_y = 0.0;
		}
		if (scroll_y > list_element_margin * (title_list.first.size() - 1)) {
			scroll_y = scroll_y_before;
		}
		scroll_y_before = scroll_y;

		//NeumorphismUI::NeumorphismRect(20, 80, Scene::Width()-40, Scene::Height()-80-20, true);

		mat = Mat3x2::Translate(0, 10-scroll_y);
		mat_mouse = Mat3x2::Translate(0, 110 - scroll_y);
		{
			const ScopedRenderTarget2D target(listview_texture);
			listview_texture.clear(DEFAULT_BACKGROUND_COLOR);

			const Transformer2D t(mat, mat_mouse);

			for (int i = 0; i < title_list.first.size(); i++) {
				// 土台
				NeumorphismUI::NeumorphismRect(20, list_element_margin * i, Scene::Width()-40, list_element_h, false);

				// ボタンの有効・無効
				bool button_enable = true;
				if (110 - scroll_y + list_element_margin * i + list_element_h / 2 < 90) {
					button_enable = false;
				}

				// 再生・停止ボタン
				if (title_list.second[i]) {	// 再生中
					NeumorphismUI::CircleSwitch(Vec2(70, list_element_margin * i + list_element_h/2), 25, title_list.second[i], stop_icon, onAnyButton, button_enable);
					
					if (target_audio != i) {
						slider.setPosition(Point(120, list_element_margin * i + 50));
						target_audio = i;
					}

					if (!slider.isSliderMoving()) {
						slider.setValueNoAnimetion(player.getPlayPosNorm());
					}
					play_pos = slider.draw(onAnyButton, button_enable);
					if (slider.isSliderLeftReleased() && button_enable) {
						player.seekTo(play_pos);
					}

					if (before_playing[i] != title_list.second[i]) {
						player.pause();
						target_audio = -1;
					}

					// タイトル
					mat_title = Mat3x2::Translate(0, scroll_y);
					const Transformer2D t2(mat_title);
					{
						const ScopedRenderTarget2D target2(title_texture);
						//title_texture.clear(DEFAULT_BACKGROUND_COLOR);
						Rect(0, 0, Scene::Width(), Scene::Height() - 100).draw(DEFAULT_BACKGROUND_COLOR);

						DrawableText title_text = FontAsset(U"middle")(title_list.first[i]);
						int title_w = title_text.region(0, 0).x;
						int title_x = 0;
						if (title_w > 250) {
							title_x = -Scene::FrameCount() % title_w;
						}

						title_text.draw(title_x, 0, font_color);
					}
					title_texture.draw(120, list_element_margin * i + 10 - scroll_y);
				}
				else {		// 別の曲が再生中
					NeumorphismUI::CircleSwitch(Vec2(70, list_element_margin * i + list_element_h / 2), 25, title_list.second[i], play_icon, onAnyButton, button_enable);

					if (before_playing[i] != title_list.second[i]) {
						if (target_audio >= 0) {
							player.pause();
							title_list.second[target_audio] = false;
							before_playing[target_audio] = false;
						}
						player.play(i);
					}

					// タイトル
					mat_title = Mat3x2::Translate(0, scroll_y);
					const Transformer2D t2(mat_title);
					{
						const ScopedRenderTarget2D target2(title_texture);
						//title_texture.clear(DEFAULT_BACKGROUND_COLOR);
						Rect(0, 0, Scene::Width(), Scene::Height() - 100).draw(DEFAULT_BACKGROUND_COLOR);

						DrawableText title_text = FontAsset(U"middle")(title_list.first[i]);
						int title_w = title_text.region(0, 0).x;
						int title_x = 0;
						if (title_w > 250) {
							title_x = -Scene::FrameCount() % title_w;
						}

						title_text.draw(title_x, 0, font_color);
					}
					title_texture.draw(120, list_element_margin * i + 20 - scroll_y);
				}
			}
		}
		listview_texture.draw(0, 100);

		// ファイルを開くボタンを表示
		if (NeumorphismUI::CircleButton(fileopen_button_pos, 30, fileopen_icon, onAnyButton)) {
			auto file_open = AudioFileOpen();
			if (file_open.first) {
				player.audioRegister(file_open.second);
			}
		}

		// 再生処理を継続
		player.playing();

		// ウィンドウの移動
#if defined(_WIN32) || defined(_WIN64)
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, dpi_update);
		}

		dpi_update = !dpi_update;
#endif
    }
    
	return true;
}

bool VersionInformation(Player& player, Color& button_close_color, Texture& window_close_icon, Color& font_color) {
	// ボタン用アイコン
	Texture return_button_icon{ Icon(IconFont::Return), 20 };		// 戻る

	// 画像
	Texture albus_box_icon(U"{}/data/img/icon.png"_fmt(specific::getCurrentDir()));

	// ボタンの位置
	Point return_button_pos;
	return_button_pos = Point(10, 10);

	// マウスクリックした地点の記録用
	Point mouse_clicked;
	bool window_moving = false;
	
	bool onAnyButton, dpi_update = true;
	
	while (System::Update()) {
		onAnyButton = false;
		
		// 画面上部のボタン群
		// 閉じるボタン
		if (ExitButton(button_close_color, window_close_icon, onAnyButton)) {
			return true;
		}
		// もどる
		if (NeumorphismUI::RectButton(return_button_pos, Vec2(40, 40), return_button_icon, onAnyButton)) {
			return false;
		}

		// 画面タイトル
		FontAsset(U"title")(U"バージョン情報").draw(Arg::center(Scene::Width() / 2, 30), Color(font_color));

		// バージョン情報
		albus_box_icon.resized(200, 200).drawAt(Scene::Width() / 2, 200);

		FontAsset(U"big")(U"Albus Box").drawAt(Scene::Width() / 2, 350, font_color);

		FontAsset(U"middle")(U"Version: {:>24}\nRelease Number: {:>8}"_fmt(VERSION, RELEASE_NUM)).drawAt(Scene::Width() / 2, 450, font_color);

		FontAsset(U"small")(U"©YotioSoft 2021 | Powered by OpenSiv3D").drawAt(Scene::Width() / 2, Scene::Height()-50, font_color);

		// 再生処理を継続
		player.playing();

#if defined(_WIN32) || defined(_WIN64)
		// ウィンドウの移動
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, dpi_update);
		}

		dpi_update = !dpi_update;
#endif
    }
        
	return true;
}

// 歌詞設定画面
struct lyrics_with_timestamp {
	LyricsElement lyrics;
	Timestamp time_begin;
	Timestamp time_end;
};
bool lyricsSetting(Player& player, Color& button_close_color, Texture& window_close_icon, Color& font_color)
{
	// 自動で曲の遷移を無効化
	player.setAutoMoveToNext(false);

	// ボタン用アイコン
	Texture return_button_icon{ Icon(IconFont::Return), 20 };		// 戻る
	Texture fileopen_icon{ Icon(IconFont::Plus), 20 };				// ファイルを追加

	Texture play_icon{ Icon(IconFont::Play), 20 };			// 再生
	Texture stop_icon{ Icon(IconFont::Pause), 20 };			// 停止

	// ボタンの位置
	Point return_button_pos;
	return_button_pos = Point(10, 10);
	Point fileopen_button_pos = Point(Scene::Width() - 50, Scene::Height() - 50);
	Point listopen_button_pos = Point(Scene::Width() / 2 - 30, 70);
	Point save_button_pos = Point(Scene::Width() / 2 + 30, 70);

	// シークバー
	double play_pos = 0.0;		// シークバーの初期値
	int slider_width = Scene::Width() - 40 - 100 - 30;
	NeumorphismUI::Slider slider(play_pos, Vec2{ 0, 0 }, slider_width, 20);
    slider.setPosition(Point(120, 100));

	// リストビューを作成
	pair<Array<String>, Array<bool>> title_list = player.getTitleList();

	// リスト表示用RenderTexture
	RenderTexture listview_texture(Scene::Width(), Scene::Height() - 100, Color(DEFAULT_BACKGROUND_COLOR));
	RenderTexture title_texture(250, 40, Color(DEFAULT_BACKGROUND_COLOR));
	Mat3x2 mat, mat_mouse, mat_title;
	double scroll_y = 0.0, scroll_y_before = 0.0;
	const int list_element_h = 80;
	const int list_element_margin = list_element_h + 10;

	int target_audio = -1;

	// マウスクリックした地点の記録用
	Point mouse_clicked;
	bool window_moving = false;

	bool onAnyButton, dpi_update = true;
	bool onAddButton = false;

	// 歌詞オブジェクトを取得
	Lyrics* lyrics_obj = player.getLyricsObj();
	// 歌詞リストを取得
	Array<struct lyrics_with_timestamp> lyrics_list;
	if (lyrics_obj != nullptr) {
		for (LyricsElement lyrics_el : lyrics_obj->get_lyrics_list()) {
			Timestamp begin = player.convertToTimestamp(lyrics_el.begin);
			Timestamp end = player.convertToTimestamp(lyrics_el.end);

			struct lyrics_with_timestamp lwt = { lyrics_el, begin, end };
			lyrics_list << lwt;
		}
	}

	// 編集中の歌詞カードのインデックス（-1: 未指定）
	int editing_lyrics_card_num = -1;

	// 入力ボックス用
	TextEditState tes_lyrics;
	TextEditState tes_begin_min, tes_begin_sec;
	TextEditState tes_end_min, tes_end_sec;

	while (System::Update()) {
		onAnyButton = false;

		// 画面上部のボタン群
		// 閉じるボタン
		if (ExitButton(button_close_color, window_close_icon, onAnyButton)) {
			// 自動で曲の遷移を有効化
			player.setAutoMoveToNext(true);

			return true;
		}
		// もどる
		if (NeumorphismUI::RectButton(return_button_pos, Vec2(40, 40), return_button_icon, onAnyButton)) {
			// 自動で曲の遷移を有効化
			player.setAutoMoveToNext(true);

			return false;
		}

		// 画面タイトル
		FontAsset(U"title")(U"歌詞設定").draw(Arg::center(Scene::Width() / 2, 30), Color(font_color));
        
        // 再生ボタン、シークバー、タイトルを表示
        bool playing = player.playing();
        NeumorphismUI::CircleSwitch(Vec2(70, 110), 30, playing, play_icon, stop_icon, onAnyButton);
        
        if (!slider.isSliderMoving()) {
            slider.setValueNoAnimetion(player.getPlayPosNorm());
        }
        play_pos = slider.draw(onAnyButton);
        if (slider.isSliderLeftReleased()) {
            player.seekTo(play_pos);
        }

        if (playing != player.playing()) {
            if (playing) {
                player.play();
            }
            else {
                player.pause();
            }
        }

        // タイトル
		DrawableText title_text = FontAsset(U"middle")(player.getTitle());
		title_text.draw(120, 70, font_color);

		// 再生位置を表示
		Timestamp now = player.getPlayPosTime();
		Timestamp total = player.getTotalTime();
		FontAsset(U"small")(U"{}:{:0>2} / {}:{:0>2}"_fmt(now.min, now.sec, total.min, total.sec)).draw(120, 125, Color(font_color));

		// リストを表示
		scroll_y += Mouse::Wheel() * 50;
		if (scroll_y < 0.0) {
			scroll_y = 0.0;
		}
		if (scroll_y > list_element_margin * (lyrics_list.size() - 1)) {
			scroll_y = scroll_y_before;
		}
		scroll_y_before = scroll_y;

		//NeumorphismUI::NeumorphismRect(20, 80, Scene::Width()-40, Scene::Height()-80-20, true);
		mat = Mat3x2::Translate(0, 10 - scroll_y);
		mat_mouse = Mat3x2::Translate(0, 150 - scroll_y);
		{
			const ScopedRenderTarget2D target(listview_texture);
			listview_texture.clear(DEFAULT_BACKGROUND_COLOR);

			const Transformer2D t(mat, mat_mouse);

			for (int i = 0; i < lyrics_list.size(); i++) {
				// 土台
				Rect background_rect(20, list_element_margin * i, Scene::Width() - 40, list_element_h);
				if (!onAddButton && background_rect.mouseOver() && editing_lyrics_card_num != i) {
					NeumorphismUI::NeumorphismRect(20, list_element_margin * i, Scene::Width() - 40, list_element_h, false, Color(250, 250, 250));
					Cursor::RequestStyle(CursorStyle::Hand);

					// クリックされたら編集中の歌詞に指定
					if (background_rect.leftClicked()) {
						editing_lyrics_card_num = i;

						// 値の代入
						tes_begin_min.text = Format(lyrics_list[editing_lyrics_card_num].time_begin.min);
						tes_begin_sec.text = Format(lyrics_list[editing_lyrics_card_num].time_begin.sec);
						tes_end_min.text = Format(lyrics_list[editing_lyrics_card_num].time_end.min);
						tes_end_sec.text = Format(lyrics_list[editing_lyrics_card_num].time_end.sec);
						tes_lyrics.text = lyrics_list[editing_lyrics_card_num].lyrics.lyrics;
					}
				}
				else {
					NeumorphismUI::NeumorphismRect(20, list_element_margin * i, Scene::Width() - 40, list_element_h, false);
				}

				// 編集中の場合
				if (editing_lyrics_card_num == i) {
					// 歌詞の有効範囲の設定項目
					SimpleGUI::TextBox(tes_begin_min, Vec2(50, list_element_margin * i), 40);
					FontAsset(U"middle")(U":").draw(Arg::center(50 + 40*1 + 5*1, list_element_margin * i + 15), Color(font_color));
					SimpleGUI::TextBox(tes_begin_sec, Vec2(50 + 40*1 + 5*2, list_element_margin * i), 40);

					FontAsset(U"middle")(U"～").draw(Arg::center(50 + 40*2 + 5*2 + 15*1, list_element_margin * i + 15), Color(font_color));

					SimpleGUI::TextBox(tes_end_min, Vec2(50 + 40*2 + 5*2 + 15*2, list_element_margin * i), 40);
					FontAsset(U"middle")(U":").draw(Arg::center(50 + 40*3 + 5*3 + 15*2, list_element_margin * i + 15), Color(font_color));
					SimpleGUI::TextBox(tes_end_sec, Vec2(50 + 40*3 + 5*4 + 15*2, list_element_margin * i), 40);

					// 歌詞の設定
					SimpleGUI::TextBox(tes_lyrics, Vec2(50, list_element_margin * i + 40), background_rect.size.x - 50);

					// 枠外がクリックされたら設定を反映
					if (!background_rect.leftPressed() && MouseL.down()) {
						double begin_double = Parse<double>(tes_begin_min.text) * 60 + Parse<double>(tes_begin_sec.text);
						double end_double = Parse<double>(tes_end_min.text) * 60 + Parse<double>(tes_end_sec.text);
						int new_lyric_index = lyrics_obj->set_lyric(editing_lyrics_card_num, begin_double, end_double, tes_lyrics.text);

						// リストを再取得
						lyrics_list.clear();
						for (LyricsElement lyrics_el : lyrics_obj->get_lyrics_list()) {
							Timestamp begin = player.convertToTimestamp(lyrics_el.begin);
							Timestamp end = player.convertToTimestamp(lyrics_el.end);

							struct lyrics_with_timestamp lwt = { lyrics_el, begin, end };
							lyrics_list << lwt;
						}

						// インデックス番号が変更された場合はスクロール
						if (editing_lyrics_card_num != new_lyric_index) {
							scroll_y = list_element_margin * new_lyric_index;
						}

						// 未編集状態に
						editing_lyrics_card_num = -1;

						break;
					}
				}
				// それ以外
				else {
					// 歌詞の有効範囲の表示
					FontAsset(U"small")(U"{}:{:0>2} ～ {}:{:0>2}"_fmt(lyrics_list[i].time_begin.min, lyrics_list[i].time_begin.sec, lyrics_list[i].time_end.min, lyrics_list[i].time_end.sec)).draw(50, list_element_margin * i + 10, Color(font_color));

					// 歌詞の表示
					FontAsset(U"middle")(lyrics_list[i].lyrics.lyrics).draw(50, list_element_margin * i + 40, Color(font_color));
				}
			}
		}
		listview_texture.draw(0, 150);

		// 歌詞ボタン
		onAddButton = false;
		if (NeumorphismUI::CircleButton(fileopen_button_pos, 30, fileopen_icon, onAddButton)) {
			// 末尾に移動
			scroll_y = list_element_h * lyrics_list.size();

			// 新たな歌詞カードを追加
			editing_lyrics_card_num = lyrics_list.size();

			Timestamp begin = player.getPlayPosTime();
			Timestamp end = player.convertToTimestamp(player.getPlayPosSec() + 3);
			LyricsElement le = { begin.min*60+begin.sec, end.min*60+end.sec, U"" };

			struct lyrics_with_timestamp lwt = { le, begin, end };
			lyrics_list << lwt;

			// 値の代入
			tes_begin_min.text = Format(lyrics_list[editing_lyrics_card_num].time_begin.min);
			tes_begin_sec.text = U"{:0>2}"_fmt(lyrics_list[editing_lyrics_card_num].time_begin.sec);
			tes_end_min.text = Format(lyrics_list[editing_lyrics_card_num].time_end.min);
			tes_end_sec.text = U"{:0>2}"_fmt(lyrics_list[editing_lyrics_card_num].time_end.sec);
			tes_lyrics.text = lyrics_list[editing_lyrics_card_num].lyrics.lyrics;
		}
		onAnyButton |= onAddButton;

		// 再生処理を継続
		player.playing();

		// ウィンドウの移動
#if defined(_WIN32) || defined(_WIN64)
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, dpi_update);
		}

		dpi_update = !dpi_update;
#endif
	}

	// 自動で曲の遷移を有効化
	player.setAutoMoveToNext(true);

	return true;
}

// 設定画面
bool AlbusBoxSetting(Player& player, Color& button_close_color, Texture& window_close_icon, Color& font_color) {
	// フォント
	Font button_font{ 13, Typeface::CJK_Regular_JP };

	// ボタン用アイコン
	Texture return_button_icon{ Icon(IconFont::Return), 20 };		// 戻る
	
	// アイコン
	Texture volume_icon{ Icon(IconFont::Volume), 30 };
	Texture info_icon{ Icon(IconFont::Information), 30 };
	
	// アイコンの位置
	const int icon_left_x = 50;
	const int icon_top_y = 100;
	
	// 音量調整バー
	double volume = player.getVolume();
	double volume_before = volume;
	NeumorphismUI::Slider volume_bar(volume, Vec2{ icon_left_x+100, icon_top_y }, Scene::Width()-icon_left_x*2-100, 30);

	// スイッチ
	// 波形の表示
	NeumorphismUI::SlideSwitch show_wave_switch(player.isShowWaveEnabled(), Vec2(Scene::Width() - icon_left_x - 50, icon_top_y + 100), Vec2(50, 30));
	// ループ再生
	NeumorphismUI::SlideSwitch loop_switch(player.isLoopEnabled(), Vec2(Scene::Width() - icon_left_x - 50, icon_top_y + 180), Vec2(50, 30));
	
	// ボタンの位置
	Point return_button_pos;
	return_button_pos = Point(10, 10);
	
	// マウスクリックした地点の記録用
	Point mouse_clicked;
	bool window_moving = false;
	
	bool onAnyButton, dpi_update = true;
	
	while (System::Update()) {
		onAnyButton = false;
		
		// 画面上部のボタン群
		// 閉じるボタン
		if (ExitButton(button_close_color, window_close_icon, onAnyButton)) {
			return true;
		}
		// もどる
		if (NeumorphismUI::RectButton(return_button_pos, Vec2(40, 40), return_button_icon, onAnyButton)) {
			player.saveSettings();
			return false;
		}
		
		// 画面タイトル
		FontAsset(U"title")(U"設定").draw(Arg::center(Scene::Width()/2, 30), Color(font_color));
		
		// 各設定項目
		// 音量
		volume_icon.draw(icon_left_x, icon_top_y, Color(font_color));
		volume = volume_bar.draw(onAnyButton);
		if (volume != volume_before) {
			volume_before = volume;
			player.changeVolumeTo(volume);
		}

		// 波形の表示
		FontAsset(U"middle")(U"波形の表示").draw(icon_left_x, icon_top_y + 100, font_color);
		player.setShowWave(show_wave_switch.draw());

		// ループ再生
		FontAsset(U"middle")(U"ループ再生").draw(icon_left_x, icon_top_y + 180, font_color);
		player.setLoop(loop_switch.draw());

		// 歌詞設定画面
		if (NeumorphismUI::RectButton(Vec2(icon_left_x, icon_top_y + 260), Vec2(150, 50), U"歌詞設定", button_font, onAnyButton)) {
			if (lyricsSetting(player, button_close_color, window_close_icon, font_color)) {
				return true;
			}
		}

		// バージョン情報
		if (NeumorphismUI::RectButton(Vec2(Scene::Width() / 2 - 50/2, Scene::Height() - 100), Vec2(50, 50), info_icon, onAnyButton)) {
			if (VersionInformation(player, button_close_color, window_close_icon, font_color)) {
				return true;
			}
		}

		// 再生処理を継続
		player.playing();
		
#if defined(_WIN32) || defined(_WIN64)
		// ウィンドウの移動
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, dpi_update);
		}

		dpi_update = !dpi_update;
#endif
	}
	return true;
}

void drawThumbnailTexture(Player& player, int thumbnail_size) {
	Texture *thumbnail_texture = player.getThumbnailTexture();

	if (thumbnail_texture->width() > thumbnail_texture->height()) {
		thumbnail_texture->resized(Size(thumbnail_texture->width() / thumbnail_texture->height() * thumbnail_size, thumbnail_size)).draw(0, 0);
	}
	else {
		thumbnail_texture->resized(Size(thumbnail_size, thumbnail_texture->height() / thumbnail_texture->width() * thumbnail_size)).draw(0, 0);
	}
}

void AlbusBox() {
	Scene::SetBackground(DEFAULT_BACKGROUND_COLOR);
    Window::Resize(400, 640);
    
#if defined(_WIN32) || defined(_WIN64)
    Window::SetStyle(WindowStyle::Frameless);
    specific::setWindowStyle(0, 0, 400, 640, 40, 40, true);
#else
    specific::setWindowStyle(0, 0, 400, 640, 40, 40);
#endif

	// ウィンドウタイトル
	Window::SetTitle(TITLE);

	// フォントアセットの登録
	FontAsset::Register(U"small", 13, Typeface::CJK_Regular_JP);
	FontAsset::Register(U"middle", 16, Typeface::CJK_Regular_JP);
	FontAsset::Register(U"big", 36, Typeface::CJK_Regular_JP);
	FontAsset::Register(U"title", 16, Typeface::Mplus_Bold);
	FontAsset::Register(U"icon_font", 16, Typeface::Icon_MaterialDesign);

	// プレイヤーの用意
	Player player;

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
	Texture play_icon{ Icon(IconFont::Play), 30 };
	Texture pause_icon{ Icon(IconFont::Pause), 30 };

	Texture prev_icon{ Icon(IconFont::Prev), 20 };
	Texture next_icon{ Icon(IconFont::Next), 20 };

	Texture setting_icon{ Icon(IconFont::Setting), 20 };
	Texture fileopen_icon{ Icon(IconFont::FileOpen), 20 };
	Texture playlist_icon{ Icon(IconFont::PlayList), 20 };

	Texture window_close_icon{ Icon(IconFont::Times), 20 };
	
	// ボタンの位置
	Point setting_button_pos;
	Point fileopen_button_pos;
	Point playlist_button_pos;
	setting_button_pos = Point(30, 30);
	fileopen_button_pos = Point(80, 30);
	playlist_button_pos = Point(130, 30);

	// ボタン押下検知用
	bool prev_button_pressed = false;
	bool next_button_pressed = false;

	// 色
	Color font_color(Palette::Gray);
	Color button_close_color(Color(242, 38, 19));

	// サムネ用
	const int thumbnail_size = 260;
	RenderTexture thumbnail_texture(player.getDefdaultThumbnailImage());
	Circle thumbnail_circle(Scene::Width() / 2, Scene::Height() / 3, thumbnail_size / 2);

	// 歌詞用
	Rect lyrics_rect;
	String lyrics_str;
	Font lyrics_font{ 14, Typeface::CJK_Regular_JP };

	// 高速フーリエ変換用
	FFTResult fft;

	// 座標変換行列（タイトル用）
	Mat3x2 mat = Mat3x2::Identity();
	RectF title_rect, title_rect_before;
	int count_for_music = 0;

	// タイトル編集用
	TextEditState tes_title;
	bool editing_title = false;
	
	// カーソルが何らかのボタンオブジェクト上にあるか
	bool onAnyButton;
	
	pair<bool, FilePath> file_open;
    
    // サムネイルFFT関連
    const int fft_size = 800;
    const int box_size = 10;
    const int box_half_size = box_size / 2;
    const int row_boxes = 20;
    int i, j;
    const double fft_size_per_row_boxes = (double)fft_size / row_boxes;
    int fft_box_count;
    const int row_box_x_const = thumbnail_texture.width() / row_boxes;
    const int thumbnail_height = thumbnail_texture.height();
    const int row_box_wh = box_size * 3 / 4;
	const int thumbnail_half_wh = thumbnail_height / 2;

	bool fft_update = true;
	while (System::Update()) {
		onAnyButton = false;
		
		// 画面上部のボタン群
		bool isExitButtonPushed = ExitButton(button_close_color, window_close_icon, onAnyButton);
		if (isExitButtonPushed)
			break;

		// 画面上部のボタン群
		// 設定へ画面遷移
		if (NeumorphismUI::CircleButton(setting_button_pos, 20, setting_icon, onAnyButton)) {
			if (AlbusBoxSetting(player, button_close_color, window_close_icon, font_color)) {
				break;		// 閉じるボタンが押されたらループを抜ける
			}
		}
		
		// 再生リスト
		if (NeumorphismUI::CircleButton(playlist_button_pos, 20, playlist_icon, onAnyButton)) {
			if (playListView(player, button_close_color, window_close_icon, font_color)) {
				break;		// 閉じるボタンが押されたらループを抜ける
			}
		}

		// ファイルを開く
		if (file_open.first) {
			if (FileSystem::Extension(file_open.second) == U"playlist") {
				player.loadPlayList(file_open.second);
			}
			else {
				player.openAndPlay(file_open.second);
			}

			file_open.first = false;
		}
		if (NeumorphismUI::CircleButton(fileopen_button_pos, 20, fileopen_icon, onAnyButton)) {
			file_open = AudioFileOpen();

			if (file_open.first) {
				FontAsset(U"middle")(U"Loading...").drawAt(Scene::Width() / 2, Scene::Height() / 3 + 170, font_color);
			}
		}

		// サムネイル
		if (NeumorphismUI::NeumorphismCircle(Scene::Width() / 2, Scene::Height() / 3, thumbnail_size / 2 + 10, false)) {
			if (player.isOpened()) {
				Cursor::RequestStyle(CursorStyle::Hand);

				if (MouseL.down()) {
					pair<bool, FilePath> image_file_open = ImageFileOpen();
					if (image_file_open.first) {
						player.setThumbnailImage(image_file_open.second);
					}
				}
			}
		}

		// 波形を表示（FFT:高速フーリエ変換）
		if (playing && player.isOpened() && player.isShowWaveEnabled()) {
            if (fft_update) {
                ScopedRenderTarget2D target(thumbnail_texture);
                player.getThumbnailTexture()->drawAt(thumbnail_size / 2, thumbnail_size / 2);

                // FFT更新
                player.fft(fft);
                
				// 波形の描画
                for (i = 0; i < row_boxes; i++) {
                    double size = Pow(fft.buffer[i * fft_size_per_row_boxes], 0.6f) * 2000;
                    fft_box_count = size / box_size;
                    for (j = 0; j < fft_box_count; j++) {
                        Rect(Arg::center(i * row_box_x_const + box_half_size, thumbnail_height - j * box_size - box_half_size), row_box_wh, row_box_wh).draw(Color(200, 200, 200, 200));
                    }
                }

				// 歌詞を表示
				if (player.lyricsExist()) {
					if (player.updateLyrics()) {
						lyrics_str = player.getLyrics();
						RectF region = lyrics_font(lyrics_str).region(0, 0);
						if (region.w > thumbnail_size) {
							lyrics_rect = Rect(Arg::center(thumbnail_half_wh, thumbnail_half_wh), thumbnail_size, region.w / thumbnail_size * lyrics_font.fontSize());
						}
						else {
							lyrics_rect = Rect(Arg::center(thumbnail_half_wh, thumbnail_half_wh), region.w + 2, region.h + 2);
						}
					}
					//lyrics_circle(lyrics_font.getTexture()).draw();
					if (lyrics_str.size() > 0) {
						//lyrics_rect.draw(Palette::Black);
						int lyrics_display_count = player.getLyricsDisplayAlphaColor();
						lyrics_font(lyrics_str).draw(lyrics_rect, Color(255, 255, 255, lyrics_display_count));
					}
				}

				// マウスオーバー時の表示
                if (thumbnail_circle.mouseOver()) {
                    Rect(0, 0, thumbnail_size, thumbnail_size).draw(Color(0, 0, 0, 127));
                    FontAsset(U"small")(U"クリックでサムネイル画像変更").draw(Arg::center(thumbnail_size / 2, thumbnail_size / 2), Color(Palette::White));
                }
            }
		}
		else {
			ScopedRenderTarget2D target(thumbnail_texture);
			player.getThumbnailTexture()->drawAt(thumbnail_size / 2, thumbnail_size / 2);
		}

		thumbnail_circle(thumbnail_texture(0, 0, thumbnail_size, thumbnail_size)).draw();

		// タイトル
		// タイトル部分のマウスオーバー時にタイトル部分を光らせる
		if (title_rect.mouseOver() && Cursor::OnClientRect()) {
			title_rect.draw(Color(255, 255, 255, 100));
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		// タイトルの表示
		if (!file_open.first) {	// "Loading..."を表示していないときだけ
			title_rect = FontAsset(U"middle")(player.getTitle()).region(Arg::center(Scene::Width() / 2, Scene::Height() / 3 + 170));
			if (title_rect != title_rect_before) {
				title_rect_before = title_rect;
				count_for_music = Scene::Width();
			}
			if (title_rect.w > Scene::Width() && !editing_title) {
				mat = Mat3x2::Translate((title_rect.w - Scene::Width()) / 2 - (count_for_music % ((int)title_rect.w + Scene::Width())) + Scene::Width(), 0);
				{
					const Transformer2D t(mat, TransformCursor::No);
					FontAsset(U"middle")(player.getTitle()).drawAt(Scene::Width() / 2, Scene::Height() / 3 + 170, font_color);
				}
			}
			else if (!editing_title) {
				FontAsset(U"middle")(player.getTitle()).drawAt(Scene::Width() / 2, Scene::Height() / 3 + 170, font_color);
			}
		}
		count_for_music++;

		// タイトル部分がクリックされたらテキストボックスを設置
		if (title_rect.leftClicked() && !editing_title && Cursor::OnClientRect()) {
			editing_title = true;
			tes_title.text = player.getTitle();
		}
		if (editing_title) {
			SimpleGUI::TextBox(tes_title, Vec2(0, Scene::Height() / 3 + 170 - 10), Scene::Width());
			
			title_rect = RectF(0, Scene::Height() / 3 + 170 - 10, Scene::Width(), 40);

			// editing_title == trueかつタイトル部分以外でクリックされたら編集完了
			if (!title_rect.leftClicked() && MouseL.down()/* && Cursor::GetRequestedStyle() != CursorStyle::IBeam */) {
				editing_title = false;
				player.editTitle(tes_title.text);
			}

			if (title_rect.mouseOver()) {
				onAnyButton = true;
			}
		}
		// editing_title == trueかつタイトル部分以外でクリックされたら編集完了
		if (editing_title && !title_rect.leftClicked() && MouseL.down()) {
			editing_title = false;
		}

		// シークバー
		if (!slider.isSliderMoving()) {
			slider.setValueNoAnimetion(player.getPlayPosNorm());
			//cout << (double)audio_file.posSample()/audio_file.samples() << endl;
		}
		play_pos = slider.draw(onAnyButton);
		if (slider.isSliderLeftReleased()) {
			player.seekTo(play_pos);
		}

		// 再生位置表示
		Timestamp current_time = player.getPlayPosTime();
		Timestamp length_time = player.getTotalTime();
		FontAsset(U"small")(U"{}:{:0>2}"_fmt(current_time.min, current_time.sec)).draw(40, Scene::Height() / 2 + 140, font_color);
		FontAsset(U"small")(U"{}:{:0>2}"_fmt(length_time.min, length_time.sec)).draw(Arg::topRight = Point(Scene::Width() - 40, Scene::Height() / 2 + 140), font_color);

		// トラック移動ボタン
		prev_button_pressed = NeumorphismUI::CircleButton(Scene::Width() / 4 - 10, Scene::Height() - 100, 30, prev_icon, onAnyButton);
		next_button_pressed = NeumorphismUI::CircleButton(Scene::Width() - Scene::Width() / 4 + 10, Scene::Height() - 100, 30, next_icon, onAnyButton);
		
		if (prev_button_pressed)
			player.previous();
		if (next_button_pressed)
			player.next();

		// 再生/一時停止ボタン
		NeumorphismUI::CircleSwitch(Scene::Width() / 2, Scene::Height() - 100, 40, playing, play_icon, pause_icon, onAnyButton);
		if (before_playing != playing) {
			if (playing)
				player.play();
			else
				player.pause();
			
			before_playing = playing;
		}
		
		playing = player.playing();
		
#if defined(_WIN32) || defined(_WIN64)
		// ウィンドウの移動
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, fft_update);
		}
#endif

		fft_update = !fft_update;
	}
	
	player.free();
	player.saveSettings();
}
