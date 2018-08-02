#include "mainPage.h"

void mainPage::readConfig(Bit::JsonTree* tree)
{
	messageListener_.readConfig(tree->getChildPtr("messageListener"));
	bgMedia_.readConfig(tree->getChildPtr("bgMedia"));
	arenaWidthInUE_ = tree->getChildPtr("arenaWidthInUE")->getValueT<float>();
	Bit::JsonTree* ems = tree->getChildPtr("effectMedias");
	for (Bit::JsonTree::Iter l = ems->begin(); l != ems->end(); l++)
	{
		string name = Bit::JsonTree::toBitJsonTreePtr(*l)->getChildPtr("name")->getValueT<string>();
		Bit::Multimedia tmpMedia;
		tmpMedia.readConfig(Bit::JsonTree::toBitJsonTreePtr(*l)->getChildPtr("media"));
		app::console() << name << endl;
		nameToMedia_[name] = tmpMedia;
	}
	mGlsl_ = gl::GlslProg(loadAsset(tree->getChildPtr("vertexShader")->getValueT<string>()), loadAsset(tree->getChildPtr("pixelShader")->getValueT<string>()));
}

void mainPage::readParams(Bit::JsonTree* tree, Bit::ParamsRef params)
{
	//params->addParam<bool>(tree->getChildPtr("shouldHaveShadow"), shouldHaveAura_);

	params->addParam<float>(tree->getChildPtr("debugCircleRadius"), debugCircleRadius_);
	params->addParam<float>(tree->getChildPtr("debugCircleAlpha"), debugCircleAlpha_);

	rectPoints_.push_back(Vec2f(0, 0));
	rectPoints_.push_back(Vec2f(1, 0));
	rectPoints_.push_back(Vec2f(1, 1));
	rectPoints_.push_back(Vec2f(0, 1));
	rectPoints_.shrink_to_fit();

	debugCircleColor_.push_back(Vec3f(1, 1, 1));
	debugCircleColor_.push_back(Vec3f(1, 1, 1));
	debugCircleColor_.push_back(Vec3f(1, 1, 1));
	debugCircleColor_.push_back(Vec3f(1, 1, 1));
	rectPoints_.shrink_to_fit();

	params->addParam<ci::Vec2f>(tree->getChildPtr("rectPoints0"), rectPoints_[0]);
	params->addParam<ci::Vec2f>(tree->getChildPtr("rectPoints1"), rectPoints_[1]);
	params->addParam<ci::Vec2f>(tree->getChildPtr("rectPoints2"), rectPoints_[2]);
	params->addParam<ci::Vec2f>(tree->getChildPtr("rectPoints3"), rectPoints_[3]);

	params->addParam<ci::Vec3f>(tree->getChildPtr("debugCircleColor0"), debugCircleColor_[0]);
	params->addParam<ci::Vec3f>(tree->getChildPtr("debugCircleColor1"), debugCircleColor_[1]);
	params->addParam<ci::Vec3f>(tree->getChildPtr("debugCircleColor2"), debugCircleColor_[2]);
	params->addParam<ci::Vec3f>(tree->getChildPtr("debugCircleColor3"), debugCircleColor_[3]);
}

