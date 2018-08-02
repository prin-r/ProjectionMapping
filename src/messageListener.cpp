#include "messageListener.h"
#include "cinder/app/AppBasic.h"

using namespace std;

void messageListener::readConfig(Bit::JsonTree* tree)
{
	portNo_ = tree->getChildPtr("portNo")->getValueT<int>();
	numberOfPlayerParameters_ = tree->getChildPtr("numberOfPlayerParameters")->getValueT<int>();
	numberOfEnemyParameters_ = tree->getChildPtr("numberOfEnemyParameters")->getValueT<int>();
}

void messageListener::readParams(Bit::JsonTree* tree, Bit::ParamsRef params)
{
}

void messageListener::setup(Bit::Multimedia ar, Bit::Multimedia ab)
{
	receiveSomeThing_ = false;
	listener_.setup(portNo_);
	character player;
	auraBlue_ = ab;
	auraRed_ = ar;
	auraBlue_.setup();
	auraRed_.setup();
	player.setup(auraBlue_, Vec2f(0.0f, 0.0f), -1, 0);
	listOfCurPos_[-1] = player;

}

void messageListener::update()
{
	receiveSomeThing_ = false;
	while (listener_.hasWaitingMessages()) {
		osc::Message message;
		listener_.getNextMessage(&message);
		int numData = message.getNumArgs();
		if (numData > 0) {
			std::set<int> setOfID;
			std::set<int> setOfLossID;

			for (int i = numberOfPlayerParameters_; i < numData; i += numberOfEnemyParameters_) {
				if (i < numData) {
					setOfID.insert(static_cast<int>(message.getArgAsFloat(i)));
				}
			}
			for (std::map<int, character>::iterator it = listOfCurPos_.begin(); it != listOfCurPos_.end(); ++it) {
				if (it->first < 0) continue;
				if (!(setOfID.find(it->first) != setOfID.end())) {
					setOfLossID.insert(it->first);
				}
			}
			for (std::set<int>::iterator it = setOfLossID.begin(); it != setOfLossID.end(); ++it) {
				if (*it < 0) continue;
				listOfCurPos_.erase(listOfCurPos_.find(*it));
			}
			for (int i = numberOfPlayerParameters_; i < numData; i += numberOfEnemyParameters_) {
				if (i + 3 < numData) {
					int id = static_cast<int>(message.getArgAsFloat(i));
					if (listOfCurPos_.find(id) != listOfCurPos_.end()) {
						listOfCurPos_[id].currentState_ = static_cast<int>(message.getArgAsFloat(i + 3));
						listOfCurPos_[id].pos_ = Vec2f(message.getArgAsFloat(i + 1), message.getArgAsFloat(i + 2));
					}
					else {
						character enemy;
						enemy.setup(auraRed_, Vec2f(message.getArgAsFloat(i + 1), message.getArgAsFloat(i + 2)), id, static_cast<int>(message.getArgAsFloat(i + 3)));
						listOfCurPos_[id] = enemy;
					}
				}
			}

			listOfCurPos_[-1].id_ = -1;
			listOfCurPos_[-1].pos_ = Vec2f(message.getArgAsFloat(0), message.getArgAsFloat(1));
			listOfCurPos_[-1].currentState_ = message.getArgAsFloat(numberOfPlayerParameters_ - 1);
		}
		receiveSomeThing_ = true;
	}
	for (std::map<int, character>::iterator it = listOfCurPos_.begin(); it != listOfCurPos_.end(); ++it) {
		it->second.update();
	}
}