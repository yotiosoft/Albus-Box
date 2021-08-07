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
	volume = 1.0;
}

void Player::open(FilePath audio_filepath) {
	audio_files << new Audio(audio_filepath);
	audio_files.back()->setVolume(volume);
	audio_files_path << audio_filepath;
	
	stop();
	current_track = (int)audio_files.size()-1;
	play();
}

void Player::setLoop(bool enable) {
	audio_files[current_track]->setLoop(enable);
}

bool Player::play() {
	if (!isOpened()) {
		return false;
	}
	
	status = PlayerStatus::Play;
	audio_files[current_track]->play();
	
	return true;
}

bool Player::playFromBegin() {
	if (!isOpened()) {
		return false;
	}
	
	status = PlayerStatus::Play;
	audio_files[current_track]->setPosSample(0);
	audio_files[current_track]->play();
	
	return true;
}

bool Player::playing() {
	if (!isOpened()) {
		return false;
	}
	
	if (status == PlayerStatus::Play && !audio_files[current_track]->isPlaying()) {
		next();
	}
	
	return status == PlayerStatus::Play;
}

bool Player::pause() {
	if (!isOpened()) {
		return false;
	}
	
	status = PlayerStatus::Pause;
	audio_files[current_track]->pause();
	
	return true;
}

bool Player::stop() {
	if (!isOpened()) {
		return false;
	}
	
	status = PlayerStatus::Stop;
	audio_files[current_track]->stop();
	
	return true;
}

void Player::previous() {
	if (!isOpened()) {
		return;
	}
	
	// 現在再生中の曲の再生位置が1.0秒以上なら0.0秒に戻る
	if (audio_files[current_track]->posSec() >= 1.0) {
		audio_files[current_track]->setPosSec(0.0);
		return;
	}
	
	// そうでなければ（前の曲へボタンが2度押されたら）前の曲へ
	// 現在の曲をストップ
	stop();
	
	// 前の曲へ（現在の曲がリストの先頭ならリストの最後の曲へ）
	current_track --;
	if (current_track == -1) {
		current_track = (int)audio_files.size()-1;
	}
	
	// 再生
	playFromBegin();
}

void Player::next() {
	if (!isOpened()) {
		return;
	}
	
	if (current_track == 0 && audio_files.size() == 1) {	// 1曲しかリストに読み込まれていない場合
		audio_files[current_track]->setPosSec(0.0);			// 0.0秒に戻る
		play();
		return;
	}
	
	// 現在の曲をストップ
	stop();
	
	// 次の曲へ（現在の曲がリストの最後ならリストの先頭の曲へ）
	current_track ++;
	if (current_track == audio_files.size()) {
		current_track = 0;
	}
	
	// 再生
	playFromBegin();
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
	audio_files[current_track]->setPosSample(skip_pos * audio_files[current_track]->samples());
	
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
		af->setVolume(volume);
	}
	
	return true;
}

String Player::getTitle() {
	if (!isOpened()) {
		return U"";
	}
	return  FileSystem::BaseName(audio_files_path[current_track]);
}

int Player::getPlayPosSec() {
	if (!isOpened()) {
		return 0;
	}
	return audio_files[current_track]->posSec();
}

int Player::getPlayPosTimeMin() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track]->posSec() / 60;
}

int Player::getPlayPosTimeSec() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track]->posSec() % 60;
}

double Player::getPlayPosNorm() {
	if (!isOpened()) {
		return 0.0;
	}
	return (double)audio_files[current_track]->posSample() / audio_files[current_track]->samples();
}

int64 Player::getPlayPosSample() {
	if (!isOpened()) {
		return 0;
	}
	return audio_files[current_track]->posSample();
}

int Player::getTotalTimeMin() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track]->lengthSec() / 60;
}

int Player::getTotalTimeSec() {
	if (!isOpened()) {
		return 0;
	}
	return (int)audio_files[current_track]->lengthSec() % 60;
}

void Player::fft(FFTResult& fft) {
	if (!isOpened())
		return;
	
	FFT::Analyze(fft, *audio_files[current_track]);
}

bool Player::isOpened() {
	if (current_track == -1) {
		return false;
	}
	return true;
}

void Player::free() {
	// 再生中の曲を停止
	stop();
	
	for (auto af : audio_files) {
		af->release();
		delete(af);
	}
}
