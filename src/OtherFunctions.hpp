//
//  OtherFunctions.hpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#ifndef OtherFunctions_hpp
#define OtherFunctions_hpp

#pragma once

#include "Header.hpp"

void cMes(Font& font, String str, Point topLeft, Size size, Color& color);

Point getStringTopLeft(Font& font, String str, Point topLeft, Point bottomRight);
Point getStringTopLeft(Font& font, String str, Point topLeft, Size size);

#endif /* OtherFunctions_hpp */
