#pragma once

#include <AlbusBox.hpp>

void Main() {
	Scene::SetBackground(DEFAULT_BACKGROUND_COLOR);
	Window::SetStyle(WindowStyle::Frameless);
	Window::Resize(400, 640);
	
	AlbusBox albus_box;
	albus_box.init();
	
	while (System::Update()) {
		if (!albus_box.draw())
			break;
	}
	
	albus_box.release();
}

