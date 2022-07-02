//
//  Player.cpp
//  Albus-Box
//
//  Created by YotioSoft on 2021/08/07.
//

#include "Player.hpp"

Player::Player() {
	current_track = -1;
	status = PlayerStatus::Stop;
	lyrics_exist = false;
	
	loadSettings();
	loadAudioProfiles();

	// 標準のサムネイル画像を生成
	default_thumbnail_texture = Texture(getDefdaultThumbnailImage());
	current_track_thumbnail_texture = &default_thumbnail_texture;
}

void Player::audioRegister(FilePath audio_filepath) {
	uint64 new_audio_hash = MD5::FromFile(audio_filepath).hash();
	AudioStruct new_audio_struct{ new_audio_hash, nullptr, audio_filepath };

	if (!FileSystem::Exists(audio_filepath)) {
		return;
	}

	audio_files << new_audio_struct;
	audio_files_path << audio_filepath;
	has_lyrics << false;

	openLyricsFile(audio_files.size()-1);
}

void Player::open(int num) {
	Audio* new_audio_file = new Audio(audio_files[num].file_path);

	if (!FileSystem::Exists(audio_files[num].file_path)) {
		return;
	}

	audio_files[num].audio = new_audio_file;
	audio_files[num].isOpened = true;
}

void Player::openAndPlay(FilePath audio_filepath) {
	audioRegister(audio_filepath);

	stop();

	play((int)audio_files.size() - 1);
}

void Player::openLyricsFile(int num) {
	// ハッシュ値に対応する歌詞ファイルがlyricsディレクトリ内にあれば開く
	String lyrics_filepath = U"{}/{}.lyrics"_fmt(specific::getLyricsDirPath(), audio_files[num].hash);
	if (!has_lyrics[num] && FileSystem::Exists(lyrics_filepath)) {
		lyrics[audio_files[num].hash] = Lyrics(lyrics_filepath);
		has_lyrics[num] = true;
	}
	else {
		has_lyrics[num] = false;
	}
}

void Player::close(int num) {
	if (num < 0) {
		return;
	}
	
	audio_files[num].audio->release();
	audio_files[num].isOpened = false;
}

void Player::openAudioFiles(int num) {
	// 前後含め3曲分読み込み
	for (int i = -RANGE; i <= RANGE; i++) {
		int track_num = getTrackNumber(num, i);
		if (!audio_files[track_num].isOpened) {
			audio_files[track_num].isOpened = true;
			threads[track_num] = std::thread([this, track_num]() { this->open(track_num); });
		}
	}
}

void Player::closeAudioFiles(int num) {
	for (int i = 0; i < audio_files.size(); i++) {
		if (audio_files[i].isOpened) {
			int distance = abs(i - num) % audio_files.size();
			if (distance > RANGE) {
				close(i);
				//Console << U"Close: " << i << U" num: " << num << U" distance: " << distance;
			}
		}
	}
}

bool Player::play() {
	return play(current_track);
}

bool Player::play(int num) {
	if (!isOpened()) {
		return false;
	}

	if (num != current_track && num < audio_files.size()) {
		pause();
		close(current_track);
		move(num);
		audio_files[current_track].audio->seekSamples(0);
	}

	status = PlayerStatus::Play;
	audio_files[current_track].audio->play();

	return true;
}

bool Player::playFromBegin() {
	return playFromBegin(current_track);
}

bool Player::playFromBegin(int num) {
	if (!isOpened()) {
		return false;
	}

	if (num != current_track && num < audio_files.size()) {
		stop();
	}

	play(num);

	return true;
}

bool Player::playing() {
	if (!isOpened()) {
		return false;
	}
	
	if (status == PlayerStatus::Play && !audio_files[current_track].audio->isPlaying()) {
		next();
	}
	
	return status == PlayerStatus::Play;
}

bool Player::pause() {
	if (!isOpened()) {
		return false;
	}
	if (current_track < 0) {
		return false;
	}
	
	status = PlayerStatus::Pause;
	audio_files[current_track].audio->pause();
	
	return true;
}

