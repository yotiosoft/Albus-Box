#include "AlbusBox.hpp"

pair<bool, FilePath> AudioFileOpen() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = { {U"音声ファイルとプレイリスト", {U"mp3", U"ogg", U"wav", U"m4a", U"playlist"}} };
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
	// ウィンドウを閉じるボタン
	if (OS == "Windows") {
		if (NeumorphismUI::CircleButton(Scene::Width()-30, 30, 15, window_close_icon, onAnyButton)) {
			return true;
		}
	}
	else if (OS == "Mac") {
		Font font_temp = Font(FontAsset(U"middle"));
		Circle exit_button_circle(25, 25, 8);
		exit_button_circle.draw(Color(255, 95, 87));
		if (exit_button_circle.mouseOver()) {
			Cursor::RequestStyle(CursorStyle::Hand);
			onAnyButton = true;
			FontAsset(U"small")(U"×").draw(Arg::center(25, 25));
		}
		if (exit_button_circle.leftReleased()) {
			return true;
		}
	}
	
	return false;
}

// 再生リスト
bool playListView(Player& player, Color& button_close_color, Texture& window_close_icon, Color& font_color)
{
	// ボタン用アイコン
	Texture return_button_icon{ Icon(IconFont::Return), 20 };		// 戻る
	Texture fileopen_icon{ Icon(IconFont::Plus), 20 };				// ファイルを開く
	Texture save_button_icon{ Icon(IconFont::Save), 20 };			// 保存

	Texture play_icon{ Icon(IconFont::Play), 20 };			// 再生
	Texture stop_icon{ Icon(IconFont::Stop), 20 };			// 停止

	// ボタンの位置
	Point return_button_pos;
	if (OS == "Windows") {
		return_button_pos = Point(10, 10);
	}
	else if (OS == "Mac") {
		return_button_pos = Point(45, 10);
	}
	Point fileopen_button_pos = Point(Scene::Width()-50, Scene::Height()-50);
	Point save_button_pos = Point(Scene::Width() / 2, 70);

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
	
	bool onAnyButton;

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
				if (110 - scroll_y + list_element_margin * i + list_element_h / 2 < 100) {
					button_enable = false;
				}

				// 再生・停止ボタン
				if (title_list.second[i]) {	// 再生中
					NeumorphismUI::CircleSwitch(Vec2(70, list_element_margin * i + list_element_h/2), 25, title_list.second[i], stop_icon, button_enable);
					
					if (target_audio != i) {
						slider.setPosition(Point(120, list_element_margin * i + 50));
						target_audio = i;
					}

					if (!slider.isSliderMoving()) {
						slider.setValueNoAnimetion(player.getPlayPosNorm());
					}
					play_pos = slider.draw(onAnyButton);
					if (slider.isSliderLeftReleased()) {
						player.seekTo(play_pos);
					}

					if (before_playing[i] != title_list.second[i]) {
						player.pause();
						target_audio = -1;
					}

					// タイトル
					mat_title = Mat3x2::Translate(0, 0);
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

						title_text.draw(title_x, scroll_y, font_color);
					}
					title_texture.draw(120, list_element_margin * i + 10);
				}
				else {		// 別の曲が再生中
					NeumorphismUI::CircleSwitch(Vec2(70, list_element_margin * i + list_element_h / 2), 25, title_list.second[i], play_icon, button_enable);

					if (before_playing[i] != title_list.second[i]) {
						if (target_audio >= 0) {
							player.pause();
							title_list.second[target_audio] = false;
							before_playing[target_audio] = false;
						}
						player.play(i);
					}

					// タイトル
					mat_title = Mat3x2::Translate(0, 0);
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

						title_text.draw(title_x, scroll_y, font_color);
					}
					title_texture.draw(120, list_element_margin * i + 20);
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
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
		}
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
	if (OS == "Windows") {
		return_button_pos = Point(10, 10);
	}
	else if (OS == "Mac") {
		return_button_pos = Point(45, 10);
	}

	// マウスクリックした地点の記録用
	Point mouse_clicked;
	bool window_moving = false;
	
	bool onAnyButton;
	
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

		// ウィンドウの移動
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
		}
	}
	return true;
}

bool AlbusBoxSetting(Player& player, Color& button_close_color, Texture& window_close_icon, Color& font_color) {
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
	if (OS == "Windows") {
		return_button_pos = Point(10, 10);
	}
	else if (OS == "Mac") {
		return_button_pos = Point(45, 10);
	}
	
	// マウスクリックした地点の記録用
	Point mouse_clicked;
	bool window_moving = false;
	
	bool onAnyButton;
	
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

		// バージョン情報
		if (NeumorphismUI::RectButton(Vec2(Scene::Width() / 2 - 50/2, Scene::Height() - 100), Vec2(50, 50), info_icon, onAnyButton)) {
			if (VersionInformation(player, button_close_color, window_close_icon, font_color)) {
				return true;
			}
		}

		// 再生処理を継続
		player.playing();
		
		// ウィンドウの移動
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
		}
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
	Window::SetStyle(WindowStyle::Frameless);
	Window::Resize(400, 640);

	specific::setWindowStyle(0, 0, 400, 640, 40, 40);

	// フォントアセットの登録
	FontAsset::Register(U"small", 13, Typeface::CJK_Regular_JP);
	FontAsset::Register(U"middle", 16, Typeface::CJK_Regular_JP);
	FontAsset::Register(U"big", 36, Typeface::CJK_Regular_JP);
	FontAsset::Register(U"title", 16, Typeface::Mplus_Bold);

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
	
	Texture loading_texture(U"{}/data/img/loading-24.gif"_fmt(specific::getCurrentDir()));

	// ボタンの位置
	Point setting_button_pos;
	Point fileopen_button_pos;
	Point playlist_button_pos;
	if (OS == "Windows") {
		setting_button_pos = Point(30, 30);
		fileopen_button_pos = Point(80, 30);
		playlist_button_pos = Point(130, 30);
	}
	else if (OS == "Mac") {
		playlist_button_pos = Point(Scene::Width() - 130, 30);
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
	RenderTexture thumbnail_texture(player.getDefdaultThumbnailImage());
	Circle thumbnail_circle(Scene::Width() / 2, Scene::Height() / 3, thumbnail_size / 2);

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
			loading_texture.drawAt(Scene::Width() / 2, Scene::Height() / 3);

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
		LineString fft_line;
		if (playing && player.isOpened() && player.isShowWaveEnabled()) {
			ScopedRenderTarget2D target(thumbnail_texture);
			player.getThumbnailTexture()->drawAt(thumbnail_size / 2, thumbnail_size / 2);
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
		FontAsset(U"small")(U"{}:{:0>2}"_fmt(player.getPlayPosTimeMin(), player.getPlayPosTimeSec())).draw(40, Scene::Height() / 2 + 140, font_color);
		FontAsset(U"small")(U"{}:{:0>2}"_fmt(player.getTotalTimeMin(), player.getTotalTimeSec())).draw(Arg::topRight = Point(Scene::Width() - 40, Scene::Height() / 2 + 140), font_color);

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
		
		// ウィンドウの移動
		if (!onAnyButton) {
			specific::moveWindow(mouse_clicked, window_moving);
		}
	}
	
	player.free();
	player.saveSettings();
}