void mainPage::setup()
{
	w_ = getWindowWidth();
	h_ = getWindowHeight();

	messageListener_.setup(nameToMedia_["heroRed"], nameToMedia_["heroBlue"]);

	bgMedia_.setup();

	for (std::map<string, Bit::Multimedia>::iterator iter = nameToMedia_.begin(); iter != nameToMedia_.end(); ++iter)
	{
		iter->second.setup();
	}

	currentMousePos_ = Vec2i(0,0);
	gl::Fbo::Format csaaFormat;
	csaaFormat.enableMipmapping(true);
	csaaFormat.setSamples(4);
	csaaFormat.setCoverageSamples(8);
	screenFbo_ = gl::Fbo(w_, h_, csaaFormat);

	ci::app::getWindow()->connectMouseDown(&mainPage::mouseDown, this);
	ci::app::getWindow()->connectMouseMove(&mainPage::mouseMove, this);

	rectPointsOriginal_.push_back(Vec2f(0, 0));
	rectPointsOriginal_.push_back(Vec2f(1, 0));
	rectPointsOriginal_.push_back(Vec2f(1, 1));
	rectPointsOriginal_.push_back(Vec2f(0, 1));
	rectPointsOriginal_.shrink_to_fit();

	resetRectArea_ = false;
	/*ci::app::getWindow()->connectMouseUp(&mainPage::mouseUp, this);

	ci::app::getWindow()->connectTouchesBegan(&mainPage::touchesBegan, this);
	ci::app::getWindow()->connectTouchesEnded(&mainPage::touchesEnded, this);*/
}

void mainPage::resetRectProjection2Default() {
	for (int i = 0; i < rectPointsOriginal_.size(); i++) {
		rectPoints_[i].x = rectPointsOriginal_[i].x;
		rectPoints_[i].y = rectPointsOriginal_[i].y;
	}
}

void mainPage::update()
{
	allIDs_ = "";
	messageListener_.update();
	for (std::map<int, character>::iterator it = messageListener_.listOfCurPos_.begin(); it != messageListener_.listOfCurPos_.end(); ++it) {
		allIDs_ += "[" + std::to_string(it->first) + "," + std::to_string(it->second.id_) + "] , ";
		if (it->second.oldState_ != it->second.currentState_) {

			string effectName = "";

			if (it->second.id_ < 0) {
				switch (it->second.currentState_) {
				case 0:
					effectName = "";
					break;
				case 1:
					effectName = "effect_01";
					break;
				case 2:
					effectName = "effect_01";
					break;
				case 3:
					break;
				case 4:
					break;
				case 5:
					break;
				default:
					effectName = "";
					break;
				}

				if (effectName != "") {
					Bit::DisplayArea da = nameToMedia_[effectName].getDisplayArea();
					Vec2f drawPos = it->second.pos_ / arenaWidthInUE_ + Vec2f(0.5f, 0.5f);//it->second.pos_ * Vec2f(w_, h_) / arenaWidthInUE_ + Vec2f(w_, h_) * 0.5f;
					da.setNormalizedPosition(drawPos);
					it->second.addEffect(effectName, nameToMedia_[effectName], da);
				}

			}
			else {
				switch (it->second.currentState_) {
				case 0:
					effectName = "";
					break;
				case 1:
					effectName = "effect_02";
					break;
				case 2:
					effectName = "effect_03";
					break;
				case 3:
					effectName = "effect_08";
					break;
				case 4:
					effectName = "effect_04";
					break;
				case 5:
					effectName = "effect_09";
					break;
				default:
					effectName = "";
					break;
				}

				if (effectName != "") {
					Bit::DisplayArea da = nameToMedia_[effectName].getDisplayArea();
					Vec2f drawPos = it->second.pos_ / arenaWidthInUE_ + Vec2f(0.5f, 0.5f);//it->second.pos_ * Vec2f(w_, h_) / arenaWidthInUE_ + Vec2f(w_, h_) * 0.5f;
					da.setNormalizedPosition(drawPos);
					it->second.addEffect(effectName, nameToMedia_[effectName], da);
				}

				if (it->second.currentState_ == 4) {
					if (!it->second.hasEffect("effect_04")) {
						Bit::DisplayArea da = nameToMedia_["effect_04"].getDisplayArea();
						Vec2f drawPos = it->second.pos_ / arenaWidthInUE_ + Vec2f(0.5f, 0.5f);
						da.setNormalizedPosition(drawPos);
						it->second.addEffect("effect_04", nameToMedia_["effect_04"], da);
					}
				}
			}
		}

		for (std::map<int, character>::iterator it = messageListener_.listOfCurPos_.begin(); it != messageListener_.listOfCurPos_.end(); ++it){
			if (it->second.id_ < 0) {
				Bit::DisplayArea da = nameToMedia_["heroBlue"].getDisplayArea();
				Vec2f drawPos = it->second.pos_ / arenaWidthInUE_ + Vec2f(0.5f, 0.5f);
				da.setNormalizedPosition(drawPos);
				it->second.aura_.setDisplayArea(da);
			}
			else {
				Bit::DisplayArea da = nameToMedia_["heroRed"].getDisplayArea();
				Vec2f drawPos = it->second.pos_ / arenaWidthInUE_ + Vec2f(0.5f, 0.5f);
				da.setNormalizedPosition(drawPos);
				it->second.aura_.setDisplayArea(da);
			}
		}

		if (it->second.id_ > -1 && it->second.currentState_ == 4) {
			if (!it->second.hasEffect("effect_04")) {
				Bit::DisplayArea da = nameToMedia_["effect_04"].getDisplayArea();
				Vec2f drawPos = it->second.pos_ / arenaWidthInUE_ + Vec2f(0.5f, 0.5f);
				da.setNormalizedPosition(drawPos);
				it->second.addEffect("effect_04", nameToMedia_["effect_04"], da);
			}
		}

		it->second.oldState_ = it->second.currentState_;
		it->second.update();
	}

	if (debugMode_) {
		if (selectedIndex_ > -1) {
			rectPoints_[selectedIndex_] = Vec2f((currentMousePos_.x + 0.0) / (w_ + 0.0), (currentMousePos_.y + 0.0) / (h_ + 0.0));
		}
	}
	customProjectionMatrix_ = rd_.general2DProjection(rectPoints_[0], rectPoints_[1], rectPoints_[2], rectPoints_[3]);
}

