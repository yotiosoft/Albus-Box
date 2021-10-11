//
//  Player.hpp
//  Albus-Box
//
//  Created by YotioSoft on 2021/08/07.
//

#ifndef Player_hpp
#define Player_hpp

#include <Siv3D.hpp>
#include <Siv3D/MD5.hpp>
#include <Specific.hpp>

#include <iostream>
#include <thread>

namespace PlayerStatus {
	enum Type {
		Stop	= -1,
		Pause	= 0,
		Play	= 1
	};
}

struct AudioFileProfile {
	String title;
	String artist_name;
	FilePath thumbnail_image_filepath;
};

struct AudioStruct {
	uint64 hash;
	Audio* audio;
	FilePath file_path;
	bool isOpened = false;
};

class Player {
public:
	// コンストラクタ
	Player();
	
	// 曲を開く
	void audioRegister(FilePath audio_filepath);
	void open(int num);
	void openAndPlay(FilePath audio_filepath);

	//　曲を閉じる
	void close(int num);

	// 前後の曲を含めて読み込み
	void openAudioFiles(int num);
	
	// 再生
	bool play();
	bool play(int num);
	bool playFromBegin();
	bool playFromBegin(int num);
	
	// 再生中の処理
	bool playing();
	
	// 一時停止
	bool pause();
	
	// 再生停止
	bool stop();
	
	// 前の曲へ
	void previous();
	
	// 次の曲へ
	void next();

	// 曲を遷移したときの処理
	void move(int num);
	
	// 再生位置変更
	bool seekTo(double skip_pos);
	
	// 現在の音量を返す
	double getVolume();
	
	// 音量変更
	bool changeVolumeTo(double volume_norm);
	
	// 曲のタイトルを返す
	String getTitle(int num);
	String getTitle();
	pair<Array<String>, Array<bool>> getTitleList();

	// タイトルを編集
	void editTitle(String new_title);

	// サムネイル画像を返す
	Texture& getThumbnailTexture();
	Image getDefdaultThumbnailImage();

	// サムネイル画像を設定
	void setThumbnailImage(FilePath thumbnail_image_filepath);

	// サムネイル画像の読み込み・リサイズ処理
	void loadThumbnailImage();
	
	// 再生位置を返す(秒)
	int getPlayPosSec();
	
	// 時分で再生位置を返す
	int getPlayPosTimeMin();
	int getPlayPosTimeSec();
	
	// 再生位置を返す(0.0~1.0)
	double getPlayPosNorm();
	
	// 再生位置を返す(フレーム)
	int64 getPlayPosSample();
	
	// 時分で曲の長さを返す
	int getTotalTimeMin();
	int getTotalTimeSec();

	// 現在の再生状態を返す
	PlayerStatus::Type getStatus();

	// 波形の表示状態
	bool isShowWaveEnabled();
	void setShowWave(bool enable);

	// ループ再生
	bool isLoopEnabled();
	void setLoop(bool enable);
	
	// 高速フーリエ変換
	void fft(FFTResult& fft);
	
	// ファイルが開かれているか否か
	bool isOpened();
	
	// 設定ファイルを読み込み
	void loadSettings();
	
	// 設定ファイルを保存
	void saveSettings();
	
	// 楽曲情報を読み込み
	void loadAudioProfiles();
	
	// 楽曲情報を保存
	void saveAudioProfiles();

	// プレイリストを読み込み
	void loadPlayList(FilePath playlist_filepath);

	// プレイリストを保存
	void savePlayList();
	
	// 解放
	void free();
	
private:
	Array<AudioStruct> audio_files;
	Array<FilePath> audio_files_path;
	map<uint64, AudioFileProfile> audio_files_profile;
	int current_track;
	PlayerStatus::Type status;
	double volume;
	bool show_wave;
	bool loop;

	// サムネイル関係
	const int thumbnail_size = 260;
	Texture default_thumbnail_texture;
	Texture current_track_thumbnail_texture;
};

#endif /* Player_hpp */
