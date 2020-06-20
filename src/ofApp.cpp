#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofHideCursor();
	ofSetCircleResolution(50);
	ofSetVerticalSync(false);
	ofSetBackgroundAuto(false);
	ofSetFrameRate(10);
	ofLogToConsole();
	ofSetEscapeQuitsApp(false);
#ifdef TARGET_OSX
	ofSetDataPathRoot("../data");
#endif

	// フォントをロード
	font.load("fonts/RictyDiminished-Regular.ttf", 12);

	sensingAreaManager.setup();

	// 各種設定をロード
	loadConfig();

	onBeginCameraSelection();

	// カメラが一台しか無いときは、それを自動選択する
	if (cameraName.size() == 1) {
		cameraIndex = 0;
		onBeginSensing();
	}

	// カメラの名前の指定がある場合は、それを探す。
	else if (cameraSearchName != "") {
		for (int i = 0; i < cameraName.size(); ++i) {
			auto &name = cameraName[i];
			if (ofIsStringInString(name, cameraSearchName)) {
				cameraIndex = i;
				onBeginSensing();
			}
		}
	}

	//gui
	gui.setup();
	gui.setPosition(0, 5);
	gui.add(frameRate.setup("Frame rate", 30, 1, 60));
	gui.add(videoShow.setup("Video show", true));
    gui.add(fisheye.setup("Fisheye", 0, 0, 1.0));

	sensingAreaManager.addParametorToGui(gui);
	isGuiShowing = true;

	SensingArea::selected = nullptr;

	oscSender.setup(oscAddress, oscPort);

	// SAVE などのボタン
	ofRectangle rect(10, videoHeight - 100, 50, 13);
	saveButton.setup("SAVE", rect);
	buttons.push_back(&saveButton);
	loadButton.setup("LOAD", rect);
	buttons.push_back(&loadButton);
	clearButton.setup("CLEAR", rect);
	buttons.push_back(&clearButton);
	cameraSelectionButton.setup("CAMERA SELECTION", rect);
	buttons.push_back(&cameraSelectionButton);
	videoSettingsButton.setup("VIDEO SETTINGS", rect);
	buttons.push_back(&videoSettingsButton);
	repositionButtons();

	DrawScaleManager::drawScale = ofVec2f(1.0, 1.0);

	loadConfig();
}

void ofApp::onBeginCameraSelection() {
	// selectionフラグを立てる
	cameraSelection = true;

	// カメラを列挙
    if (cameraName.empty()) {
        ofSetLogLevel(OF_LOG_VERBOSE);
        ofLog() << "///// camera list /////";
        vector<ofVideoDevice> list = videoGrabber.listDevices();
        ofLog() << "///// camera list end /////";
        ofSetLogLevel(OF_LOG_ERROR);
        cameraName.clear();
        for (int i = 0; i < list.size(); ++i) {
            cameraName.push_back(list[i].deviceName);
        }
    }
}

void ofApp::onBeginSensing() {
	// この状態で保存したとき、同じカメラで起動するようにデバイス名を代入しておく
	cameraSearchName = cameraName[cameraIndex];

	// windowサイズを変更
	ofSetWindowShape(videoWidth, videoHeight);

	// vidGrabberを初期化
	videoGrabber.setDeviceID(cameraIndex);
	//vidGrabber.setVerbose(true);
	videoGrabber.setDesiredFrameRate(videoFps);
	ofSetFrameRate(videoFps);
	videoGrabber.initGrabber(videoWidth, videoHeight);

	// 描画するfboを準備
	viewScreen.allocate(videoWidth, videoHeight, GL_RGB);

	repositionButtons();

	sensingAreaManager.start(&videoGrabber);

	// カメラ選択モードを終了する
	cameraSelection = false;
}

void ofApp::onBackToCameraSelection() {
	videoGrabber.close();
	cameraIndex = -1;
	cameraSearchName = "";
	SensingArea::selected = nullptr;

	sensingAreaManager.stop();

	onBeginCameraSelection();
}

