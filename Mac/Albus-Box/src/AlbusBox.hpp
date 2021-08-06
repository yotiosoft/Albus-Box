//
//  AlbusBox.hpp
//  Albus-Box
//
//  Created by YotioSoft on 2021/08/06.
//

#ifndef AlbusBox_hpp
#define AlbusBox_hpp

#pragma once

#include "Specific.hpp"-

class AlbusBox {
public:
	void init();
	
	pair<bool, FilePath> FileOpen();
	bool draw();
	
	void release();
	
private:
	// フォント
	Font font13;
	Font font16;
	
	// スライダー
	double play_pos = 0.0;		// スライダーの初期値
	NeumorphismUI::Slider slider;
	
	bool playing = true;	// 再生中(false: 一時停止中)
	
	// マウスクリックした地点の記録用
	Point mouse_clicked;
	
	// アイコン
	Texture play_icon;
	Texture pause_icon;
	
	Texture prev_icon;
	Texture next_icon;
	
	Texture fileopen_icon;
	
	// ファイル
	Array<FilePath> audio_file_path_list;
	int track_in_list = 0;
	Array<Audio> audio_files_list;
	bool enabled_loop = false;
	
	// ボタン押下検知用
	bool prev_button_pressed = false;
	bool next_button_pressed = false;
	
	// 色
	Color font_color;
	Color button_close_color;
	
	// サムネ用
	const int thumbnail_size = 260;
	Image thumbnail_image;
	Color thumbnail_color1;
	Color thumbnail_color2;

	
	RenderTexture thumbnail_texture;
	Texture thunbnail_image_texture;
	Circle thumbnail_circle;
	
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
};

#endif /* AlbusBox_hpp */