bool Player::stop() {
	if (!isOpened()) {
		return false;
	}
	if (current_track < 0) {
		return false;
	}
	
	status = PlayerStatus::Stop;
	audio_files[current_track].audio->stop();
	
	return true;
}

void Player::previous() {
	if (!isOpened()) {
		return;
	}
	
	// 現在再生中の曲の再生位置が1.0秒以上なら0.0秒に戻る
	if (audio_files[current_track].audio->posSec() >= 1.0) {
		audio_files[current_track].audio->seekTime(0.0);
		return;
	}
	
	// そうでなければ（前の曲へボタンが2度押されたら）前の曲へ
	// 現在の曲をストップ
	PlayerStatus::Type before_status = status;
	stop();
	
	// 前の曲へ（現在の曲がリストの先頭ならリストの最後の曲へ）
	move(current_track - 1);
	
	if (before_status == PlayerStatus::Play) {
		// 再生
		playFromBegin();
	}
	else {
		pause();
	}
}

void Player::next() {
	if (!isOpened()) {
		return;
	}
	if (!auto_move_to_next) {
		status = PlayerStatus::Pause;
		return;
	}
	
	if (current_track == 0 && audio_files.size() == 1 && loop) {	// 1曲しかリストに読み込まれていない場合
		audio_files[current_track].audio->seekTime(0.0);					// 0.0秒に戻る
		play();
		return;
	}
	else if (current_track == 0 && audio_files.size() == 1 && !loop) {
		audio_files[current_track].audio->seekTime(0.0);
		pause();
		return;
	}
	
	// 現在の曲をストップ
	PlayerStatus::Type before_status = status;
	stop();
	
	// 次の曲へ（現在の曲がリストの最後ならリストの先頭の曲へ）
	move(current_track + 1);
	
	if (before_status == PlayerStatus::Play) {
		// 再生
		playFromBegin();
	}
	else {
		pause();
	}
}

void Player::move(int num) {
	// 前の曲を閉じる
	if (num == audio_files.size()) {
		num = 0;
	}
	if (num == -1) {
		num = (int)audio_files.size() - 1;
	}

	current_track = num;

	// サムネイル画像の取得
	loadThumbnailImage();

	for (auto it = threads.begin(); it != threads.end(); it++) {
		it->second.join();
	}
	threads.clear();

	// 不要なファイルを閉じる
	closeAudioFiles(num);

	// ファイルを開く
	openAudioFiles(num);

	if (threads.count(num) > 0) {
		threads[num].join();
		threads.erase(num);
	}

	// 設定を反映
	reflectSettings(current_track);

	return;
}

void Player::setAutoMoveToNext(bool arg) {
	auto_move_to_next = arg;
}

bool Player::reflectSettings(int num) {
	if (audio_files.size() <= num) {
		return false;
	}

	// ボリュームの反映
	audio_files[num].audio->setVolume(volume);

	// ループ設定の反映
	audio_files[num].audio->setLoop(loop);

	return true;
}

bool Player::seekTo(double skip_pos) {
	if (!isOpened()) {
		return false;
	}
	
	// 0.0~1.0の範囲外ならエラー
	if (skip_pos < 0.0 || skip_pos > 1.0) {
		return false;
	}
	
	// 再生位置変更
	audio_files[current_track].audio->seekSamples(skip_pos * audio_files[current_track].audio->samples());
	
	return true;
}

double Player::getVolume() {
	return volume;
}

bool Player::changeVolumeTo(double volume_norm) {
	if (volume_norm < 0.0 || volume_norm > 1.0) {
		return false;
	}
	
	// ボリューム変更
	volume = volume_norm;

	if (audio_files.size() == 0) {
		return true;
	}

	audio_files[current_track].audio->setVolume(volume);
	
	return true;
}

int Player::getTrackNumber(int current, int offset) {
	int ret = current + offset;

	if (ret >= audio_files.size()) {
		ret %= audio_files.size();
	}
	else if (ret < 0) {
		ret %= audio_files.size();
	}

	return ret;
}

