#include "particles.h"

void particles::setup() {

}

void particles::draw() {
	for (std::list<Bit::Multimedia>::iterator it = effects_.begin(); it != effects_.end(); ++it) {
		it->draw();
	}
}

void particles::addEffect(Bit::Multimedia effect) {
	effects_.push_back(effect);
	effects_.back().play();
}

void particles::addEffect(Bit::Multimedia effect, Bit::DisplayArea da) {
	effects_.push_back(effect);
	effects_.back().setDisplayArea(da);
	effects_.back().play();
}

void particles::update() {
	std::vector<std::list<Bit::Multimedia>::iterator> removeIts;
	for (std::list<Bit::Multimedia>::iterator it = effects_.begin(); it != effects_.end(); ++it) {
		if (it->isDone()) {
			removeIts.push_back(it);
		}
	}
	for (int i = 0; i < removeIts.size(); i++) {
		effects_.erase(removeIts[i]);
	}
}