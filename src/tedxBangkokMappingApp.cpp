#include "cinder/app/AppNative.h"
#include "cinder/app/AppBasic.h"
#include "cinder/gl/gl.h"
#include "cinder/params/Params.h"
#include "cinder/Surface.h"
#include "cinder/gl/gl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Font.h"
#include "cinder/app/AppImplMsw.h"
#include "boost/thread/mutex.hpp"

#if defined(WIN32) && !defined(_DEBUG)
#include "AppVerify.h"
#endif

#include "Config.h"
#include "Debug.h"
#include "BitException.h"
#include "BitExceptionHandler.h"
#include "BitShortcutKey.h"

#include "mainPage.h"

#include "AClass.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class tedxBangkokMappingApp : public AppNative {
public:
	void prepareSettings(Settings *settings);
	void setup();
	void shutdown();
	void mouseDown(MouseEvent event);
	void keyDown(KeyEvent event);
	void update();
	void draw();
	void emitClose();
	
private:
#if defined(WIN32) && !defined(_DEBUG)
	AppVerify  appVerify_;
#endif
	void toggleFullscreen();
	void toggleDisplayParameters();
	void checkExceptionFromThread();

	Bit::Config config_;
	bool      borderless_;
	
	// debugging values
	bool      debug_;
	bool	  reset_;
	bool      paramsOn_;
	Bit::ShortcutKey shortcutKey_;
	bool setupCompleted_;

	mainPage mp_;

	AClass aClass_;
};

void tedxBangkokMappingApp::prepareSettings( Settings *settings )
{
#if defined(WIN32) && !defined(_DEBUG)
	//if( !appVerify_.isValid() ) {
	//	showCursor();
	//	MessageBox( NULL, L"Application has expired.", L"ERROR", MB_OK );
	//	exit( 1 );
	//}
#endif

	// initialize debugging values
	debug_ = false;
	paramsOn_ = false;
	setupCompleted_ = false;

	try {
		// register shutdown function to exception handler
		Bit::ExceptionHandler::registerShutdownFunction(std::bind(&tedxBangkokMappingApp::emitClose, this));
		// read settings for window setup
		config_.readConfig();
		Bit::Config::DisplayConfig appConfig = config_.getDisplayConfig();

		// setup window 
		settings->setWindowSize(appConfig.windowSize.x, appConfig.windowSize.y);
		settings->setWindowPos(appConfig.windowPos.x, appConfig.windowPos.y);
		settings->setAlwaysOnTop(appConfig.alwaysOnTop);
		settings->setBorderless(appConfig.borderless);
		borderless_ = appConfig.borderless;
		// setup cursor
		if (appConfig.hideCursor)
			hideCursor();
		else
			showCursor();
	}
	catch (std::exception& e) {
		Bit::ExceptionHandler::handleException(&e);
	}
}

void tedxBangkokMappingApp::setup()
{
	// setup shortcut keys
	shortcutKey_.setupDisplayDialog("Shortcut list", Vec2i(400, 200), ColorA(0.3f, 0.3f, 0.3f, 0.4f));
	shortcutKey_.addShortcut(KeyEvent::KEY_ESCAPE, std::bind(&tedxBangkokMappingApp::emitClose, this), "close application");
	shortcutKey_.addShortcut('d', &debug_, "toggle display debug mode");
	shortcutKey_.addShortcut('r', &reset_, "resetRectProjection");
	shortcutKey_.addShortcut('f', std::bind(&tedxBangkokMappingApp::toggleFullscreen, this), "toggle fullscreen mode");
	shortcutKey_.addShortcut('p', std::bind(&tedxBangkokMappingApp::toggleDisplayParameters, this), "toggle display parameters dialog");
	shortcutKey_.addShortcut(KeyEvent::KEY_F1, std::bind(&Bit::ShortcutKey::toggleDisplay, &shortcutKey_), "toggle display shortcut keys list");
	
	try {
		config_.setup();
		
		// setup everything here
		config_.readConfigurableConfig(aClass_, "aClassConfig");	// this will eventually calls AClass::readConfig() with the Bit::JsonTree* node named "aClassConfig" as argument
		config_.readConfigurableParams(aClass_, "aClassParams");	// this will eventually calls AClass::readParams() with the Bit::JsonTree* node named "aClassParams" as argument
		
		config_.readConfigurableConfig(mp_, "mainPage");
		config_.readConfigurableParams(mp_, "mainPage");

		reset_ = false;
		mp_.setup();
		// mark setup complete at the end of setup.
		setupCompleted_ = true;
	}
	catch (std::exception& e) {
		Bit::ExceptionHandler::handleException(&e);
	}
}

void tedxBangkokMappingApp::emitClose()

{
	// if setup is donw (we have window), emit the same event like clicking windows' close button (X button) on upper right corner
	// TODO: we need to handle multiple windows later
	if (setupCompleted_){
		WindowImplMsw* impl = reinterpret_cast<WindowImplMsw*>(::GetWindowLongPtr((HWND)ci::app::getWindow()->getNative(), GWLP_USERDATA));
		impl->getWindow()->emitClose();
		impl->privateClose();
		delete impl;
		// quit will call shutdown() for clean up and close the app
		quit();
	}
	else{	// otherwise, simply exit
		exit(Bit::Exception::getExitCode());
	}
}

void tedxBangkokMappingApp::shutdown()
{
	//int exitCode = Bit::Exception::getExitCode();
	//exit( exitCode );	// we can not exit() here as memory leaks will occur
}

void tedxBangkokMappingApp::toggleFullscreen()
{
	setFullScreen(!isFullScreen());
}

void tedxBangkokMappingApp::toggleDisplayParameters()
{
	paramsOn_ = !paramsOn_;
	if (paramsOn_) {
		showCursor();

		config_.showParams();
	}
	else {
		hideCursor();

		config_.hideParams();
	}
}

void tedxBangkokMappingApp::keyDown( KeyEvent event )
{
	shortcutKey_.keyDown(event);
}

void tedxBangkokMappingApp::mouseDown( MouseEvent event )
{
}

void tedxBangkokMappingApp::update()
{
	try {
		// check if there is any exception from thread, for more info see Bit::ExceptionHandler::checkExceptionFromThread
		Bit::ExceptionHandler::checkExceptionFromThread();
		
		// added update part here
		if (debug_) {
			mp_.debugMode_ = true;
		}
		else {
			mp_.debugMode_ = false;
		}

		if (paramsOn_ || debug_) {
			showCursor();
		}
		else {
			hideCursor();
		}

		if (reset_) {
			mp_.resetRectProjection2Default();
			reset_ = false;
		}

		mp_.update();
		
	}
	catch (std::exception& e) {
		Bit::ExceptionHandler::handleException(&e);
	}
}

void tedxBangkokMappingApp::draw()
{
	if (!setupCompleted_)
		return;

	// clear out the window with black
	gl::clear(Color(0, 0, 0));
	
	// draw everything here
	mp_.draw();
	
	
	// all debugging things 

	if (debug_) {
		// draw fps
		gl::drawString(toString(getAverageFps()), Vec2f(10, 10), ColorA(0.3f, 0.3f, 0.7f, 1.0f), Font("Arial", 30));
	}
	
	if (paramsOn_) {
		config_.drawParams();
	}
	
	// draw all shortcut keys to dialog
	shortcutKey_.draw();
}

CINDER_APP_NATIVE(tedxBangkokMappingApp, RendererGl)
