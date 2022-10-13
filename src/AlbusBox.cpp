#include "AlbusBox.hpp"

bool pin_window = false;

// 音声ファイルを開く
pair<bool, FilePath> AudioFileOpen() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = { {U"音声ファイルとプレイリスト", {U"mp3", U"ogg", U"wav", U"m4a", U"playlist"}} };
	String filePathTemp;
	if (const auto open = Dialog::OpenFile(ff)) {
		return pair<bool, FilePath>(true, open.value());
	}

	return pair<bool, FilePath>(false, FilePath());
}

// プレイリストファイルを開く
pair<bool, FilePath> OpenPlayList() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = { {U"プレイリスト", {U"playlist"}} };
	String filePathTemp;
	if (const auto open = Dialog::OpenFile(ff)) {
		return pair<bool, FilePath>(true, open.value());
	}

	return pair<bool, FilePath>(false, FilePath());
}

// 画像ファイルを開く
pair<bool, FilePath> ImageFileOpen() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = { {U"画像ファイル", {U"png", U"jpg", U"jpeg", U"bmp", U"gif", U"tga", U"ppm", U"pgm", U"pbm", U"pnm", U"webp"}} };
	String filePathTemp;
	if (const auto open = Dialog::OpenFile(ff)) {
		return pair<bool, FilePath>(true, open.value());
	}

	return pair<bool, FilePath>(false, FilePath());
}

