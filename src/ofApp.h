#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "UIButton/UIButton.h"
#include "ImageSensing/SensingAreaManager.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();
	void draw_selection();
	void draw_sensing();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mousePressed(int x, int y, int button);
	void mouseDragged(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofVideoGrabber videoGrabber;
	bool videoGrabberFrameIsNew;
	
	SensingAreaManager sensingAreaManager;

	// 起動時のカメラ選択
	bool cameraSelection;
	int cameraIndex;
	vector<string> cameraName;
	ofTrueTypeFont font;
	string cameraSearchName;

	// カメラ選択画面の開始処理
	void onBeginCameraSelection();

	// カメラ選択の後にセンシングモードになる時の開始処理
	void onBeginSensing();

	// カメラ選択画面に戻る処理
	void onBackToCameraSelection();

	// camera draw screen
	ofFbo viewScreen;

	// GUI
	ofxPanel gui;
	ofxIntSlider frameRate;
    ofxFloatSlider fisheye;
	ofxToggle videoShow; // videoを描画するかどうか（節約時に消せるように）
	bool isGuiShowing;

	// ボタン形 UI
	UIButton saveButton, loadButton, clearButton, cameraSelectionButton, videoSettingsButton;
	vector<UIButton *> buttons;
	void repositionButtons();

	// カメラ画像のサイズ、fps
	int videoWidth, videoHeight;
	int videoFps;


	// OSC
	ofxOscSender oscSender;
	string oscAddress;
	int oscPort;

	// 設定値の保存
	void loadConfig();
	void saveConfig();
};