String Player::getTitle(int num) {
	if (!isOpened()) {
		return U"";
	}

	if (audio_files_profile.count(audio_files[num].hash) == 0) {
		// ファイル情報が存在しなければファイル名を返す
		return FileSystem::BaseName(audio_files_path[num]);
	}

	// ファイル情報が存在するなら設定されたタイトルを返す
	if (audio_files_profile[audio_files[num].hash].title.length() > 0) {
		return audio_files_profile[audio_files[num].hash].title;
	}

	// ファイル情報が存在してもタイトルが設定されていなければファイル名を返す
	return FileSystem::BaseName(audio_files_path[num]);
}

String Player::getTitle() {
	return getTitle(current_track);
}

pair<Array<String>, Array<bool>> Player::getTitleList() {
	Array<String> title_list;
	for (int i = 0; i < audio_files.size(); i++) {
		title_list << getTitle(i);
	}

	Array<bool> playing(audio_files.size(), false);
	if (status == PlayerStatus::Play) {
		playing[current_track] = true;
	}

	return pair<Array<String>, Array<bool>>{title_list, playing};
}

void Player::editTitle(String new_title) {
	if (getTitle(current_track) == new_title || new_title == U"") {
		return;
	}

	// ファイル情報をハッシュ値とともに格納
	audio_files_profile[audio_files[current_track].hash].title = new_title;
	
	// 設定ファイルを上書き保存
	saveAudioProfiles();
}

Texture* Player::getThumbnailTexture() {
	return current_track_thumbnail_texture;
}

Image Player::getDefdaultThumbnailImage() {
	Image thumbnail_image(thumbnail_size, thumbnail_size, Palette::White);
	Color thumbnail_color1(Palette::Orangered);
	Color thumbnail_color2(Palette::Mediumpurple);

	for (int y = 0; y < thumbnail_image.height(); y++) {
		for (int x = 0; x < thumbnail_image.width(); x++) {
			double t = (double)(x + y) / (thumbnail_image.width() + thumbnail_image.height() - 2);
			thumbnail_image[y][x] = thumbnail_color1.lerp(thumbnail_color2, t);
		}
	}

	return thumbnail_image;
}

void Player::setThumbnailImage(FilePath thumbnail_image_filepath) {
	if (thumbnail_image_filepath.length() > 0 && FileSystem::Exists(thumbnail_image_filepath)) {
		audio_files_profile[audio_files[current_track].hash].thumbnail_image_filepath = thumbnail_image_filepath;

		if (!audio_files_profile[audio_files[current_track].hash].thumbnail_texture.isEmpty()) {
			audio_files_profile[audio_files[current_track].hash].thumbnail_texture.release();
		}

		loadThumbnailImage();

		saveAudioProfiles();
	}
}

void Player::loadThumbnailImage() {
	if (audio_files_profile.count(audio_files[current_track].hash) > 0) {
		if (audio_files_profile[audio_files[current_track].hash].thumbnail_image_filepath.length() > 0
			&& FileSystem::Exists(audio_files_profile[audio_files[current_track].hash].thumbnail_image_filepath)) {

			if (audio_files_profile[audio_files[current_track].hash].thumbnail_texture.isEmpty()) {
				if (!FileSystem::Exists(audio_files_profile[audio_files[current_track].hash].thumbnail_image_filepath)) {
					// ファイルが存在しなければデフォルトのサムネイル画像
					current_track_thumbnail_texture = &default_thumbnail_texture;
					return;
				}

				Image thumbnail_image_temp(audio_files_profile[audio_files[current_track].hash].thumbnail_image_filepath);

				if (thumbnail_image_temp.width() > thumbnail_image_temp.height()) {
					thumbnail_image_temp = thumbnail_image_temp.fitted(Size(thumbnail_image_temp.width() / thumbnail_image_temp.height() * thumbnail_size * 2, thumbnail_size * 2));
				}
				else {
					thumbnail_image_temp = thumbnail_image_temp.fitted(Size(thumbnail_size * 2, thumbnail_image_temp.height() / thumbnail_image_temp.width() * thumbnail_size * 2));
				}

				audio_files_profile[audio_files[current_track].hash].thumbnail_texture = Texture(thumbnail_image_temp);
			}
			current_track_thumbnail_texture = &audio_files_profile[audio_files[current_track].hash].thumbnail_texture;

			return;
		}
	}

	// ファイル情報が存在しなければデフォルトのサムネイル画像
	current_track_thumbnail_texture = &default_thumbnail_texture;

	return;
}