// 閉じるボタンの表示
bool ExitButton(Color& button_close_color, Texture& window_close_icon, bool& onAnyButton, bool enableButtons) {
#if defined(_WIN32) || defined(_WIN64)
		// 最前面表示ボタン
		Font small_font = FontAsset(U"icon_font");
		if (pin_window) {
			NeumorphismUI::CircleSwitch(Scene::Width() - 70, 30, 15, pin_window, U"\U000F0404", small_font, onAnyButton, enableButtons);
		}
		else {
			NeumorphismUI::CircleSwitch(Scene::Width() - 70, 30, 15, pin_window, U"\U000F0403", small_font, onAnyButton, enableButtons);
		}
		specific::pinWindow(pin_window);

		// ウィンドウを閉じるボタンが押されたとき
		if (NeumorphismUI::CircleButton(Scene::Width()-30, 30, 15, window_close_icon, onAnyButton, enableButtons)) {
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

	int count_for_music = title_texture.width();

	while (System::Update()) {
		onAnyButton = false;
		
		title_list = player.getTitleList();
		before_playing = title_list.second;

		bool buttons_enable = !player.willBeLoading();

		// 画面上部のボタン群
		// 閉じるボタン
		if (ExitButton(button_close_color, window_close_icon, onAnyButton, buttons_enable)) {
			return true;
		}
		// もどる
		if (NeumorphismUI::RectButton(return_button_pos, Vec2(40, 40), return_button_icon, onAnyButton, buttons_enable)) {
			return false;
		}

		// リストを開くボタン
		if (NeumorphismUI::CircleButton(listopen_button_pos, 20, listopen_button_icon, onAnyButton, buttons_enable)) {
			pair<bool, FilePath> playlist = OpenPlayList();
			if (playlist.first) {
				if (!player.loadPlayList(playlist.second)) {
					System::MessageBoxOK(U"プレイリストの読み込みに失敗しました。\n音声ファイルが読み込めません。");
				}
			}
		}
		// 保存ボタン
		if (NeumorphismUI::CircleButton(save_button_pos, 20, save_button_icon, onAnyButton, buttons_enable)) {
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
				bool button_enable = buttons_enable;
				if (110 - scroll_y + list_element_margin * i + list_element_h / 2 < 90) {
					button_enable = false;
				}
				if (Cursor::Pos().y + 10 - scroll_y < 0) {
					button_enable = false;
					//mouse_on_top_of_window = true;
				}
				if (Cursor::Pos().y + 10 - scroll_y > listview_texture.height()) {
					button_enable = false;
					//mouse_on_top_of_window = true;
				}

				// 再生・停止ボタン
				if (title_list.second[i]) {	// 再生中
					NeumorphismUI::CircleSwitch(Vec2(70, list_element_margin * i + list_element_h/2), 25, title_list.second[i], stop_icon, onAnyButton, button_enable);
					
					if (target_audio != i) {
						slider.setPosition(Point(120, list_element_margin * i + 50));
						target_audio = i;

						count_for_music = title_texture.width();
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
						int title_w = title_text.region(0, 0).w;
						if (title_w > title_texture.width()) {
							mat = Mat3x2::Translate(- (count_for_music % ((int)title_w + title_texture.width())) + title_texture.width(), 0);
							{
								const Transformer2D t(mat, TransformCursor::No);
								title_text.draw(0, 0, font_color);
							}
						}
						else {
							title_text.draw(0, 0, font_color);
						}
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
		if (NeumorphismUI::CircleButton(fileopen_button_pos, 30, fileopen_icon, onAnyButton, buttons_enable)) {
			auto file_open = AudioFileOpen();
			if (file_open.first) {
				player.audioRegister(file_open.second);
			}
		}

		// ウィンドウの移動
#if defined(_WIN32) || defined(_WIN64)
		if (buttons_enable && !onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, dpi_update);
		}

		dpi_update = !dpi_update;
#endif

		// 再生処理を継続
		player.playing();

		count_for_music++;
    }
    
	return true;
}

// バージョン情報画面
bool VersionInformation(Player& player, Color& button_close_color, Texture& window_close_icon, Color& font_color) {
	// ボタン用アイコン
	Texture return_button_icon{ Icon(IconFont::Return), 20 };		// 戻る

	// 画像
	Texture albus_box_icon(U"{}/data/img/icon.png"_fmt(specific::getCurrentDir()));
	Texture albus_box_title(U"{}/data/img/title.svg"_fmt(specific::getCurrentDir()));

	// ボタンの位置
	Point return_button_pos;
	return_button_pos = Point(10, 10);

	// マウスクリックした地点の記録用
	Point mouse_clicked;
	bool window_moving = false;
	
	bool onAnyButton, dpi_update = true;
	
	while (System::Update()) {
		onAnyButton = false;

		bool buttons_enable = !player.willBeLoading();
		
		// 画面上部のボタン群
		// 閉じるボタン
		if (ExitButton(button_close_color, window_close_icon, onAnyButton, buttons_enable)) {
			return true;
		}
		// もどる
		if (NeumorphismUI::RectButton(return_button_pos, Vec2(40, 40), return_button_icon, onAnyButton, buttons_enable)) {
			return false;
		}

		// 画面タイトル
		FontAsset(U"title")(U"バージョン情報").draw(Arg::center(Scene::Width() / 2, 30), Color(font_color));

		// バージョン情報
		albus_box_icon.resized(200, 200).drawAt(Scene::Width() / 2, 200);
		albus_box_title.resized(200, 37).drawAt(Scene::Width() / 2, 350);

		FontAsset(U"middle")(U"Version: {:>24}\nRelease Number: {:>8}"_fmt(VERSION, RELEASE_NUM)).drawAt(Scene::Width() / 2, 450, font_color);

		FontAsset(U"small")(U"© YotioSoft 2021-2022 | Developed with OpenSiv3D").drawAt(Scene::Width() / 2, Scene::Height()-50, font_color);

#if defined(_WIN32) || defined(_WIN64)
		// ウィンドウの移動
		if (!onAnyButton && buttons_enable) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, dpi_update);
		}

		dpi_update = !dpi_update;
#endif

		// 再生処理を継続
		player.playing();
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
	Point add_lyric_card_button_pos = Point(Scene::Width() - 50, Scene::Height() - 50);

	// シークバー
	double play_pos = 0.0;		// シークバーの初期値
	int slider_width = Scene::Width() - 40 - 100 - 30;
	NeumorphismUI::Slider slider(play_pos, Vec2{ 0, 0 }, slider_width, 20);
    slider.setPosition(Point(120, 100));

	// リストビューを作成
	pair<Array<String>, Array<bool>> title_list = player.getTitleList();

	// リスト表示用RenderTexture
	RenderTexture listview_texture(Scene::Width(), Scene::Height() - 150, Color(DEFAULT_BACKGROUND_COLOR));
	RenderTexture title_texture(Scene::Width() - 120, 40, Color(DEFAULT_BACKGROUND_COLOR));
	RenderTexture input_texture(Scene::Width() - 40 - 35, 40);
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

	// リストビューのy座標
	const int listview_y = 150;
	const int listview_margin = 10;

	int count_for_music = title_texture.width();

	while (System::Update()) {
		onAnyButton = false;

		// 画面上部のボタン群
		// 閉じるボタン (常に有効; この画面内ではプレイヤーによる自動読み込みがないため)
		if (ExitButton(button_close_color, window_close_icon, onAnyButton, true)) {
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

		// 歌詞カード操作無効エリア
		Rect no_cards_area(0, 0, Scene::Width(), 150);

		// 画面タイトル
		FontAsset(U"title")(U"歌詞設定").draw(Arg::center(Scene::Width() / 2, 30), Color(font_color));

		// タイトル
		DrawableText title_text = FontAsset(U"middle")(player.getTitle());
		int title_w = title_text.region(120, 70).w;
		int title_area_w = title_texture.width();
		{
			const ScopedRenderTarget2D target(title_texture);
			title_texture.clear(Color(DEFAULT_BACKGROUND_COLOR));
			if (title_area_w < title_w) {
				mat = Mat3x2::Translate(-(count_for_music % ((int)title_w + title_area_w)) + title_area_w, 0);
				{
					const Transformer2D t(mat, TransformCursor::No);
					title_text.draw(0, 0, font_color);
				}
			}
			else {
				title_text.draw(0, 0, font_color);
			}
		}
		title_texture.draw(120, 70);
        
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

		// 再生位置を表示
		Timestamp now = player.getPlayPosTime();
		Timestamp total = player.getTotalTime();
		FontAsset(U"small")(U"{}:{:0>2} / {}:{:0>2}"_fmt(now.min, now.sec, total.min, total.sec)).draw(120, 125, Color(font_color));

		// リスト外にマウスカーソルがある場合はリスト上への操作は無効
		bool list_area_mouse_desable = no_cards_area.mouseOver();

		// リストを表示
		scroll_y += Mouse::Wheel() * 50;
		if (scroll_y < 0.0) {
			scroll_y = 0.0;
		}
		if (scroll_y > listview_margin + list_element_margin * (lyrics_list.size() - 1)) {
			scroll_y = scroll_y_before;
		}
		scroll_y_before = scroll_y;

		//NeumorphismUI::NeumorphismRect(20, 80, Scene::Width()-40, Scene::Height()-80-20, true);
		mat = Mat3x2::Translate(0, -scroll_y);
		mat_mouse = Mat3x2::Translate(0, listview_y - scroll_y);
		{
			const ScopedRenderTarget2D target(listview_texture);
			listview_texture.clear(DEFAULT_BACKGROUND_COLOR);

			const Transformer2D t(mat, mat_mouse);

			for (int i = 0; i < lyrics_list.size(); i++) {
				// ボタンの有効・無効
				bool button_enable = true;
				bool mouse_on_top_of_window = false;
				if (listview_margin + listview_y - scroll_y + list_element_margin * i + list_element_h / 2 < 150 - 40) {
					button_enable = false;
				}
				if (Cursor::Pos().y - scroll_y < 0) {
					button_enable = false;
					mouse_on_top_of_window = true;
				}
				if (listview_margin + listview_y - scroll_y + list_element_margin * i > Scene::Height()) {
					button_enable = false;
				}
				if (Cursor::Pos().y - scroll_y > listview_texture.height()) {
					button_enable = false;
					mouse_on_top_of_window = true;
				}
				
				// 土台
				Rect background_rect(20, listview_margin + list_element_margin * i, Scene::Width() - 40, list_element_h);
				if (!list_area_mouse_desable && button_enable && !onAddButton && background_rect.mouseOver() && editing_lyrics_card_num != i) {
					NeumorphismUI::NeumorphismRect(20, listview_margin + list_element_margin * i, Scene::Width() - 40, list_element_h, false, Color(250, 250, 250));
					Cursor::RequestStyle(CursorStyle::Hand);

					// クリックされたら編集中の歌詞に指定
					if (background_rect.leftClicked()) {
						editing_lyrics_card_num = i;

						// 値の代入
						tes_begin_min.text = Format(lyrics_list[editing_lyrics_card_num].time_begin.min);
						tes_begin_sec.text = U"{:0>2}"_fmt(Format(lyrics_list[editing_lyrics_card_num].time_begin.sec));
						tes_end_min.text = Format(lyrics_list[editing_lyrics_card_num].time_end.min);
						tes_end_sec.text = U"{:0>2}"_fmt(Format(lyrics_list[editing_lyrics_card_num].time_end.sec));
						tes_lyrics.text = lyrics_list[editing_lyrics_card_num].lyrics.lyrics;
					}
				}
				else {
					NeumorphismUI::NeumorphismRect(20, listview_margin + list_element_margin * i, Scene::Width() - 40, list_element_h, false);
				}

				// 編集中の場合
				if ((button_enable || mouse_on_top_of_window) && editing_lyrics_card_num == i) {
					// 歌詞の有効範囲の設定項目
					if (listview_margin + list_element_margin * i - scroll_y >= -50) {
						SimpleGUI::TextBox(tes_begin_min, Vec2(40, listview_margin + list_element_margin * i), 40);
						FontAsset(U"middle")(U":").draw(Arg::center(40 + 40*1 + 5*1, listview_margin + list_element_margin * i + 15), Color(font_color));
						SimpleGUI::TextBox(tes_begin_sec, Vec2(40 + 40*1 + 5*2, listview_margin + list_element_margin * i), 40);

						FontAsset(U"middle")(U"～").draw(Arg::center(40 + 40*2 + 5*2 + 15*1, listview_margin + list_element_margin * i + 15), Color(font_color));

						SimpleGUI::TextBox(tes_end_min, Vec2(40 + 40*2 + 5*2 + 15*2, listview_margin + list_element_margin * i), 40);
						FontAsset(U"middle")(U":").draw(Arg::center(40 + 40*3 + 5*3 + 15*2, listview_margin + list_element_margin * i + 15), Color(font_color));
						SimpleGUI::TextBox(tes_end_sec, Vec2(40 + 40*3 + 5*4 + 15*2, listview_margin + list_element_margin * i), 40);
					}

					// 歌詞の設定
					if (listview_margin + list_element_margin * i + 40 - scroll_y >= -50) {
						Mat3x2 mat2 = Mat3x2::Translate(0, scroll_y);
						Mat3x2 mat_mouse2 = Mat3x2::Translate(40, listview_margin + list_element_margin * i + 40);
						{
							const ScopedRenderTarget2D target(input_texture);
							input_texture.clear(DEFAULT_BACKGROUND_COLOR);

							const Transformer2D t(mat2, mat_mouse2);
						
							SimpleGUI::TextBox(tes_lyrics, Vec2(2, 2), background_rect.size.x - 40);
						}
						input_texture.draw(40 - 2, listview_margin + list_element_margin * i + 40 - 2);
					}

					// 削除ボタン
					if (listview_margin + list_element_margin * i + 20 + 15 - scroll_y >= -50) {
						// もし、押されたら
						if (NeumorphismUI::CircleButton(Scene::Width() - 40 - 10, listview_margin + list_element_margin * i + 20, 15, window_close_icon, onAddButton)) {
							lyrics_obj->del_lyric(i);

							// リストを再取得
							lyrics_list.clear();
							for (LyricsElement lyrics_el : lyrics_obj->get_lyrics_list()) {
								Timestamp begin = player.convertToTimestamp(lyrics_el.begin);
								Timestamp end = player.convertToTimestamp(lyrics_el.end);

								struct lyrics_with_timestamp lwt = { lyrics_el, begin, end };
								lyrics_list << lwt;
							}

							editing_lyrics_card_num = -1;

							// 保存
							player.saveLyrics();
						}
					}

					// 枠外がクリックされたら設定を反映
					if (!list_area_mouse_desable && !background_rect.leftPressed() && MouseL.down()) {
						// 入力チェック
						if (tes_begin_min.text.length() == 0 || tes_begin_sec.text.length() == 0 || tes_end_min.text.length() == 0 || tes_end_sec.text.length() == 0) {
							System::MessageBoxOK(U"表示時間が未入力です。");
							continue;
						}
						const String r = U".*[^0-9].*";
						if (RegExp(r).fullMatch(tes_begin_min.text) || RegExp(r).fullMatch(tes_begin_sec.text) || RegExp(r).fullMatch(tes_end_min.text) || RegExp(r).fullMatch(tes_end_sec.text)) {
							System::MessageBoxOK(U"表示時間の入力に数字以外が含まれています。");
							continue;
						}

						double begin_double = atof(tes_begin_min.text.toUTF8().c_str()) * 60 + atof(tes_begin_sec.text.toUTF8().c_str());
						double end_double = atof(tes_end_min.text.toUTF8().c_str()) * 60 + atof(tes_end_sec.text.toUTF8().c_str());

						// 歌詞リストに追加
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
							int before_scroll_y = scroll_y;
							scroll_y = listview_margin + list_element_margin * new_lyric_index;

							// 位置修正前の時点で歌詞カードが画面外に出ない場合はスクロールの必要なし
							if (abs(scroll_y - before_scroll_y) < listview_texture.height()) {
								scroll_y = before_scroll_y;
							}
						}

						// 未編集状態に
						editing_lyrics_card_num = -1;

						// 保存
						player.saveLyrics();

						break;
					}
				}
				// それ以外
				else {
					// 歌詞の有効範囲の表示
					FontAsset(U"small")(U"{}:{:0>2} ～ {}:{:0>2}"_fmt(lyrics_list[i].time_begin.min, lyrics_list[i].time_begin.sec, lyrics_list[i].time_end.min, lyrics_list[i].time_end.sec)).draw(50, listview_margin + list_element_margin * i + 10, Color(font_color));

					// 歌詞の表示
					FontAsset(U"middle")(lyrics_list[i].lyrics.lyrics).draw(50, listview_margin + list_element_margin * i + 40, Color(font_color));
				}
			}
		}
		listview_texture.draw(0, listview_y);

		// 歌詞追加ボタン
		onAddButton = false;
		if (NeumorphismUI::CircleButton(add_lyric_card_button_pos, 30, fileopen_icon, onAddButton)) {
			// 押されたとき：
			// 末尾に移動
			int before_scroll_y = scroll_y;
			scroll_y = list_element_h * lyrics_list.size();
			// 位置修正前の時点で歌詞カードが画面外に出ない場合はスクロールの必要なし
			if (abs(scroll_y - before_scroll_y) < listview_texture.height()) {
				scroll_y = before_scroll_y;
			}

			// 新たな歌詞カードを追加
			editing_lyrics_card_num = lyrics_list.size();

			Timestamp begin = player.getPlayPosTime();
			Timestamp end = player.convertToTimestamp(player.getPlayPosSec() + 3);
			LyricsElement le = { (double)begin.min*60+begin.sec, (double)end.min*60+end.sec, U"" };

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

#if defined(_WIN32) || defined(_WIN64)
		// ウィンドウの移動
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, dpi_update);
		}

		dpi_update = !dpi_update;
#endif

		// 再生処理を継続
		player.playing();

		count_for_music++;
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

		bool buttons_enable = !player.willBeLoading();
		
		// 画面上部のボタン群
		// 閉じるボタン
		if (ExitButton(button_close_color, window_close_icon, onAnyButton, buttons_enable)) {
			return true;
		}
		// もどる
		if (NeumorphismUI::RectButton(return_button_pos, Vec2(40, 40), return_button_icon, onAnyButton, buttons_enable)) {
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
		if (player.isOpened()) {
			if (NeumorphismUI::RectButton(Vec2(icon_left_x, Scene::Height() - 100), Vec2(Scene::Width() - 100 - 20 - icon_left_x, 50), U"歌詞設定", button_font, onAnyButton, buttons_enable)) {
				if (lyricsSetting(player, button_close_color, window_close_icon, font_color)) {
					return true;
				}
			}
		}

		// バージョン情報
		if (NeumorphismUI::RectButton(Vec2(Scene::Width()-100, Scene::Height() - 100), Vec2(50, 50), info_icon, onAnyButton, buttons_enable)) {
			if (VersionInformation(player, button_close_color, window_close_icon, font_color)) {
				return true;
			}
		}

#if defined(_WIN32) || defined(_WIN64)
		// ウィンドウの移動
		if (buttons_enable && !onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, dpi_update);
		}

		dpi_update = !dpi_update;
#endif

		// 再生処理を継続
		player.playing();
	}
	return true;
}

// サムネイル画像の描画
void drawThumbnailTexture(Player& player, int thumbnail_size) {
	Texture *thumbnail_texture = player.getThumbnailTexture();

	if (thumbnail_texture->width() > thumbnail_texture->height()) {
		thumbnail_texture->resized(Size(thumbnail_texture->width() / thumbnail_texture->height() * thumbnail_size, thumbnail_size)).draw(0, 0);
	}
	else {
		thumbnail_texture->resized(Size(thumbnail_size, thumbnail_texture->height() / thumbnail_texture->width() * thumbnail_size)).draw(0, 0);
	}
}

// 起動時のウィンドウの初期化
void window_init() {
	Scene::SetBackground(DEFAULT_BACKGROUND_COLOR);
	Window::Resize(WINDOW_WIDTH, WINDOW_HEIGHT);

#if defined(_WIN32) || defined(_WIN64)
	Window::SetStyle(WindowStyle::Frameless);
	specific::setWindowStyle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_ROUNDRECT_RADIUS, WINDOW_ROUNDRECT_RADIUS, true);
#else
	specific::setWindowStyle(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, WINDOW_ROUNDRECT_RADIUS, WINDOW_ROUNDRECT_RADIUS);
#endif

	// ウィンドウタイトル
	Window::SetTitle(TITLE);

	// フォントアセットの登録
	FontAsset::Register(U"small", 13, Typeface::CJK_Regular_JP);
	FontAsset::Register(U"middle", 16, Typeface::CJK_Regular_JP);
	FontAsset::Register(U"big", 36, Typeface::CJK_Regular_JP);
	FontAsset::Register(U"title", 16, Typeface::Mplus_Bold);
	FontAsset::Register(U"icon_font", 16, Typeface::Icon_MaterialDesign);
}

