//
//  OtherFunctions.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "OtherFunctions.hpp"

void cMes(Font& font, String str, Point topLeft, Size size, Color& color) {
	Point str_long = getStringTopLeft(font, str, topLeft, size);
	font(str).draw(str_long.x, str_long.y, color);
}

Point getStringTopLeft(Font& font, String str, Point topLeft, PositionStruct bottomRight) {
	Point retSize;
	retSize.x = (bottomRight.x + topLeft.x) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).w / 2;
	retSize.y = (bottomRight.y + topLeft.y) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).h / 2;
	return retSize;
}
Point getStringTopLeft(Font& font, String str, Point topLeft, Size size) {
	Point retSize;
	retSize.x = (topLeft.x * 2 + size.x) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).w / 2;
	retSize.y = (topLeft.y * 2 + size.y) / 2 - font(str).region(Scene::Width() / 2, Scene::Height() / 2).h / 2;
	return retSize;
}
