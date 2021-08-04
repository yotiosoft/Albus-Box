#include <Specific.hpp>
#include <NeumorphismUI.hpp>
#include "ButtonEx.hpp"
#include "Enums.hpp"

void Main() {
	Scene::SetBackground(DEFAULT_BACKGROUND_COLOR);
	Window::SetStyle(WindowStyle::Frameless);
	Window::Resize(400, 640);
	
	int count = 0;
	
	Font font12(12);
	Font font24(24);
	
	// スライダー
	double play_pos = 0.0;		// スライダーの初期値
	double play_pos_before = 0.0;
	NeumorphismUI::Slider slider(play_pos, Vec2{50, Scene::Height()/2+50}, Scene::Width()-100, 30);
	
	bool playing = true;	// 再生中(false: 一時停止中)
	
	// マウスクリックした地点の記録用
	Point mouse_clicked;
	
	// アイコン
	Texture play_icon(Icon(IconFont::Play, 30));
	Texture pause_icon(Icon(IconFont::Pause, 30));
	
	Texture prev_icon(Icon(IconFont::Prev, 20));
	Texture next_icon(Icon(IconFont::Next, 20));
	
	// ファイル
	const Audio audio_file(U"sample/魔王魂 シャイニングスター.ogg");
	
	while (System::Update()) {
		// ウィンドウの移動
		if (MouseL.down()) {
			mouse_clicked = Cursor::Pos();
		}
		if (MouseL.pressed()) {
			Window::SetPos(Cursor::ScreenPos() - mouse_clicked);
		}
		
		// スライダー
		if (!slider.isSliderMoving()) {
			slider.setValueNoAnimetion(audio_file.posSec()/audio_file.lengthSec());
		}
		play_pos = slider.draw();
		
		// トラック移動ボタン
		NeumorphismUI::CircleButton(Scene::Width()/4-10, Scene::Height()-100, 30, prev_icon);
		NeumorphismUI::CircleButton(Scene::Width()-Scene::Width()/4+10, Scene::Height()-100, 30, next_icon);
		
		// 再生/一時停止ボタン
		NeumorphismUI::CircleSwitchEx(Scene::Width()/2, Scene::Height()-100, 40, playing, play_icon, pause_icon);
		
		// 再生制御
		if (playing) {
			audio_file.play();
		}
		else {
			audio_file.pause();
		}
		
		// 再生位置変更
		if (fabs(play_pos*audio_file.lengthSec() - play_pos_before*audio_file.lengthSec()) > 1) {
			audio_file.setPosSec(play_pos*audio_file.lengthSec());
		}
		play_pos_before = play_pos;
	}
}

