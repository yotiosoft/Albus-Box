#include <Specific.hpp>
#include <NeumorphismUI.hpp>
#include "ButtonEx.hpp"
#include "Enums.hpp"

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
	
	// ファイル
	String audio_file_path = U"sample/魔王魂 シャイニングスター.ogg";
	Audio audio_file(audio_file_path);
	bool enabled_loop = false;
	audio_file.setLoop(enabled_loop);
	
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
	
	Wave wave = audio_file.getWave();
	cout << wave.size() << " " << audio_file.samples() << endl;
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
		
		// サムネイル
		NeumorphismUI::NeumorphismCircle(Scene::Width()/2, Scene::Height()/3, thumbnail_size/2+10, false);
		
		// 波形を表示
		LineString fft_line;
		if (playing) {
			ScopedRenderTarget2D target(thumbnail_texture);
			thunbnail_image_texture.draw(0, 0);
			cout << thumbnail_texture.width() << endl;
			FFT::Analyze(fft, audio_file);
			int fft_size = 800;
			int box_size = 10;
			int row_boxes = 20;
			for (int i = 0; i < row_boxes; i++) {
				double size = Pow(fft.buffer[i * (double)fft_size / row_boxes], 0.6f) * 1500;
				cout << "A" << (double)fft_size / row_boxes << endl;
				int fft_box_count = size / box_size;
				for (int j = 0; j < fft_box_count; j++) {
					Rect(Arg::center(i*thumbnail_texture.width()/row_boxes+box_size/2, thumbnail_texture.height()-j*box_size-box_size/2), box_size*3/4, box_size*3/4).draw(Color(200, 200, 200, 200));
				}
				//fft_line << Vec2{i/(fft.buffer.size() / 20)*Scene::Width()/20, Scene::Height()/3+120-size};
			}
			
			//fft_line.drawCatmullRom(1, Palette::Gray);
		}
		else {
			ScopedRenderTarget2D target(thumbnail_texture);
			thunbnail_image_texture.draw(0, 0);
		}
		/*
		else {
			for (int i = 0; i < fft.buffer.size(); i += fft.buffer.size() / 20) {
				const double size = 0.0;
				
				fft_line << Vec2{i/(fft.buffer.size() / 20)*Scene::Width()/20, Scene::Height()/3+120-size};
			}
			
			fft_line.drawCatmullRom(1, Palette::Gray);
		}*/
		
		thumbnail_circle(thumbnail_texture(0, 0, thumbnail_size, thumbnail_size)).draw();
		
		// タイトル
		cMes(font16, U"シャイニングスター", Point(0, Scene::Height()/3+120), Size(Scene::Width(), 100), font_color);
		
		// スライダー
		if (!slider.isSliderMoving()) {
			slider.setValueNoAnimetion((double)audio_file.posSample()/audio_file.samples());
			//cout << (double)audio_file.posSample()/audio_file.samples() << endl;
		}
		play_pos = slider.draw();
		if (slider.isSliderPressed()) {
			audio_file.setPosSample(play_pos*audio_file.samples());
		}
		
		// 再生位置表示
		font13(U"{}:{:0>2}"_fmt((int)audio_file.posSec()/60, (int)audio_file.posSec()%60)).draw(40, Scene::Height()/2+140, font_color);
		font13(U"{}:{:0>2}"_fmt((int)audio_file.lengthSec()/60, (int)audio_file.lengthSec()%60)).draw(Arg::topRight=Point(Scene::Width()-40, Scene::Height()/2+140), font_color);
		
		// トラック移動ボタン
		prev_button_pressed = NeumorphismUI::CircleButton(Scene::Width()/4-10, Scene::Height()-100, 30, prev_icon);
		next_button_pressed = NeumorphismUI::CircleButton(Scene::Width()-Scene::Width()/4+10, Scene::Height()-100, 30, next_icon);
		
		// 再生/一時停止ボタン
		NeumorphismUI::CircleSwitchEx(Scene::Width()/2, Scene::Height()-100, 40, playing, play_icon, pause_icon);
		
		// 再生終了時の処理
		if (audio_file.posSample() == audio_file.samples()) {
			audio_file.stop();
			playing = false;
		}
		
		// 再生制御
		if (!playing) {
			audio_file.pause();
		}
		else if(!audio_file.isPlaying()) {
			audio_file.play();
		}
		
		if (prev_button_pressed) {
			audio_file.setPosSample(0.0);
		}
		if (next_button_pressed) {
			audio_file.setPosSample(0.0);
		}
		
		cout << "Left:" << wave[audio_file.posSample()].left << endl;
		cout << "Right:" << wave[audio_file.posSample()].right << endl;
	}
	
	audio_file.stop();
}

