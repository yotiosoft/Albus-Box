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
	
	loadSettings();
	loadAudioProfiles();

	// 標準のサムネイル画像を生成
	default_thumbnail_texture = Texture(getDefdaultThumbnailImage());
	current_track_thumbnail_texture = default_thumbnail_texture;
}

void Player::open(FilePath audio_filepath) {
	Audio* new_audio_file = new Audio(audio_filepath);
	uint64 new_audio_hash = Hash::XXHash3(&audio_filepath);
	AudioStruct new_audio_struct{ new_audio_hash, new_audio_file };

	audio_files << new_audio_struct;
	audio_files.back().audio->setVolume(volume);
	audio_files.back().audio->setLoop(loop);
	audio_files_path << audio_filepath;
	
	stop();
	move((int)audio_files.size() - 1);

	play();
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
		move(num);
	}

	status = PlayerStatus::Play;
	audio_files[current_track].audio->seekSamples(0);
	audio_files[current_track].audio->play();

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
	
	status = PlayerStatus::Pause;
	audio_files[current_track].audio->pause();
	
	return true;
}

bool Player::stop() {
	if (!isOpened()) {
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
	if (num == audio_files.size()) {
		num = 0;
	}
	if (num == -1) {
		num = (int)audio_files.size() - 1;
	}

	current_track = num;

	// サムネイル画像の取得
	loadThumbnailImage();

	return;
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
	
	// すべてのデータの適応
	for (auto af : audio_files) {
		af.audio->setVolume(volume);
	}
	
	return true;
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
	// ファイル情報をハッシュ値とともに格納
	audio_files_profile[audio_files[current_track].hash].title = new_title;
	
	// 設定ファイルを上書き保存
	saveAudioProfiles();
}

Texture& Player::getThumbnailTexture() {
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
		loadThumbnailImage();

		saveAudioProfiles();
	}
}

void Player::loadThumbnailImage() {
	if (audio_files_profile.count(audio_files[current_track].hash) > 0) {
		if (audio_files_profile[audio_files[current_track].hash].thumbnail_image_filepath.length() > 0
			&& FileSystem::Exists(audio_files_profile[audio_files[current_track].hash].thumbnail_image_filepath)) {

			Image thumbnail_image_temp(audio_files_profile[audio_files[current_track].hash].thumbnail_image_filepath);

			if (thumbnail_image_temp.width() > thumbnail_image_temp.height()) {
				thumbnail_image_temp = thumbnail_image_temp.fitted(Size(thumbnail_image_temp.width() / thumbnail_image_temp.height() * thumbnail_size * 2, thumbnail_size * 2));
			}
			else {
				thumbnail_image_temp = thumbnail_image_temp.fitted(Size(thumbnail_size * 2, thumbnail_image_temp.height() / thumbnail_image_temp.width() * thumbnail_size * 2));
			}

			current_track_thumbnail_texture = Texture(thumbnail_image_temp);

			return;
		}
	}

	// ファイル情報が存在しなければデフォルトのサムネイル画像
	current_track_thumbnail_texture = default_thumbnail_texture;

	return;
}

int Player::getPlayPosSec() {
	if (!isOpened()) {
		return 0;
	}
	return audio_files[current_track].audio->posSec();
}

int Player::getPlayPosTimeMin() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track].audio->posSec() / 60;
}

int Player::getPlayPosTimeSec() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track].audio->posSec() % 60;
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

int Player::getTotalTimeMin() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track].audio->lengthSec() / 60;
}

int Player::getTotalTimeSec() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track].audio->lengthSec() % 60;
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

	for (auto af : audio_files) {
		af.audio->setLoop(loop);
	}

	if (status == PlayerStatus::Play) {
		std::cout << "to play " << play_samples << std::endl;
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
	if (current_track == -1) {
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

void Player::loadPlayList() {

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
	
	for (auto af : audio_files) {
		af.audio->release();
		delete(af.audio);
	}
}
