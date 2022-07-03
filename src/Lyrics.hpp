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
	double begin;
	double end;
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

	// 歌詞の削除
	void del_lyric(int lyric_num);

	// 歌詞設定の変更
	void set_lyric(int lyric_num, double begin, double end, String str);

	// 歌詞の取得
	String get_lyrics(double time_samples);
	Array<LyricsElement> get_lyrics_list();

	// 現在の歌詞のインデックス値を取得
	int get_lyrics_index(double time_samples);

	// 歌詞の始点からの経過時間と終点までの残り時間
	double get_begin_time();
	double get_end_time();
	
private:
	// 歌詞配列
	Array<LyricsElement> lyrics_array;
    LyricsElement* current_lyrics;

	// 現在の歌詞のインデックス番号
	int current_index;
    double before_time;

	// このLyricsインスタンスは有効？
	bool is_valid;
};

#endif /* Lyrics_hpp */
