//
//  Structs.hpp
//  Cities-Box
//
//  Created by YotioSoft on 2021/03/19.
//

#ifndef Structures_hpp
#define Structures_hpp

#include <stdio.h>

using namespace std;

// 画面上の座標格納
typedef struct PositionStruct {
	int x;
	int y;
} PositionStruct;

// ファイル情報
typedef struct FileStruct {
	string file_path;
	
	string folder_path;
	string folder_name;
	string file_name;
} FileStruct;

#endif /* Structs_hpp */
