//
//  Lyrics.hpp
//  Albus-Box
//
//  Created by YotioSoft on 2022/04/03.
//

#ifndef Lyrics_hpp
#define Lyrics_hpp

#include <Siv3D.hpp>
#include <Siv3D/MD5.hpp>
#include <Specific.hpp>

#include <iostream>
#include <thread>

typedef struct LyricsElement {
	int begin;
	int end;
	String lyrics;
} LyricsElement;

class Lyrics {
public:
	void make_sample();

	// コンストラクタ
	Lyrics();
	Lyrics(String path);

	// 歌詞の取り込み（設定画面から）
	void add_lyric(double begin, double end, String str);

	// 歌詞の取得
	String get_lyrics(double time_samples);

	// 現在の歌詞のインデックス値を取得
	int get_lyrics_index(double time_samples);

	// 歌詞の長さ
	int get_current_lyrics_length(double time_samples);
	
private:
	// 歌詞配列
	Array<LyricsElement> lyrics_array;

	// 現在の歌詞のインデックス番号
	int current_index;
	int before_index;

	// このLyricsインスタンスは有効？
	bool is_valid;
};

#endif /* Lyrics_hpp */