int Player::getPlayPosSec() {
	if (!isOpened()) {
		return 0;
	}
	return audio_files[current_track].audio->posSec();
}

Timestamp Player::getPlayPosTime() {
	if (!isOpened()) {
		return Timestamp{ 0, 0 };
	}

	double pos_sec = audio_files[current_track].audio->posSec();
	//Console << pos_sec;
	return Timestamp{ (int)pos_sec / 60, (int)pos_sec % 60 };
}

double Player::getPlayPosNorm() {
	if (!isOpened()) {
		return 0.0;
	}
	return (double)audio_files[current_track].audio->posSample() / audio_files[current_track].audio->samples();
}

int64 Player::getPlayPosSample() {
	if (!isOpened()) {
		return 0;
	}
	return audio_files[current_track].audio->posSample();
}

Timestamp Player::getTotalTime() {
	if (!isOpened()) {
		return Timestamp{ 0, 0 };
	}

	double length_sec = audio_files[current_track].audio->lengthSec();
	return Timestamp{ (int)length_sec / 60, (int)length_sec % 60 };
}

bool Player::lyricsExist() {
	return has_lyrics[current_track] && isOpened();
}

bool Player::updateLyrics() {
	if (!isOpened() || !has_lyrics[current_track]) {
		lyrics_begin_time = -1.0;
		lyrics_end_time = -1.0;
		return false;
	}
	
	if ((temp_lyrics = lyrics[audio_files[current_track].hash].get_lyrics(audio_files[current_track].audio->posSec())) != before_lyrics) {
		current_lyrics = temp_lyrics;
		before_lyrics = current_lyrics;
		lyrics_begin_time = lyrics[audio_files[current_track].hash].get_begin_time();
		lyrics_end_time = lyrics[audio_files[current_track].hash].get_end_time();
		return true;
	}

	return false;
}

String Player::getLyrics() {
	if (isOpened() && has_lyrics[current_track]) {
		return current_lyrics;
	}

	return U"";
}

int Player::getLyricsDisplayAlphaColor() {
	if (!isOpened() || !has_lyrics[current_track]) {
		return 0;
	}

	if (lyrics_begin_time < 0 || lyrics_end_time < 0) {
		return 0;
	}

	double play_sec = audio_files[current_track].audio->posSec();
	double trans_len = 0.2;

	if (0.0 <= play_sec - lyrics_begin_time && play_sec - lyrics_begin_time <= trans_len) {
		return (play_sec - lyrics_begin_time) * 255 / trans_len;
	}

	if (0.0 <= lyrics_end_time - play_sec && lyrics_end_time - play_sec <= trans_len) {
		return (lyrics_end_time - play_sec) * 255 / trans_len;
	}

	return 255;
}

Lyrics* Player::getLyricsObj() {
	if (isOpened() && has_lyrics[current_track]) {
		return &lyrics[audio_files[current_track].hash];
	}

	return NULL;
}

PlayerStatus::Type Player::getStatus() {
	return status;
}

bool Player::isShowWaveEnabled() {
	return show_wave;
}

void Player::setShowWave(bool enable) {
	show_wave = enable;
}

bool Player::isLoopEnabled() {
	return loop;
}

void Player::setLoop(bool enable) {
	if (enable == loop)
		return;

	loop = enable;

	if (!isOpened())
		return;
	
	audio_files[current_track].audio->pause();

	int64 play_samples = audio_files[current_track].audio->posSample();

	audio_files[current_track].audio->setLoop(loop);

	if (status == PlayerStatus::Play) {
		audio_files[current_track].audio->seekSamples(play_samples);
		audio_files[current_track].audio->play();
	}
}

