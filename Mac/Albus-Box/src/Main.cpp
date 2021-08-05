#include <Specific.hpp>
#include <NeumorphismUI.hpp>
#include "ButtonEx.hpp"
#include "Enums.hpp"

pair<bool, FilePath> FileOpen() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = {{U"音声ファイル", {U"mp3", U"ogg", U"wav", U"m4a"}}};
	String filePathTemp;
	if (const auto open = Dialog::OpenFile(ff)) {
		return pair<bool, FilePath>(true, open.value());
	}
	
	return pair<bool, FilePath>(false, FilePath());
}

void Main() {
	Scene::SetBackground(DEFAULT_BACKGROUND_COLOR);
	Window::SetStyle(WindowStyle::Frameless);
	Window::Resize(400, 640);
	
	Font font13(13, U"{}/NotoSansCJKjp/NotoSansCJKjp-Regular.otf"_fmt(specific::getFontsDir()));
	Font font16(16, U"{}/NotoSansCJKjp/NotoSansCJKjp-Regular.otf"_fmt(specific::getFontsDir()));
	
	// スライダー
	double play_pos = 0.0;		// スライダーの初期値
	NeumorphismUI::Slider slider(play_pos, Vec2{50, Scene::Height()/2+100}, Scene::Width()-100, 30);
	
	bool playing = true;	// 再生中(false: 一時停止中)
	
	// マウスクリックした地点の記録用
	Point mouse_clicked;
	
	// アイコン
	Texture play_icon(Icon(IconFont::Play, 30));
	Texture pause_icon(Icon(IconFont::Pause, 30));
	
	Texture prev_icon(Icon(IconFont::Prev, 20));
	Texture next_icon(Icon(IconFont::Next, 20));
	
	Texture fileopen_icon(Icon(IconFont::FileOpen, 20));
	
	// ファイル
	Array<FilePath> audio_file_path_list;
	int track_in_list = 0;
	audio_file_path_list << U"sample/魔王魂 シャイニングスター.ogg";
	Array<Audio> audio_files_list;
	audio_files_list << Audio(audio_file_path_list[track_in_list]);
	bool enabled_loop = false;
	audio_files_list[track_in_list].setLoop(enabled_loop);
	
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
	Circle thumbnail_circle(Scene::Width()/2, Scene::Height()/3, thumbnail_size/2);
	
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
	
	while (System::Update()) {
		// ウィンドウの移動
		if (MouseL.down()) {
			mouse_clicked = Cursor::Pos();
		}
		if (MouseL.pressed()) {
			Window::SetPos(Cursor::ScreenPos() - mouse_clicked);
		}
		
		// ウィンドウを閉じる/最小化ボタン
		if (NeumorphismUI::CircleButton(20, 20, 8, U"", font16, button_close_color)) {
			break;
		}
		
		// 右上のボタン群
		// ファイルを開く
		if (NeumorphismUI::CircleButton(Scene::Width()-30, 30, 20, fileopen_icon)) {
			auto file_open = FileOpen();
			if (file_open.first) {
				audio_files_list[track_in_list].stop();
				
				audio_file_path_list << file_open.second;
				audio_files_list << Audio(audio_file_path_list.back());
				audio_files_list.back().setLoop(enabled_loop);
				cout << audio_file_path_list << endl;
				track_in_list = (int)audio_file_path_list.size()-1;
				playing = true;
			}
		}
		
		// サムネイル
		NeumorphismUI::NeumorphismCircle(Scene::Width()/2, Scene::Height()/3, thumbnail_size/2+10, false);
		
		// 波形を表示
		LineString fft_line;
		if (playing) {
			ScopedRenderTarget2D target(thumbnail_texture);
			thunbnail_image_texture.draw(0, 0);
			FFT::Analyze(fft, audio_files_list[track_in_list]);
			int fft_size = 800;
			int box_size = 10;
			int row_boxes = 20;
			for (int i = 0; i < row_boxes; i++) {
				double size = Pow(fft.buffer[i * (double)fft_size / row_boxes], 0.6f) * 1500;
				int fft_box_count = size / box_size;
				for (int j = 0; j < fft_box_count; j++) {
					Rect(Arg::center(i*thumbnail_texture.width()/row_boxes+box_size/2, thumbnail_texture.height()-j*box_size-box_size/2), box_size*3/4, box_size*3/4).draw(Color(200, 200, 200, 200));
				}
			}
		}
		else {
			ScopedRenderTarget2D target(thumbnail_texture);
			thunbnail_image_texture.draw(0, 0);
		}
		
		thumbnail_circle(thumbnail_texture(0, 0, thumbnail_size, thumbnail_size)).draw();
		
		// タイトル
		cMes(font16, FileSystem::BaseName(audio_file_path_list[track_in_list]), Point(0, Scene::Height()/3+120), Size(Scene::Width(), 100), font_color);
		
		// スライダー
		if (!slider.isSliderMoving()) {
			slider.setValueNoAnimetion((double)audio_files_list[track_in_list].posSample()/audio_files_list[track_in_list].samples());
			//cout << (double)audio_file.posSample()/audio_file.samples() << endl;
		}
		play_pos = slider.draw();
		if (slider.isSliderPressed()) {
			audio_files_list[track_in_list].setPosSample(play_pos*audio_files_list[track_in_list].samples());
		}
		
		// 再生位置表示
		font13(U"{}:{:0>2}"_fmt((int)audio_files_list[track_in_list].posSec()/60, (int)audio_files_list[track_in_list].posSec()%60)).draw(40, Scene::Height()/2+140, font_color);
		font13(U"{}:{:0>2}"_fmt((int)audio_files_list[track_in_list].lengthSec()/60, (int)audio_files_list[track_in_list].lengthSec()%60)).draw(Arg::topRight=Point(Scene::Width()-40, Scene::Height()/2+140), font_color);
		
		// トラック移動ボタン
		prev_button_pressed = NeumorphismUI::CircleButton(Scene::Width()/4-10, Scene::Height()-100, 30, prev_icon);
		next_button_pressed = NeumorphismUI::CircleButton(Scene::Width()-Scene::Width()/4+10, Scene::Height()-100, 30, next_icon);
		
		// 再生/一時停止ボタン
		NeumorphismUI::CircleSwitchEx(Scene::Width()/2, Scene::Height()-100, 40, playing, play_icon, pause_icon);
		
		// 再生終了時の処理
		if (audio_files_list[track_in_list].posSample() == audio_files_list[track_in_list].samples()) {
			audio_files_list[track_in_list].stop();
			
			track_in_list ++;
			
			if (track_in_list >= audio_file_path_list.size() - 1) {
				track_in_list = 0;
			}
			audio_files_list[track_in_list].play();
			playing = true;
			
			/*
			audio_file.stop();
			playing = false;*/
		}
		
		// 再生制御
		if (!playing) {
			audio_files_list[track_in_list].pause();
		}
		else if(!audio_files_list[track_in_list].isPlaying()) {
			audio_files_list[track_in_list].play();
		}
		
		if (prev_button_pressed) {
			if (audio_files_list[track_in_list].posSec() > 1.0) {
				audio_files_list[track_in_list].setPosSample(0.0);
			}
			else {
				audio_files_list[track_in_list].stop();
				
				track_in_list --;
				if (track_in_list < 0) {
					track_in_list = (int)audio_file_path_list.size()-1;
				}
				cout << "before" << endl;
				audio_files_list[track_in_list].play();
				cout << "after" << endl;
				playing = true;
			}
		}
		if (next_button_pressed) {
			audio_files_list[track_in_list].stop();
			
			track_in_list ++;
			if (track_in_list >= audio_file_path_list.size()) {
				track_in_list = 0;
			}
			audio_files_list[track_in_list].play();
			playing = true;
		}
	}
	
	audio_files_list[track_in_list].stop();
	
	for (auto af : audio_files_list)
		af.release();
}

