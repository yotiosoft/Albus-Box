//
//  OtherFunctions.cpp
//  Cities Box
//
//  Created by YotioSoft on 2019/11/27.
//

#include "OtherFunctions.hpp"

void addTextFile(string filePath, string str) {
	ofstream ofs;
	ofs.open(filePath, ios::out | ios::binary | ios::app);
	
	ofs << str;
}

void debugLog(String str) {
	addTextFile("log.txt", str.toUTF8()+"\n");;
}