void Player::fft(FFTResult& fft) {
	if (!isOpened())
		return;
	
	FFT::Analyze(fft, *audio_files[current_track].audio);
}

bool Player::isOpened() {
	if (audio_files.size() == 0) {
		return false;
	}
	return true;
}

void Player::loadSettings() {
	if (!FileSystem::Exists(specific::getSettingFilePath())) {
		volume = 1.0;
		show_wave = true;
		loop = false;
		
		return;
	}
	
	JSON j_setting = JSON::Load(specific::getSettingFilePath());
	
	volume = j_setting[U"volume"].get<double>();
	show_wave = j_setting[U"show_wave"].get<bool>();
	loop = j_setting[U"loop"].get<bool>();
}

void Player::saveSettings() {
	JSON j_setting;

	j_setting[U"volume"] = volume;
	j_setting[U"show_wave"] = show_wave;
	j_setting[U"loop"] = loop;
	
	j_setting.save(specific::getSettingFilePath());
}

void Player::loadAudioProfiles() {
	if (!FileSystem::Exists(specific::getAudioProfilesFilePath())) {
		return;
	}

	JSON audio_profile = JSON::Load(specific::getAudioProfilesFilePath());
	
	for (auto audio_profile : audio_profile[U"audio_profiles"].arrayView()) {
		uint64 hash = audio_profile[U"hash"].get<uint64>();
		audio_files_profile[hash].title = audio_profile[U"title"].getString();
		audio_files_profile[hash].artist_name = audio_profile[U"artist_name"].getString();
		audio_files_profile[hash].thumbnail_image_filepath = audio_profile[U"thumbnail_image_filepath"].getString();
	}
}

void Player::saveAudioProfiles() {
	JSON audio_profiles;

	Array<JSON> audio_profile_json_array;
	for (auto audio_profile : audio_files_profile) {
		JSON audio_profile_json;

		audio_profile_json[U"hash"] = audio_profile.first;

		audio_profile_json[U"title"] = audio_profile.second.title;
		audio_profile_json[U"artist_name"] = audio_profile.second.artist_name;
		audio_profile_json[U"thumbnail_image_filepath"] = audio_profile.second.thumbnail_image_filepath;

		audio_profile_json_array << audio_profile_json;
	}

	audio_profiles[U"audio_profiles"] = audio_profile_json_array;
	
	audio_profiles.save(specific::getAudioProfilesFilePath());
}

void Player::loadPlayList(FilePath playlist_filepath) {
	JSON playlist = JSON::Load(playlist_filepath);

	if (isOpened()) {
		// 再生中の曲をストップ
		stop();

		// 現在のプレイリストをクリア
		audio_files.clear();
		audio_files_path.clear();
	}

	// プレイリスト読み込み
	for (auto audio_filepath : playlist[U"list"].arrayView()) {
		audioRegister(audio_filepath.getString());
	}

	// 最初の曲を再生
	move(0);
	play();
}

void Player::savePlayList() {
	JSON playlist;

	Array<JSON> playlist_json_array;
	for (auto audio_filepath : audio_files_path) {
		playlist_json_array << JSON(audio_filepath);
	}

	playlist[U"list"] = playlist_json_array;

	Array<FileFilter> ff = { {U"プレイリスト", {U"playlist"}} };
	String filePathTemp;
	if (const auto save = Dialog::SaveFile(ff)) {
		playlist.save(save.value());
	}
}

void Player::free() {
	// 再生中の曲を停止
	stop();

	// スレッド処理の無効化
	for (auto it = threads.begin(); it != threads.end(); it++) {
		it->second.detach();
	}
	threads.clear();
	
	// Audioの解放
	for (auto af : audio_files) {
		if (af.isOpened && af.audio != nullptr)
			af.audio->release();
		delete(af.audio);
	}
}
