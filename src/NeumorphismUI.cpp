//
//  	  NeumorphismUI.cpp
//
//  		ver.0.2.0
//

#include "NeumorphismUI.hpp"

namespace NeumorphismUI {
	// 長方形
	bool NeumorphismRect(int argPositionX, int argPositionY, int argSizeX, int argSizeY, bool isDented,
		Color argBackgroundColor, Color argDarkColor, Color argLightColor,
		int argRadius, int argBlurSize, int argShadowSize,
		Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		RoundRect rect(argPositionX, argPositionY, argSizeX, argSizeY, argRadius);

		if (isDented) {
			rect.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
		}
		else {
			rect.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
		}

		return rect.mouseOver();
	}

	// 角丸長方形スイッチ
	void RectSwitch(int argPositionX, int argPositionY,
					int argSizeX, int argSizeY,
					bool& argVar, bool& onCursor, bool argEnable,
					String argStr, Font& argFont,
					Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					Color argFontColor, Color argPushedFontColor,
					int argRadius, int argBlurSize, int argShadowSize,
					Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		onCursor = false;
		
		Mat3x2 mat;
		
		RoundRect switchRect(argPositionX, argPositionY, argSizeX, argSizeY, argRadius);
		
		// マウスオーバー時にマウスポインタを変更
		if (argEnable) {
			if (switchRect.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
				onCursor = true;
			}
			// クリックされたら状態を切り替え
			bool isClicked = switchRect.leftClicked();
			if (isClicked) {
				argVar = !argVar;
			}
		}
		
		// ONの時の表示
		if (argVar) {
			switchRect.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Scale(0.9, Point(argPositionX+argSizeX/2, argPositionY+argSizeY/2));
			
			if (!argFont.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argFont(argStr).drawAt(argPositionX+argSizeX/2, argPositionY+argSizeY/2, argPushedFontColor);
				}
			}
		}
		// OFFの時の表示
		else {
			switchRect.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Identity();
			
			if (!argFont.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argFont(argStr).drawAt(argPositionX+argSizeX/2, argPositionY+argSizeY/2, argFontColor);
				}
			}
		}
	}

	void RectSwitch(Vec2 argPosition,
					int argSizeX, int argSizeY,
					bool& argVar, bool& onCursor, bool argEnable,
					String argStr, Font& argFont,
					Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					Color argFontColor, Color argPushedFontColor,
					int argRadius, int argBlurSize, int argShadowSize,
					Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		RectSwitch(argPosition.x, argPosition.y, argSizeX, argSizeY, onCursor, argEnable, argVar, argStr, argFont, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argRadius, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	void RectSwitch(int argPositionX, int argPositionY,
					Vec2 argSize,
					bool& argVar, bool& onCursor, bool argEnable,
					String argStr, Font& argFont,
					Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					Color argFontColor, Color argPushedFontColor,
					int argRadius, int argBlurSize, int argShadowSize,
					Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		RectSwitch(argPositionX, argPositionY, argSize.x, argSize.y, onCursor, argEnable, argVar, argStr, argFont, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argRadius, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	void RectSwitch(Vec2 argPosition,
					Vec2 argSize,
					bool& argVar, bool& onCursor, bool argEnable,
					String argStr, Font& argFont,
					Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					Color argFontColor, Color argPushedFontColor,
					int argRadius, int argBlurSize, int argShadowSize,
					Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		RectSwitch(argPosition.x, argPosition.y, argSize.x, argSize.y, onCursor, argEnable, argVar, argStr, argFont, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argRadius, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	// 角丸長方形ボタン
	int RectButton(int argPositionX, int argPositionY,
					int argSizeX, int argSizeY,
					String argStr, Font& argFont, bool& onCursor, bool argEnable,
					Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					Color argFontColor, Color argPushedFontColor,
					int argRadius, int argBlurSize, int argShadowSize,
					Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		onCursor = false;
		
		Mat3x2 mat;
		
		RoundRect buttonRect(argPositionX, argPositionY, argSizeX, argSizeY, argRadius);
		
		// マウスオーバー時にマウスポインタを変更
		bool clicked = false;
		if (argEnable) {
			if (buttonRect.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
				onCursor = true;
			}
			clicked = buttonRect.leftReleased();
		}
		
		// 押下時の表示
		if (buttonRect.leftPressed()) {
			buttonRect.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Scale(0.9, Point(argPositionX+argSizeX/2, argPositionY+argSizeY/2));
			
			if (!argFont.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argFont(argStr).drawAt(argPositionX+argSizeX/2, argPositionY+argSizeY/2, argPushedFontColor);
				}
			}
		}
		// 押下時以外の表示
		else {
			buttonRect.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Identity();
			
			if (!argFont.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argFont(argStr).drawAt(argPositionX+argSizeX/2, argPositionY+argSizeY/2, argFontColor);
				}
			}
		}
		
		return clicked;
	}

	int RectButton(Vec2 argPosition,
					int argSizeX, int argSizeY,
					String argStr, Font& argFont, bool& onCursor, bool argEnable,
					Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					Color argFontColor, Color argPushedFontColor,
					int argRadius, int argBlurSize, int argShadowSize,
					Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		return RectButton(argPosition.x, argPosition.y, argSizeX, argSizeY, argStr, argFont, onCursor, argEnable, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argRadius, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	int RectButton(int argPositionX, int argPositionY,
					Vec2 argSize,
					String argStr, Font& argFont, bool& onCursor, bool argEnable,
					Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					Color argFontColor, Color argPushedFontColor,
					int argRadius, int argBlurSize, int argShadowSize,
					Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		return RectButton(argPositionX, argPositionY, argSize.x, argSize.y, argStr, argFont, onCursor, argEnable, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argRadius, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	int RectButton(Vec2 argPosition,
					Vec2 argSize,
					String argStr, Font& argFont, bool& onCursor, bool argEnable,
					Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					Color argFontColor, Color argPushedFontColor,
					int argRadius, int argBlurSize, int argShadowSize,
					Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		return RectButton(argPosition.x, argPosition.y, argSize.x, argSize.y, argStr, argFont, onCursor, argEnable, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argRadius, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	int RectButton(Vec2 argPosition,
				   Vec2 argSize,
				    Texture& argTexture, bool& onCursor, bool argEnable,
					Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					Color argFontColor, Color argPushedFontColor,
					int argRadius, int argBlurSize, int argShadowSize,
					Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		onCursor = false;
		
		Mat3x2 mat;
		
		RoundRect buttonRect(argPosition.x, argPosition.y, argSize.x, argSize.y, argRadius);
		
		// マウスオーバー時にマウスポインタを変更
		bool clicked = false;
		if (argEnable) {
			if (buttonRect.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
				onCursor = true;
			}
			clicked = buttonRect.leftReleased();
		}
		
		// 押下時の表示
		if (buttonRect.leftPressed()) {
			buttonRect.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Scale(0.9, Point((int)(argPosition.x+argSize.x/2), (int)(argPosition.y+argSize.y/2)));
			
			if (!argTexture.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argTexture.drawAt(argPosition.x+argSize.x/2, argPosition.y+argSize.y/2, argPushedFontColor);
				}
			}
		}
		// 押下時以外の表示
		else {
			buttonRect.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Identity();
			
			if (!argTexture.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argTexture.drawAt(argPosition.x+argSize.x/2, argPosition.y+argSize.y/2, argFontColor);
				}
			}
		}
		
		return clicked;
	}

	// 丸型
	bool NeumorphismCircle(int argPositionX, int argPositionY, int argSize, bool isDented,
				Color argBackgroundColor, Color argDarkColor, Color argLightColor,
				int argBlurSize, int argShadowSize,
				Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		Circle switchCircle(argPositionX, argPositionY, argSize);
		
		if (isDented) {
			switchCircle.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
		}
		else {
			switchCircle.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
		}

		return switchCircle.mouseOver();
	}

	// 丸型スイッチ
	void CircleSwitch(int argPositionX, int argPositionY,
					  int argSize,
					  bool& argVar,
					  String argStr, Font& argFont, bool& onCursor, bool argEnable,
					  Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					  Color argFontColor, Color argPushedFontColor,
					  int argBlurSize, int argShadowSize,
					  Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		onCursor = false;
		
		Mat3x2 mat;
		
		Circle switchCircle(argPositionX, argPositionY, argSize);
		
		// マウスオーバー時にマウスポインタを変更
		bool isClicked = false;
		if (argEnable) {
			if (switchCircle.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
				onCursor = true;
			}
			// クリックされたら状態を切り替え
			isClicked = switchCircle.leftClicked();
			if (isClicked) {
				argVar = !argVar;
			}
		}
		
		// ONの時の表示
		if (argVar) {
			switchCircle.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Scale(0.9, Point(argPositionX, argPositionY));
			
			if (!argFont.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argFont(argStr).drawAt(argPositionX, argPositionY, argPushedFontColor);
				}
			}
		}
		// OFFの時の表示
		else {
			switchCircle.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Identity();
			
			if (!argFont.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argFont(argStr).drawAt(argPositionX, argPositionY, argFontColor);
				}
			}
		}
	}

	void CircleSwitch(Vec2 argPosition,
					  int argSize,
					  bool& argVar,
					  String argStr, Font& argFont, bool& onCursor, bool argEnable,
					  Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					  Color argFontColor, Color argPushedFontColor,
					  int argBlurSize, int argShadowSize,
					  Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		CircleSwitch(argPosition.x, argPosition.y, argSize, argVar, argStr, argFont, onCursor, argEnable, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	void CircleSwitch(int argPositionX, int argPositionY,
					  int argSize,
					  bool& argVar,
					  Texture& argTexture, bool& onCursor, bool argEnable,
					  Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					  Color argFontColor, Color argPushedFontColor,
					  int argBlurSize, int argShadowSize,
					  Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		onCursor = false;
		
		Mat3x2 mat;
		
		Circle switchCircle(argPositionX, argPositionY, argSize);
		
		// マウスオーバー時にマウスポインタを変更
		bool isClicked = false;
		if (argEnable) {
			if (switchCircle.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
				onCursor = true;
			}
			// クリックされたら状態を切り替え
			isClicked = switchCircle.leftClicked();
			if (isClicked) {
				argVar = !argVar;
			}
		}
		
		// ONの時の表示
		if (argVar) {
			switchCircle.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Scale(0.9, Point(argPositionX, argPositionY));
			
			if (!argTexture.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argTexture.drawAt(argPositionX, argPositionY, argPushedFontColor);
				}
			}
		}
		// OFFの時の表示
		else {
			switchCircle.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Identity();
			
			if (!argTexture.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argTexture.drawAt(argPositionX, argPositionY, argFontColor);
				}
			}
		}
	}

	void CircleSwitch(Vec2 argPosition,
					  int argSize,
					  bool& argVar,
					  Texture& argTexture, bool& onCursor, bool argEnable,
					  Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					  Color argFontColor, Color argPushedFontColor,
					  int argBlurSize, int argShadowSize,
					  Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		CircleSwitch(argPosition.x, argPosition.y, argSize, argVar, argTexture, onCursor, argEnable, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	void CircleSwitch(int argPositionX, int argPositionY,
					  int argSize,
					  bool& argVar,
					  Texture& argTexture, Texture& argPushedTexture, bool& onCursor, bool argEnable,
					  Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					  Color argFontColor, Color argPushedFontColor,
					  int argBlurSize, int argShadowSize,
					  Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		
		Mat3x2 mat;
		
		Circle switchCircle(argPositionX, argPositionY, argSize);
		
		// マウスオーバー時にマウスポインタを変更
		bool isClicked = false;
		if (argEnable) {
			if (switchCircle.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
				onCursor = true;
			}
			// クリックされたら状態を切り替え
			isClicked = switchCircle.leftClicked();
			if (isClicked) {
				argVar = !argVar;
			}
		}
		
		// ONの時の表示
		if (argVar) {
			switchCircle.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Scale(0.9, Point(argPositionX, argPositionY));
			
			// 座標変換行列を適用
			const Transformer2D t(mat, TransformCursor::No);
			argPushedTexture.drawAt(argPositionX, argPositionY, argFontColor);
		}
		// OFFの時の表示
		else {
			switchCircle.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Identity();
			
			// 座標変換行列を適用
			const Transformer2D t(mat, TransformCursor::No);
			argTexture.drawAt(argPositionX, argPositionY, argFontColor);
		}
	}

	// 丸型ボタン
	int CircleButton(int argPositionX, int argPositionY,
					 int argSize,
					 String argStr, Font& argFont, bool& onCursor, bool argEnable,
					 Color argBackgroundColor , Color argDarkColor, Color argLightColor,
					 Color argFontColor, Color argPushedFontColor,
					 int argBlurSize, int argShadowSize,
					 Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		
		Mat3x2 mat;
		
		Circle buttonCircle(argPositionX, argPositionY, argSize);
		
		// マウスオーバー時にマウスポインタを変更
		bool clicked = false;
		if (argEnable) {
			if (buttonCircle.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
				onCursor = true;
			}
			clicked = buttonCircle.leftReleased();
		}
		
		// 押下時の表示
		if (buttonCircle.leftPressed()) {
			buttonCircle.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Scale(0.9, Point(argPositionX, argPositionY));
			
			if (!argFont.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argFont(argStr).drawAt(argPositionX, argPositionY, argPushedFontColor);
				}
			}
		}
		// 押下時以外の表示
		else {
			buttonCircle.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Identity();
			
			if (!argFont.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argFont(argStr).drawAt(argPositionX, argPositionY, argFontColor);
				}
			}
		}
		
		return clicked;
	}

	int CircleButton(Vec2 argPosition,
					 int argSize,
					 String argStr, Font& argFont, bool& onCursor, bool argEnable,
					 Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					 Color argFontColor, Color argPushedFontColor,
					 int argBlurSize, int argShadowSize,
					 Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		return CircleButton(argPosition.x, argPosition.y, argSize, argStr, argFont, onCursor, argEnable, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	int CircleButton(int argPositionX, int argPositionY,
					 int argSize,
					 Texture& argTexture, bool& onCursor, bool argEnable,
					 Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					 Color argFontColor, Color argPushedFontColor,
					 int argBlurSize, int argShadowSize,
					 Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		
		Mat3x2 mat;
		
		Circle buttonCircle(argPositionX, argPositionY, argSize);
		
		// マウスオーバー時にマウスポインタを変更
		bool clicked = false;
		if (argEnable) {
			if (buttonCircle.mouseOver()) {
				Cursor::RequestStyle(CursorStyle::Hand);
				onCursor = true;
			}
			clicked = buttonCircle.leftReleased();
		}
		
		// 押下時の表示
		if (buttonCircle.leftPressed()) {
			buttonCircle.drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Scale(0.9, Point(argPositionX, argPositionY));
			
			if (!argTexture.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argTexture.drawAt(argPositionX, argPositionY, argPushedFontColor);
				}
			}
		}
		// 押下時以外の表示
		else {
			buttonCircle.drawShadow(argLowerShadowPosOffset, argBlurSize, argShadowSize, argDarkColor).drawShadow(argUpperShadowPosOffset, argBlurSize, argShadowSize, argLightColor).draw(argBackgroundColor);
			mat = Mat3x2::Identity();
			
			if (!argTexture.isEmpty()) {
				{
					// 座標変換行列を適用
					const Transformer2D t(mat, TransformCursor::No);
					argTexture.drawAt(argPositionX, argPositionY, argFontColor);
				}
			}
		}
		
		return clicked;
	}

	int CircleButton(Vec2 argPosition,
					 int argSize,
					 Texture& argTexture, bool& onCursor, bool argEnable,
					 Color argBackgroundColor, Color argDarkColor, Color argLightColor,
					 Color argFontColor, Color argPushedFontColor,
					 int argBlurSize, int argShadowSize,
					 Vec2 argUpperShadowPosOffset, Vec2 argLowerShadowPosOffset)
	{
		return CircleButton(argPosition.x, argPosition.y, argSize, argTexture, onCursor, argEnable, argBackgroundColor, argDarkColor, argLightColor, argFontColor, argPushedFontColor, argBlurSize, argShadowSize, argUpperShadowPosOffset, argLowerShadowPosOffset);
	}

	// スライドスイッチ
	SlideSwitch::SlideSwitch(bool argVar, int argPositionX, int argPositionY, int argSizeW, int argSizeH) {
		init(argVar, argPositionX, argPositionY, argSizeW, argSizeH);
	}
		
	SlideSwitch::SlideSwitch(bool argVar, int argPositionX, int argPositionY, Vec2 argSize) {
		init(argVar, argPositionX, argPositionY, argSize.x, argSize.y);
	}
		
	SlideSwitch::SlideSwitch(bool argVar, Vec2 argPosition, int argSizeW, int argSizeH) {
		init(argVar, argPosition.x, argPosition.y, argSizeW, argSizeH);
	}
		
	SlideSwitch::SlideSwitch(bool argVar, Vec2 argPosition, Vec2 argSize) {
		init(argVar, argPosition.x, argPosition.y, argSize.x, argSize.y);
	}
		
	bool SlideSwitch::draw() {
		switchRect
			.drawShadow(lowerShadowPosOffset, blurSize, shadowSize, darkShadow)
			.drawShadow(upperShadowPosOffset, blurSize, shadowSize, lightShadow)
			.draw(background);		// スイッチの外枠
		
		innerSliderRect
			.drawShadow(upperShadowPosOffset, blurSize, shadowSize, darkShadow)
			.drawShadow(lowerShadowPosOffset, blurSize, shadowSize, lightShadow)
			.draw(background);		// スイッチの内枠
		
		knobCircle = Circle(position.x+10/2+knobX, position.y+10/2+(size.y-10)/2, (size.y-10/2)/2);
		knobCircle.drawShadow(Vec2{0, 0}, blurSize, shadowSize, darkShadow);
		
		barRect = RoundRect(position.x+10/2, position.y+10/2, knobX, size.y-10, size.y/2);
		barRect.draw(Color(52, 152, 219));
		
		knobCircle.draw(background);
		
		// マウスオーバー時にマウスポインタを変更
		if (knobCircle.mouseOver() || barRect.mouseOver() || innerSliderRect.mouseOver()) {
			Cursor::RequestStyle(CursorStyle::Hand);
		}
		// 押されたら状態変更
		if (knobCircle.leftClicked() || barRect.leftClicked() || innerSliderRect.leftClicked()) {
			b = !b;
			sliding = true;
			slidingCount = 0.0;
		}
		
		// スライド中の動作
		if (sliding) {
			slidingCount += 0.1;
			if (slidingCount > M_PI/2) {
				sliding = false;
			}
			else {
				if (b) {
					knobX = sin(slidingCount)*(size.x-10);
				}
				else {
					knobX = (size.x-10) - sin(slidingCount)*(size.x-10);
				}
			}
		}
		
		return b;
	}
	
	// コンストラクタから呼び出す
	void SlideSwitch::init(bool argBool, int argPositionX, int argPositionY, int argSizeW, int argSizeH) {
		upperShadowPosOffset = {-4, -4};
		lowerShadowPosOffset = {4, 4};
		blurSize = 16;
		shadowSize = 2;
		
		b = argBool;
		position.x = argPositionX;
		position.y = argPositionY;
		size.x = argSizeW;
		size.y = argSizeH;
		
		background = Color(224, 229, 236);
		darkShadow = Color(163, 177, 198);
		lightShadow = Color(255, 255, 255);
		
		sliding = false;
		knobX = b*(argSizeW-10);
		
		switchRect = RoundRect(position, size, size.y/2);
		innerSliderRect = RoundRect(position.x+10/2, position.y+10/2, size.x-10, size.y-10, (size.y-10)/2);
	}

	// スライダー
	Slider::Slider(){
		
	}
	
	Slider::Slider(double argVar, int argPositionX, int argPositionY, int argSizeW, int argSizeH) {
		init(argVar, argPositionX, argPositionY, argSizeW, argSizeH);
	}
	
	Slider::Slider(double argVar, int argPositionX, int argPositionY, Vec2 argSize) {
		init(argVar, argPositionX, argPositionY, argSize.x, argSize.y);
	}
	
	Slider::Slider(double argVar, Vec2 argPosition, int argSizeW, int argSizeH) {
		init(argVar, argPosition.x, argPosition.y, argSizeW, argSizeH);
	}
	
	Slider::Slider(double argVar, Vec2 argPosition, Vec2 argSize) {
		init(argVar, argPosition.x, argPosition.y, argSize.x, argSize.y);
	}
	
	double Slider::draw(bool& onCursor) {
		sliderRect
		.drawShadow(lowerShadowPosOffset, blurSize, shadowSize, darkShadow)
		.drawShadow(upperShadowPosOffset, blurSize, shadowSize, lightShadow)
		.draw(background);	// スイッチの外枠
		
		innerSliderRect
		.drawShadow(upperShadowPosOffset, blurSize, shadowSize, darkShadow)
		.drawShadow(lowerShadowPosOffset, blurSize, shadowSize, lightShadow)
		.draw(background);	// スイッチの内枠
		
		knobCircle = Circle(position.x+10/2+knobX, position.y+10/2+(size.y-10)/2, (size.y-10/2)/2);
		knobCircle.drawShadow(Vec2{0, 0}, blurSize, shadowSize, darkShadow);
		
		barRect = RoundRect(position.x+10/2, position.y+10/2, knobX, size.y-10, size.y/2);
		barRect.draw(Color(52, 152, 219));
		
		knobCircle.draw(background);

		// マウスオーバー時にマウスポインタを変更
		if (knobCircle.mouseOver() || barRect.mouseOver() || innerSliderRect.mouseOver()) {
			Cursor::RequestStyle(CursorStyle::Hand);
			onCursor = true;
		}
		
		if (mouseLPressed) {
			mouseLPressed = false;
		}
		
		// クリックされたら状態変更
		if (knobCircle.leftPressed() || barRect.leftPressed() || innerSliderRect.leftPressed() || knobClicked) {
			if (!mouseLPressed && MouseL.down()) {
				mouseLPressed = true;
			}
			
			if (knobCircle.leftPressed()) {
				knobClicked = true;
			}
			
			// マウスがクリックされっぱなしのときにスライドし続ける
			if (barRect.leftPressed() || innerSliderRect.leftPressed()) {
				beforeX = knobX;
				clickedX = Cursor::Pos().x-position.x+10/2;
				sliding = true;
				slidingCount = 0.0;
			}
			
			value = (Cursor::Pos().x-position.x-10/2)/innerSize.x;
			
			if (value > 1.0) {
				value = 1.0;
			}
			if (value < 0.0) {
				value = 0.0;
			}
		}
		if (knobClicked && !MouseL.pressed()) {
			knobClicked = false;
		}
		
		// スライド中の動作
		if (sliding) {
			slidingCount += 0.2;
			if (slidingCount > M_PI/2) {
				slidingCount = 0;
				sliding = false;
			}
			else {
				if (beforeX > clickedX) {
					knobX = beforeX - sin(slidingCount)*(beforeX-clickedX+(innerSize.y-10)/2);
				}
				else {
					knobX = beforeX + sin(slidingCount)*(clickedX-beforeX-(innerSize.y-10)/2);
				}
			}
		}
		if (knobClicked) {
			knobX = value*innerSize.x;
		}
		
		return value;
	}
	
	bool Slider::setValue(double arg_value) {
		if (arg_value < 0.0 || arg_value > 1.0) {
			return false;
		}
		
		if (value == arg_value) {
			return true;
		}
		
		beforeX = knobX;
		clickedX = arg_value*innerSize.x;
		sliding = true;
		slidingCount = 0.0;
		
		value = arg_value;
		return true;
	}
	
	bool Slider::setValueNoAnimetion(double arg_value) {
		if (arg_value < 0.0 || arg_value > 1.0) {
			return false;
		}
		
		value = arg_value;
		knobX = arg_value * innerSize.x;
		
		return true;
	}

	void Slider::setPosition(Point arg_position) {
		upperShadowPosOffset = { -4, -4 };
		lowerShadowPosOffset = { 4, 4 };
		blurSize = 16;
		shadowSize = 2;

		position.x = arg_position.x;
		position.y = arg_position.y;
		innerSize.x = size.x - 10;
		innerSize.y = size.y - 10;

		sliderRect = RoundRect(arg_position.x, arg_position.y, size.x, size.y, size.y / 2);
		innerSliderRect = RoundRect(arg_position.x + 10 / 2, arg_position.y + 10 / 2, innerSize.x, innerSize.y, innerSize.y / 2);

		knobX = innerSize.x * value;
	}
	
	bool Slider::isSliderMoving() {
		return sliding;
	}
	
	bool Slider::isSliderLeftReleased() {
		return sliderRect.leftReleased();
	}
	
	// コンストラクタから呼び出す
	void Slider::init(double argRet, int argPositionX, int argPositionY, int argSizeW, int argSizeH) {
		upperShadowPosOffset = {-4, -4};
		lowerShadowPosOffset = {4, 4};
		blurSize = 16;
		shadowSize = 2;
		
		value = argRet;
		
		position.x = argPositionX;
		position.y = argPositionY;
		size.x = argSizeW;
		size.y = argSizeH;
		innerSize.x = size.x - 10;
		innerSize.y = size.y - 10;
		
		background = Color(224, 229, 236);
		darkShadow = Color(163, 177, 198);
		lightShadow = Color(255, 255, 255);
		
		sliderRect = RoundRect(argPositionX, argPositionY, argSizeW, argSizeH, argSizeH/2);
		innerSliderRect = RoundRect(argPositionX+10/2, argPositionY+10/2, innerSize.x, innerSize.y, innerSize.y / 2);
		
		knobClicked = false;
		sliding = false;
		knobX = innerSize.x*value;
	}
}
