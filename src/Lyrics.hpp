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
	
private:
	
};

#endif /* Lyrics_hpp */
