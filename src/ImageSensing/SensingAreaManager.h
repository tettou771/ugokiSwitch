#pragma once
#include "SensingArea.h"
#include "ImageProcessor.h"
#include "../DisplayMessage/DisplayMessage.h"

class SensingAreaManager {
public:
	SensingAreaManager();
	~SensingAreaManager();

	void setup();
	void addParametorToGui(ofxPanel &gui);
	void start(ofVideoGrabber *);
	void stop();
	void update();
	void drawDiffImage();
	void drawSensingArea();
	void drawSceneIndex();
	ofxOscBundle getOscBundle();

	void mouseMoved(ofMouseEventArgs &mouse);
	void mousePressed(ofMouseEventArgs &mouse);
	void mouseDragged(ofMouseEventArgs &mouse);
	void mouseReleased(ofMouseEventArgs &mouse);
	void keyPressed(ofKeyEventArgs &key);
	void setMouseKeyEnabled(bool _enabled);

	void clear();
	void clearThisScene();
	void clearScene(int sceneIndex);

	// gui
	ofxIntSlider brightnessThreshold; // 明るさの変化のしきい値
	ofxToggle soundEnabled; // 効果音を鳴らすかどうか
	ofxToggle verticalFlip; // 縦反転
	ofxToggle horizontalFlip; // 横反転


	void loadConfig();
	void saveConfig();

private:
	ofVideoGrabber * videoGrabber = nullptr;

	ImageProcessor imageProcessor;

	// sensing area
	struct SensingAreaScene {
		SensingAreaScene(int _index) :index(_index) {}
		~SensingAreaScene() {
			clear();
		}
		vector<SensingArea *> sensingAreas;
		int index;

		void deleteAreaIfNeeded();
		void updateSensingAreas(ofPixels &pixels, bool &needSound);
		void drawSensingAreas();
		void initialize();
		void clear();
		ofxOscBundle getOscBundle();
	};
	vector<SensingAreaScene *> scenes;
	static const int sceneNum = 13;
	SensingAreaScene *currentScene;
	void setScene(int sceneIndex);

	// click position information
	int clickedX, clickedY;
	bool makingNewSensingArea; // ドラッグでエリアを新規生成中かどうか
	void makeSensingArea(ofRectangle rect);

	bool mouseEnabled = false;

	// センシングした時に再生するサウンド
	ofSoundPlayer sound;
	void playSound();
};