//--------------------------------------------------------------
void ofApp::update() {
	// cameraSelectionなら処理を飛ばす
	if (cameraSelection) return;

	if (videoGrabber.isInitialized()) {
		videoGrabber.update();
	}

	sensingAreaManager.update();

	// if camera has new frame
	if (videoGrabber.isFrameNew()) {
	
		// send osc
		oscSender.sendBundle(sensingAreaManager.getOscBundle());

		// draw diff
		if (videoShow) {
			viewScreen.begin();
			sensingAreaManager.drawDiffImage();
            viewScreen.end();
		}
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
    if (cameraSelection) draw_selection();
	else draw_sensing();
}

void ofApp::draw_selection() {
	// カメラ選択画面なら、有効なカメラを選択するための文字列だけを表示
	ofBackground(0, 0, 50);

	float lineHeight = font.getSize()*1.5; // 行の幅
	ofSetColor(255);
	ofTranslate(10, lineHeight);
	font.drawString("Please select camera id (press 0-" + ofToString(cameraName.size() - 1) + " key).", 0, 0);
	for (int i = 0; i < cameraName.size(); ++i) {
		ofTranslate(0, lineHeight);
		font.drawString(ofToString(i) + ": " + cameraName[i], 0, 0);
	}
}

void ofApp::draw_sensing() {
	ofBackground(0);
    
	// DrawScale を使ってスケーリングして描画
	DrawScaleManager::push();

	// もしvideoShowなら描画する。これはguiで変更できて、リソースの節約時には描画しない
	if (videoShow) {
		ofPushStyle();
		ofSetColor(255);
		viewScreen.draw(0, 0);
		ofPopStyle();

		sensingAreaManager.drawSensingArea();
	}

	DrawScaleManager::pop();

	// gui
	if (isGuiShowing) {
		// background
		ofSetColor(0, 150);
		ofFill();
		ofDrawRectangle(0, 0, gui.getWidth(), ofGetHeight());

		// buttons
		ofSetLineWidth(2);
		ofSetColor(200);
		ofNoFill();
		for (auto &b : buttons) {
			b->draw();
		}

		// network infomations
		string infoStr = "Address: " + oscAddress
			+ '\n' + "Port: " + ofToString(oscPort)
			+ '\n' + "FPS: " + ofToString(ofGetFrameRate());
		ofDrawBitmapString(infoStr, 10, ofGetHeight() - 34);
		//ofDrawBitmapString(, videoWidth + 10, ofGetHeight() - 22);
		//ofDrawBitmapString(, videoWidth + 10, ofGetHeight() - 10);

		gui.draw();
	}

	// sceneIndex
	sensingAreaManager.drawSceneIndex();

	// cursor (cross lines)
	ofSetColor(255);
	ofSetLineWidth(1);
	ofPushMatrix();
	ofTranslate(ofGetMouseX() + 0.5, ofGetMouseY() + 0.5);
	float cursorSize = 6.5;
	ofDrawLine(-cursorSize, 0, cursorSize, 0);
	ofDrawLine(0, -cursorSize, 0, cursorSize);
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {
	// カメラ選択画面の操作
	if (cameraSelection) {
		// 数字キーなら
		if (48 <= key && key <= 57) {
			int n = key - 48;

			// カメラ選択画面なら、cameraIdを決定
			if (cameraSelection && n < cameraName.size()) {
				cameraIndex = n;
				onBeginSensing();
			}
		}
	}

	// センシング中の操作
	else {
		if (SensingArea::selected != nullptr) {
			SensingArea::selected->keyPressedIfSelected(key);
		}

		// カメラ選択画面に戻る
		if (key == 'b') {
			onBackToCameraSelection();
		}
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {
	if (!isGuiShowing && x < 40) {
		isGuiShowing = true;
		sensingAreaManager.setMouseKeyEnabled(false);
	}
	else if (isGuiShowing && x > gui.getWidth() + 30) {
		isGuiShowing = false;
		sensingAreaManager.setMouseKeyEnabled(true);
	}
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {
	if (button == 0) {
		// UI check if gui showing
		if (isGuiShowing) {
			if (saveButton.inside(x, y)) saveConfig();
			if (loadButton.inside(x, y)) loadConfig();
			if (clearButton.inside(x, y)) sensingAreaManager.clear();
			if (cameraSelectionButton.inside(x, y)) onBackToCameraSelection();
			if (videoSettingsButton.inside(x, y)) videoGrabber.videoSettings();
		}
	}
	else if (button == 2) {
	}
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {
	if (button == 0) {
		// 選択中のものがあれば drag 処理
		if (SensingArea::selected != nullptr) {
			SensingArea::selected->mouseDraggedIfSelected();
		}
	}
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {
	DrawScaleManager::drawScale.set(
		(float)ofGetWidth() / videoWidth,
		(float)ofGetHeight() / videoHeight
	);
	repositionButtons();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}

void ofApp::repositionButtons() {
	// guiを移動
	gui.setPosition(0, 5);
	ofRectangle rect(10, ofGetHeight() - 120, 50, 16);
	saveButton.setRect(rect);
	rect.x += 60;
	loadButton.setRect(rect);
	rect.x += 60;
	clearButton.setRect(rect);
	rect.set(10, rect.y + 24, 140, 16);
	cameraSelectionButton.setRect(rect);
	rect.y += 24;
	videoSettingsButton.setRect(rect);
}

void ofApp::loadConfig() {
	// set defaults
	frameRate = 30;
	oscAddress = "localhost";
	oscPort = 6000;


	ofXml xml;
	if (xml) {
		xml.load("config.xml");
		auto configTag = xml.findFirst("config");
		if (configTag) {
			auto cameraTag = configTag.findFirst("camera");
			if (cameraTag) {
				auto searchNameTag = cameraTag.findFirst("searchName");
				if (searchNameTag) {
					cameraSearchName = searchNameTag.getAttribute("string").getValue();
				}

				auto settingsTag = cameraTag.findFirst("settings");
				if (settingsTag) {
					videoWidth = settingsTag.getAttribute("width").getIntValue();
					videoHeight = settingsTag.getAttribute("height").getIntValue();
					videoFps = settingsTag.getAttribute("fps").getFloatValue();
				}
			}

			auto oscTag = configTag.findFirst("osc");
			if (oscTag) {
				oscAddress = oscTag.getAttribute("address").getValue();
				oscPort = oscTag.getAttribute("port").getIntValue();
			}

		}
	}

	sensingAreaManager.loadConfig();
}

void ofApp::saveConfig() {
	ofXml xml;
	auto configTag = xml.appendChild("config");
	{
		if (videoGrabber.isInitialized()) {
			auto cameraTag = configTag.appendChild("camera");
			{
				cameraTag.appendChild("searchName").setAttribute("string", cameraSearchName);
				auto settingsTag = cameraTag.appendChild("settings");
				settingsTag.setAttribute("fps", ofToString(videoFps));
				settingsTag.setAttribute("width", ofToString(videoGrabber.getWidth()));
				settingsTag.setAttribute("height", ofToString(videoGrabber.getHeight()));
			}
		}

		auto oscTag = configTag.appendChild("osc");
		{
			oscTag.setAttribute("address", ofToString(oscAddress));
			oscTag.setAttribute("port", ofToString(oscPort));
		}
	}

	xml.save("config.xml");

	sensingAreaManager.saveConfig();
}
