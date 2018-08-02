#include "character.h"

void character::setup(Bit::Multimedia aura, const Vec2f &pos, const int &id, int currentState) {
	pos_ = pos;
	id_ = id;
	currentState_ = currentState;
	oldState_ = currentState;
	aura_ = aura;
}

bool character::isStateChange() {
	return currentState_ != oldState_;
}

bool character::hasEffect(string effName) {
	for (std::list<std::pair<string, Bit::Multimedia>>::iterator it = effects_.begin(); it != effects_.end(); ++it) {
		if (it->first == effName) {
			return true;
		}
	}
	return false;
}

void character::addEffect(string effName, Bit::Multimedia effect) {
	effects_.push_back(std::pair<string, Bit::Multimedia>(effName, effect));
	effects_.back().second.play();
}

void character::addEffect(string effName, Bit::Multimedia effect, Bit::DisplayArea da) {
	effects_.push_back(std::pair<string, Bit::Multimedia>(effName,effect));
	effects_.back().second.setDisplayArea(da);
	effects_.back().second.play();
}

void character::update() {
	if (!aura_.isPlaying()) {
		aura_.play();
	}
	std::vector<std::list<std::pair<string, Bit::Multimedia>>::iterator> removeIts;
	for (std::list<std::pair<string, Bit::Multimedia>>::iterator it = effects_.begin(); it != effects_.end(); ++it) {
		if (it->second.isDone()) {
			removeIts.push_back(it);
		}
	}
	for (int i = 0; i < removeIts.size(); i++) {
		effects_.erase(removeIts[i]);
	}
}

void character::draw() {
	if (currentState_ != 3) {
		aura_.draw();
	}
	for (std::list<std::pair<string, Bit::Multimedia>>::iterator it = effects_.begin(); it != effects_.end(); ++it) {
		it->second.draw();
	}
}