//
//  ButtonEx.hpp
//  Albus-Player
//
//  Created by YotioSoft on 2021/08/04.
//

#ifndef ButtonEx_hpp
#define ButtonEx_hpp

#include "Specific.hpp"
#include <NeumorphismUI.hpp>

namespace NeumorphismUI {
	// 丸型スイッチ
	void CircleSwitchEx(int argPositionX, int argPositionY,
					  int argSize,
					  bool& argVar,
					  Texture& argTexture, Texture& argPushedTexture,
					  Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					  Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					  int argBlurSize = 16, int argShadowSize = 2,
					  Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4})
	{
		
		Mat3x2 mat;
		
		Circle switchCircle(argPositionX, argPositionY, argSize);
		
		// マウスオーバー時にマウスポインタを変更
		if (switchCircle.mouseOver()) {
			Cursor::RequestStyle(CursorStyle::Hand);
		}
		// クリックされたら状態を切り替え
		bool isClicked = switchCircle.leftClicked();
		if (isClicked) {
			argVar = !argVar;
		}
		
		// ONの時の表示
		if (argVar) {
			switchCircle.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Scale(0.9, Point(argPositionX, argPositionY));
			
			// 座標変換行列を適用
			const Transformer2D t(mat, false);
			argPushedTexture.drawAt(argPositionX, argPositionY, argFontColor);
		}
		// OFFの時の表示
		else {
			switchCircle.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Identity();
			
			// 座標変換行列を適用
			const Transformer2D t(mat, false);
			argTexture.drawAt(argPositionX, argPositionY, argFontColor);
		}
	}
}

#endif /* ButtonEx_hpp */