void AlbusBox() {
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
	int count_for_music = Scene::Width() - 50;

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

		bool buttons_enable = !player.willBeLoading() && !file_open.first;

#if defined(_WIN32) || defined(_WIN64)
		// ウィンドウの移動
		if (!onAnyButton && buttons_enable) {
			specific::moveWindow(mouse_clicked, window_moving);
			specific::setWindowStyle(0, 0, 400, 640, 40, 40, fft_update);
		}
#endif
		
		// 画面上部のボタン群
		bool isExitButtonPushed = ExitButton(button_close_color, window_close_icon, onAnyButton, buttons_enable);
		if (isExitButtonPushed)
			break;

		// 画面上部のボタン群
		// 設定へ画面遷移
		if (NeumorphismUI::CircleButton(setting_button_pos, 20, setting_icon, onAnyButton, buttons_enable)) {
			if (AlbusBoxSetting(player, button_close_color, window_close_icon, font_color)) {
				break;		// 閉じるボタンが押されたらループを抜ける
			}
		}
		
		// 再生リスト
		if (NeumorphismUI::CircleButton(playlist_button_pos, 20, playlist_icon, onAnyButton, buttons_enable)) {
			if (playListView(player, button_close_color, window_close_icon, font_color)) {
				break;		// 閉じるボタンが押されたらループを抜ける
			}
		}

		// ファイルを開く
		if (file_open.first) {
			if (FileSystem::Extension(file_open.second) == U"playlist") {
				if (!player.loadPlayList(file_open.second)) {
					System::MessageBoxOK(U"プレイリストの読み込みに失敗しました。\n音声ファイルが読み込めません。");
				}
			}
			else {
				if (!player.openAndPlay(file_open.second)) {
					System::MessageBoxOK(U"音声ファイルが存在しないか壊れています。");
				}
			}

			file_open.first = false;
		}
		if (NeumorphismUI::CircleButton(fileopen_button_pos, 20, fileopen_icon, onAnyButton, buttons_enable)) {
			file_open = AudioFileOpen();

			if (file_open.first) {
				FontAsset(U"middle")(U"Loading...").drawAt(Scene::Width() / 2, Scene::Height() / 3 + 170, font_color);
			}

			buttons_enable = false;
			Cursor::RequestStyle(CursorStyle::Arrow);
		}

		// サムネイル
		if (NeumorphismUI::NeumorphismCircle(Scene::Width() / 2, Scene::Height() / 3, thumbnail_size / 2 + 10, false)) {
			if (player.isOpened() && buttons_enable) {
				Cursor::RequestStyle(CursorStyle::Hand);

				// クリックでサムネイル画像設定
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
						// 枠内に歌詞表示を収める
						lyrics_str = player.getLyrics();
						// 文字列の表示上の長さを取得
						int w = Min((int)lyrics_font(lyrics_str).region(0, 0).w, thumbnail_size - 10) + 5;
						int line_h = (int)lyrics_font(lyrics_str).region(0, 0).h;
						// 枠の生成
						int x = thumbnail_half_wh - w / 2;

						int h = (w / thumbnail_size + 1) * line_h;
						int y = thumbnail_half_wh - h / 2;

						lyrics_rect = Rect(x, y, w, h);
					}

					if (lyrics_str.size() > 0) {
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
		if (title_rect.mouseOver() && Cursor::OnClientRect() && buttons_enable) {
			title_rect.draw(Color(255, 255, 255, 100));
			Cursor::RequestStyle(CursorStyle::Hand);
		}

		// タイトルの表示
		if (!file_open.first) {	// "Loading..."を表示していないときだけ
			String title = player.getTitle();
			title_rect = FontAsset(U"middle")(title).region(Arg::center(Scene::Width() / 2, Scene::Height() / 3 + 170));
			if (title_rect != title_rect_before) {
				title_rect_before = title_rect;
				count_for_music = Scene::Width() - 50;
			}
			if (title_rect.w > Scene::Width() && !editing_title) {
				mat = Mat3x2::Translate((title_rect.w - Scene::Width()) / 2 - (count_for_music % ((int)title_rect.w + Scene::Width())) + Scene::Width(), 0);
				{
					const Transformer2D t(mat, TransformCursor::No);
					FontAsset(U"middle")(title).drawAt(Scene::Width() / 2, Scene::Height() / 3 + 170, font_color);
				}
			}
			else if (!editing_title) {
				FontAsset(U"middle")(title).drawAt(Scene::Width() / 2, Scene::Height() / 3 + 170, font_color);
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
		prev_button_pressed = NeumorphismUI::CircleButton(Scene::Width() / 4 - 10, Scene::Height() - 100, 30, prev_icon, onAnyButton, buttons_enable);
		next_button_pressed = NeumorphismUI::CircleButton(Scene::Width() - Scene::Width() / 4 + 10, Scene::Height() - 100, 30, next_icon, onAnyButton, buttons_enable);
		
		if (prev_button_pressed)
			player.previous();
		if (next_button_pressed)
			player.next();

		// 再生/一時停止ボタン
		NeumorphismUI::CircleSwitch(Scene::Width() / 2, Scene::Height() - 100, 40, playing, play_icon, pause_icon, onAnyButton, buttons_enable);
		if (before_playing != playing) {
			if (playing)
				player.play();
			else
				player.pause();
			
			before_playing = playing;
		}
		
		playing = player.playing();

		fft_update = !fft_update;
	}
	
	player.free();
	player.saveSettings();
}
