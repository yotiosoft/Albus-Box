//
//  	  NeumorphismUI.hpp
//
//  		ver.0.2.0
//

#pragma once

#ifndef NeumorphismUI_hpp
#define NeumorphismUI_hpp

#define _USE_MATH_DEFINES
#include <stdio.h>
#include <math.h>
#include <Siv3D.hpp>

#define DEFAULT_BACKGROUND_COLOR	Color(224, 229, 236)
#define DEFAULT_SHADOW_COLOR		Color(163, 177, 198)
#define DEFAULT_LIGHT_COLOR			Color(255, 255, 255)

namespace NeumorphismUI {
	// 角丸長方形スイッチ
	void RectSwitch(int argPositionX, int argPositionY,
					int argSizeX, int argSizeY,
					bool& argVar,
					String argStr, Font& argFont,
					Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					int argRadius = 10, int argBlurSize = 16, int argShadowSize = 2,
					Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	void RectSwitch(Vec2 argPosition,
					int argSizeX, int argSizeY,
					bool& argVar,
					String argStr, Font& argFont,
					Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					int argRadius = 10, int argBlurSize = 16, int argShadowSize = 2,
					Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	void RectSwitch(int argPositionX, int argPositionY,
					Vec2 argSize,
					bool& argVar,
					String argStr, Font& argFont,
					Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					int argRadius = 10, int argBlurSize = 16, int argShadowSize = 2,
					Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	void RectSwitch(Vec2 argPosition,
					Vec2 argSize,
					bool& argVar,
					String argStr, Font& argFont,
					Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					int argRadius = 10, int argBlurSize = 16, int argShadowSize = 2,
					Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	// 角丸長方形ボタン
	int RectButton(int argPositionX, int argPositionY,
					int argSizeX, int argSizeY,
					String argStr, Font& argFont,
					Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					int argRadius = 10, int argBlurSize = 16, int argShadowSize = 2,
				   Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	int RectButton(Vec2 argPosition,
					int argSizeX, int argSizeY,
					String argStr, Font& argFont,
					Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					int argRadius = 10, int argBlurSize = 16, int argShadowSize = 2,
				   Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	int RectButton(int argPositionX, int argPositionY,
					Vec2 argSize,
					String argStr, Font& argFont,
					Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					int argRadius = 10, int argBlurSize = 16, int argShadowSize = 2,
				   Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	int RectButton(Vec2 argPosition,
					Vec2 argSize,
					String argStr, Font& argFont,
					Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					int argRadius = 10, int argBlurSize = 16, int argShadowSize = 2,
				   Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	int RectButton(Vec2 argPosition,
				Vec2 argSize,
				Texture& argTexture,
				Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
				Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
				int argRadius = 10, int argBlurSize = 16, int argShadowSize = 2,
			   Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	// 丸型
	void NeumorphismCircle(int argPositionX, int argPositionY, int argSize, bool isDented,
				Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
				int argBlurSize = 16, int argShadowSize = 2,
						   Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	// 丸型スイッチ
	void CircleSwitch(int argPositionX, int argPositionY,
					  int argSize,
					  bool& argVar,
					  String argStr, Font& argFont,
					  Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					  Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					  int argBlurSize = 16, int argShadowSize = 2,
					  Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	void CircleSwitch(Vec2 argPosition,
					  int argSize,
					  bool& argVar,
					  String argStr, Font& argFont,
					  Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					  Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					  int argBlurSize = 16, int argShadowSize = 2,
					  Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	void CircleSwitch(int argPositionX, int argPositionY,
					  int argSize,
					  bool& argVar,
					  Texture& argTexture,
					  Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					  Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					  int argBlurSize = 16, int argShadowSize = 2,
					  Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	void CircleSwitch(Vec2 argPosition,
					  int argSize,
					  bool& argVar,
					  Texture& argTexture,
					  Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					  Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					  int argBlurSize = 16, int argShadowSize = 2,
					  Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	void CircleSwitch(int argPositionX, int argPositionY,
					  int argSize,
					  bool& argVar,
					  Texture& argTexture, Texture& argPushedTexture,
					  Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					  Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					  int argBlurSize = 16, int argShadowSize = 2,
					  Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	// 丸型ボタン
	int CircleButton(int argPositionX, int argPositionY,
					 int argSize,
					 String argStr, Font& argFont,
					 Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					 Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					 int argBlurSize = 16, int argShadowSize = 2,
					 Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	int CircleButton(Vec2 argPosition,
					 int argSize,
					 String argStr, Font& argFont,
					 Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					 Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					 int argBlurSize = 16, int argShadowSize = 2,
					 Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	int CircleButton(int argPositionX, int argPositionY,
					 int argSize,
					 Texture& argTexture,
					 Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					 Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					 int argBlurSize = 16, int argShadowSize = 2,
					 Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	int CircleButton(Vec2 argPosition,
					 int argSize,
					 Texture& argTexture,
					 Color argBackgroundColor = DEFAULT_BACKGROUND_COLOR, Color argDarkColor = DEFAULT_SHADOW_COLOR, Color argLightColor = DEFAULT_LIGHT_COLOR,
					 Color argFontColor = Palette::Gray, Color argPushedFontColor = Palette::Gray,
					 int argBlurSize = 16, int argShadowSize = 2,
					 Vec2 argUpperShadowPosOffset = {-4, -4}, Vec2 argLowerShadowPosOffset = {4, 4});

	// スライドスイッチ
	class SlideSwitch {
	public:
		SlideSwitch(bool argVar, int argPositionX, int argPositionY, int argSizeW, int argSizeH);
		SlideSwitch(bool argVar, int argPositionX, int argPositionY, Vec2 argSize);
		SlideSwitch(bool argVar, Vec2 argPosition, int argSizeW, int argSizeH);
		SlideSwitch(bool argVar, Vec2 argPosition, Vec2 argSize);
		
		bool draw();
		
	private:
		bool b;
		bool sliding;
		
		Vec2 upperShadowPosOffset;
		Vec2 lowerShadowPosOffset;
		int blurSize;
		int shadowSize;
		
		Vec2 position;
		Size size;
		
		Color background;
		Color darkShadow;
		Color lightShadow;
		
		s3d::RoundRect switchRect;
		s3d::RoundRect innerSliderRect;
		s3d::RoundRect barRect;
		Circle knobCircle;
		
		int knobX;
		double slidingCount;
		
		// コンストラクタから呼び出す
		void init(bool argBool, int argPositionX, int argPositionY, int argSizeW, int argSizeH);
	};

	// スライダー
	class Slider {
	public:
		Slider();
		Slider(double argVar, int argPositionX, int argPositionY, int argSizeW, int argSizeH);
		Slider(double argVar, int argPositionX, int argPositionY, Vec2 argSize);
		Slider(double argVar, Vec2 argPosition, int argSizeW, int argSizeH);
		Slider(double argVar, Vec2 argPosition, Vec2 argSize);
		
		double draw();
		
		bool setValue(double arg_value);
		
		bool setValueNoAnimetion(double arg_value);
		
		bool isSliderMoving();
		
		bool isSliderLeftReleased();
		
	private:
		s3d::RoundRect sliderRect;
		s3d::RoundRect innerSliderRect;
		s3d::RoundRect barRect;
		Circle knobCircle;
		
		bool knobClicked;
		bool mouseLPressed;
		bool sliding;
		
		double value;
		
		Vec2 upperShadowPosOffset;
		Vec2 lowerShadowPosOffset;
		int blurSize;
		int shadowSize;
		
		Vec2 position;
		Size size;
		Size innerSize;
		
		Color background;
		Color darkShadow;
		Color lightShadow;
		
		int knobX;
		int clickedX;
		int beforeX;
		double slidingCount;
		
		// コンストラクタから呼び出す
		void init(double argRet, int argPositionX, int argPositionY, int argSizeW, int argSizeH);
	};
}

#endif /* NeumorphismUI_hpp */
