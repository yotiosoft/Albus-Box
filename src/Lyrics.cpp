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
	current_samples = 0;
	current_index = 0;
}

// 歌詞の取り込み
void Lyrics::add_lyric(int begin, int end, String str) {
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
}

// 歌詞の取得
String Lyrics::get_lyrics(int time_samples) {
	int before_samples = current_samples;
	current_samples = time_samples;

	if (before_samples - time_samples == 1) {
		// 次の歌詞の開始位置ならcurent_index++
		if (current_index < lyrics_array.size()) {
			if (current_samples == lyrics_array[current_index + 1].begin) {
				current_index++;
			}
		}
	}
	else {
		current_index = 0;
		for (int i = 0; i < lyrics_array.size(); i++) {
			if (lyrics_array[i].begin <= current_samples && lyrics_array[i].end > current_samples) {
				current_index = i;
				break;
			}
		}
	}

	// 歌詞が適用範囲なら歌詞を返す
	if (current_index >= 0 && current_index < lyrics_array.size()) {
		if (lyrics_array[current_index].begin <= current_samples && lyrics_array[current_index].end > current_samples) {
			return lyrics_array[current_index].lyrics;
		}
	}

	// 適用範囲の歌詞が存在しなければ空白を返す
	return U"";
}
