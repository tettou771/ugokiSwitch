#include "ofMain.h"
#include "ofApp.h"

// hide console
#ifdef WIN32
#ifndef _DEBUG
#pragma comment(linker, "/subsystem:\"windows\" /entry:\"mainCRTStartup\"")
#endif
#endif

//========================================================================
int main( ){
	ofSetupOpenGL(640, 480, OF_WINDOW);
    
	ofRunApp(new ofApp());
}
