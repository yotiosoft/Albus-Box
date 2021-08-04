#include <Specific.hpp>
#include <NeumorphismUI.hpp>
#include "ButtonEx.hpp"

void Main() {
	Scene::SetBackground(DEFAULT_BACKGROUND_COLOR);
	Window::SetStyle(WindowStyle::Frameless);
	Window::Resize(300, 480);
	
	double sliderVar = 0.5;		// スライダーの初期値
	bool slideSwitchVar = true;	// スライドスイッチの初期値
	int count = 0;
	
	Font font24(24);
	
	// スライダーの宣言
	NeumorphismUI::Slider slider(sliderVar, Vec2{400, 100}, 200, 30);
	// スライドスイッチの宣言
	NeumorphismUI::SlideSwitch slideSwitch(slideSwitchVar, 400, 300, 50, 30);
	
	bool rectSwitchPushed = true;		// 角丸長方形スイッチの初期値（ON）
	bool circleSwitchPushed = false;	// 丸型スイッチの初期値（OFF）
	
	// マウスクリックした地点の記録用
	Point mouse_clicked;
	Point mouse_clicked_windows_pos;
	
	while (System::Update()) {
		// ウィンドウの移動
		if (MouseL.down()) {
			mouse_clicked = Cursor::Pos();
		}
		if (MouseL.pressed()) {
			Window::SetPos(Cursor::ScreenPos() - mouse_clicked);
		}
		
		// 丸型スイッチ
		NeumorphismUI::CircleSwitchEx(150, 320, 50, circleSwitchPushed, U"▶", U"II", font24);
	}
}