void mainPage::draw()
{
	screenFbo_.bindFramebuffer();
	bgMedia_.draw();
	//gl::drawString(std::to_string(messageListener_.listOfCurPos_.size()), Vec2f(w_ - 60, 0), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	//ps_.draw();
	/*if (shouldHaveAura_) {
		for (std::map<int, character>::iterator it = messageListener_.listOfCurPos_.begin(); it != messageListener_.listOfCurPos_.end(); ++it) {
			if (it->second.id_ < 0) {
				Bit::DisplayArea da = nameToMedia_["heroBlue"].getDisplayArea();
				Vec2f drawPos = it->second.pos_ / arenaWidthInUE_ + Vec2f(0.5f, 0.5f);
				da.setNormalizedPosition(drawPos);
				nameToMedia_["heroBlue"].draw(da);
			}
			else {
				Bit::DisplayArea da = nameToMedia_["heroRed"].getDisplayArea();
				Vec2f drawPos = it->second.pos_ / arenaWidthInUE_ + Vec2f(0.5f, 0.5f);
				da.setNormalizedPosition(drawPos);
				nameToMedia_["heroRed"].draw(da);
			}
		}
	}*/

	for (std::map<int, character>::iterator it = messageListener_.listOfCurPos_.begin(); it != messageListener_.listOfCurPos_.end(); ++it) {
		it->second.draw();
	}

	//gl::drawString(std::to_string(messageListener_.listOfCurPos_.size()), Vec2f(w_ - 500, 0), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	if (debugMode_) {
		for (std::map<int, character>::iterator it = messageListener_.listOfCurPos_.begin(); it != messageListener_.listOfCurPos_.end(); ++it) {
			Vec2f drawPos = it->second.pos_ * Vec2f(w_, h_) / arenaWidthInUE_ + Vec2f(w_, h_) * 0.5f;
			stringstream stream1, stream2;
			stream1 << fixed << setprecision(2) << it->second.pos_.x;
			string sx = stream1.str();
			stream2 << fixed << setprecision(2) << it->second.pos_.y;
			string sy = stream2.str();
			gl::drawString(std::to_string(it->second.id_) + "," + sx + "," + sy + "," + std::to_string(it->second.currentState_), drawPos, ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 20));
		}
		gl::drawString(allIDs_, Vec2f(400, 0), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 20));
	}

	screenFbo_.unbindFramebuffer();

	//gl::draw(screenFbo_.getTexture());

	mGlsl_.bind();
	screenFbo_.getTexture().setFlipped(true);
	screenFbo_.getTexture().enableAndBind();

	mGlsl_.uniform("tex", 0);
	mGlsl_.uniform("basisX", customProjectionMatrix_.getColumn(0));
	mGlsl_.uniform("basisY", customProjectionMatrix_.getColumn(1));
	mGlsl_.uniform("basisZ", customProjectionMatrix_.getColumn(2));

	gl::drawSolidRect(Rectf(0, 0 , w_ , h_));
	mGlsl_.unbind();

	//gl::drawString(allIDs_ + std::to_string(currentMousePos_.x) + "," + std::to_string(currentMousePos_.y), Vec2f(400, 0), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	if (debugMode_) {
		gl::drawString(std::to_string(selectedIndex_), Vec2f(300, 0), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	}

	if (debugMode_) {
		gl::enableAlphaBlending();
		for (int i = 0; i < rectPoints_.size(); i++) {
			gl::color(ColorA(debugCircleColor_[i].x, debugCircleColor_[i].y, debugCircleColor_[i].z, debugCircleAlpha_));
			gl::drawSolidCircle(rectPoints_[i] * Vec2f(w_,h_), debugCircleRadius_);
		}
		gl::color(ColorA(1, 1, 1, 1));
		gl::disableAlphaBlending();
	}

	//Matrix33f test(2, 2, 3, 4, 5, 6, 7, 8, 9);
	//Matrix33f test2(1, 2, 1, 3, 5, 6, 7, -5, -9);
	//rd_.basisToPoints(Vec2f(1,2), Vec2f(3,2), Vec2f(3,0), Vec2f(0,-1));//rd_.transpose(test);//rd_.adj(test);//rd_.multmm(test, test2);
	//Vec3f mv = rd_.mulmv(test, Vec3f(0.1,0.4,-0.25));
	//for (int i = 0; i < 3; i++) {
	//	gl::drawString(to_string(test1x2.m[i * 3]) + "," + to_string(test1x2.m[i * 3 + 1]) + "," + to_string(test1x2.m[i * 3 + 2]), Vec2f(0, 30 * i), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	//}
	//gl::drawString(to_string(mv.x) + "," + to_string(mv.y) + "," + to_string(mv.z), Vec2f(0, 0), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	//gl::drawString(to_string(rd_.det(test)), Vec2f(0, 0), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));

}

void mainPage::mouseDown(ci::app::MouseEvent event) {
	if (selectedIndex_ > -1) {
		selectedIndex_ = -1;
	}
	else {
		if (debugMode_) {
			Vec2f mousePos = event.getPos();
			float maxDis = 10000000;
			int tmpIndex = -1;
			for (int i = 0; i < rectPoints_.size(); i++) {
				float dis = (mousePos - rectPoints_[i] * Vec2f(w_, h_)).length();
				if (dis < maxDis) {
					maxDis = dis;
					tmpIndex = i;
				}
			}
			if (tmpIndex > -1 && maxDis < debugCircleRadius_) {
				selectedIndex_ = tmpIndex;
			}
		}
	}
	currentMousePos_ = event.getPos();
}

void mainPage::mouseMove(ci::app::MouseEvent event) {
	currentMousePos_ = event.getPos();
}

void mainPage::mouseUp(ci::app::MouseEvent event) {

}

void mainPage::touchesBegan(ci::app::MouseEvent event) {

}

void mainPage::touchesEnded(ci::app::MouseEvent event) {

}