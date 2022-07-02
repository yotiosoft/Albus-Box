//
//  Lyrics.cpp
//  Albus-Box
//
//  Created by YotioSoft on 2022/04/03.
//

#include "Lyrics.hpp"

void Lyrics::make_sample() {
	JSON j_lyrics_whole;

	j_lyrics_whole[U"saved_release_num"] = RELEASE_NUM;

	Array<JSON> j_lyrics_arg;
	JSON el, el2;

	el[U"begin"] = 1000;
	el[U"end"] = 2000;
	el[U"str"] = U"This is a test.";
	j_lyrics_arg << el;

	el2[U"begin"] = 3000;
	el2[U"end"] = 4000;
	el2[U"str"] = U"This is a test.2";
	j_lyrics_arg << el2;

	j_lyrics_whole[U"lyrics"] = j_lyrics_arg;

	j_lyrics_whole.save(U"test.lyrics");
}

// コンストラクタ
Lyrics::Lyrics() {
	current_index = -1;
	lyrics_array.clear();
	is_valid = false;
}

Lyrics::Lyrics(String path) {
	// 初期化
	current_index = -1;
	lyrics_array.clear();
	is_valid = false;

	// jsonファイル読み込み
	JSON j_lyrics_whole = JSON::Load(path);
	if (not j_lyrics_whole) {
		return;
	}
	is_valid = true;

	// 歌詞の読み込み
	for (auto j_element : j_lyrics_whole[U"lyrics"].arrayView()) {
		double begin = j_element[U"begin"].get<int>();
		double end = j_element[U"end"].get<int>();
		String str = j_element[U"str"].getString();

		add_lyric(begin, end, str);
	}
}

// 歌詞の取り込み
void Lyrics::add_lyric(double begin, double end, String str) {
	// 新しいLyricsElementを生成
	LyricsElement new_lyric;
	new_lyric.begin = begin;
	new_lyric.end = end;
	new_lyric.lyrics = str;

	// 歌詞配列に挿入
	int t = 0;
	for (auto el : lyrics_array) {
		if (el.begin > begin) {
			break;
		}
		t++;
	}
	lyrics_array.insert(lyrics_array.begin() + t, new_lyric);

	// 有効化
	if (!is_valid) {
		is_valid = true;
	}
}

// 歌詞の取得
String Lyrics::get_lyrics(double time_samples) {
	if ((current_index = get_lyrics_index(time_samples)) >= 0) {
		return current_lyrics->lyrics;
	}

	return U"";
}

Array<LyricsElement> Lyrics::get_lyrics_list() {
	return lyrics_array;
}

// 現在の歌詞のインデックス値を取得
int Lyrics::get_lyrics_index(double time_samples) {
	if (current_index >= 0) {
		if (current_lyrics->begin <= time_samples && current_lyrics->end > time_samples) {
            before_time = time_samples;
			return current_index;
		}
	}
    
    if (before_time < time_samples && current_index >= 0) {
        for (int i = current_index; i < lyrics_array.size(); i++) {
            if (lyrics_array[i].begin <= time_samples && lyrics_array[i].end > time_samples) {
                current_index = i;
                current_lyrics = &lyrics_array[i];
                before_time = time_samples;
                return i;
            }
            if (lyrics_array[i].begin > time_samples) {
                break;
            }
        }
    }
    else if (current_index < 0) {
        for (int i = 0; i < lyrics_array.size(); i++) {
            if (lyrics_array[i].begin <= time_samples && lyrics_array[i].end > time_samples) {
                current_index = i;
                current_lyrics = &lyrics_array[i];
                before_time = time_samples;
                return i;
            }
            if (lyrics_array[i].begin > time_samples) {
                break;
            }
        }
    }
    else {
        for (int i = current_index; i >= 0; i--) {
            if (lyrics_array[i].begin <= time_samples && lyrics_array[i].end > time_samples) {
                current_index = i;
                current_lyrics = &lyrics_array[i];
                before_time = time_samples;
                return i;
            }
            if (lyrics_array[i].begin > time_samples) {
                break;
            }
        }
    }
    
    before_time = time_samples;
	return -1;
}

// 歌詞の始点からの経過時間と終点までの残り時間
double Lyrics::get_begin_time() {
	return current_lyrics->begin;
}
double Lyrics::get_end_time() {
	return current_lyrics->end;
}
