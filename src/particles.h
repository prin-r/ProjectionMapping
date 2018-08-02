#pragma once

#include "cinder/Vector.h"
#include <cinder/app/AppBasic.h>
#include "cinder/Utilities.h"
#include "cinder/ImageIo.h"
#include "cinder/Font.h"
#include "Resources.h"
#include "BitMultimedia.h"

using namespace ci::app;
using namespace ci;
using namespace std;

class particles
{
public:
	
	std::list<Bit::Multimedia> effects_;

	void setup();
	void draw();
	void update();
	void addEffect(Bit::Multimedia effect);
	void addEffect(Bit::Multimedia effect, Bit::DisplayArea da);

private:

};