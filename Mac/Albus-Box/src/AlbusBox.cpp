//
//  AlbusBox.cpp
//  Albus-Box
//
//  Created by YotioSoft on 2021/08/06.
//

#include "AlbusBox.hpp"

void AlbusBox::init() {
	font13 = Font(13, U"{}/NotoSansCJKjp/NotoSansCJKjp-Regular.otf"_fmt(specific::getFontsDir()));
	font16 = Font(16, U"{}/NotoSansCJKjp/NotoSansCJKjp-Regular.otf"_fmt(specific::getFontsDir()));
	
	slider = NeumorphismUI::Slider(0.0, Vec2{50, Scene::Height()/2+100}, Scene::Width()-100, 30);
	
	play_icon = Texture(Icon(IconFont::Play, 30));
	pause_icon = Texture(Icon(IconFont::Pause, 30));
	
	prev_icon = Texture(Icon(IconFont::Prev, 20));
	next_icon = Texture(Icon(IconFont::Next, 20));
	
	fileopen_icon = Texture(Icon(IconFont::FileOpen, 20));
	
	audio_file_path_list << U"sample/魔王魂 シャイニングスター.ogg";
	audio_files_list << Audio(audio_file_path_list[track_in_list]);
	audio_files_list[track_in_list].setLoop(enabled_loop);
	
	font_color = Color(Palette::Gray);
	button_close_color = Color(242, 38, 19);
	
	thumbnail_image = Image(thumbnail_size, thumbnail_size, Palette::White);
	thumbnail_color1 = Color(Palette::Orangered);
	thumbnail_color2 = Color(Palette::Mediumpurple);
	
	for (int y = 0; y < thumbnail_image.height(); y++) {
		for (int x = 0; x < thumbnail_image.width(); x++) {
			double t = (double)(x + y) / (thumbnail_image.width() + thumbnail_image.height() - 2);
			thumbnail_image[y][x] = thumbnail_color1.lerp(thumbnail_color2, t);
		}
	}
	thumbnail_texture = RenderTexture(thumbnail_image);
	thunbnail_image_texture = Texture(thumbnail_image);
	thumbnail_circle = Circle(Scene::Width()/2, Scene::Height()/3, thumbnail_size/2);
}

pair<bool, FilePath> AlbusBox::FileOpen() {
	// ファイル選択ダイアログ
	Array<FileFilter> ff = {{U"音声ファイル", {U"mp3", U"ogg", U"wav", U"m4a"}}};
	String filePathTemp;
	if (const auto open = Dialog::OpenFile(ff)) {
		return pair<bool, FilePath>(true, open.value());
	}
	
	return pair<bool, FilePath>(false, FilePath());
}

bool AlbusBox::draw() {
	// ウィンドウの移動
	if (MouseL.down()) {
		mouse_clicked = Cursor::Pos();
	}
	if (MouseL.pressed()) {
		Window::SetPos(Cursor::ScreenPos() - mouse_clicked);
	}
	
	// ウィンドウを閉じる/最小化ボタン
	if (NeumorphismUI::CircleButton(20, 20, 8, U"", font16, button_close_color)) {
		return false;
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
			double size = Pow(fft.buffer[i * (double)fft_size / row_boxes], 0.6f) * 2000;
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
	NeumorphismUI::CircleSwitch(Scene::Width()/2, Scene::Height()-100, 40, playing, play_icon, pause_icon);
	
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
	
	return true;
}

void AlbusBox::release() {
	audio_files_list[track_in_list].stop();

	for (auto af : audio_files_list)
		af.release();
}
