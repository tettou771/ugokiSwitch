#include "SensingAreaManager.h"



SensingAreaManager::SensingAreaManager() {
}


SensingAreaManager::~SensingAreaManager() {
}

void SensingAreaManager::setup() {
	sound.load("sounds/bang.wav");

	for (int i = 0; i < sceneNum; ++i) {
		scenes.push_back(new SensingAreaScene(i));
	}
	setScene(0);
}

void SensingAreaManager::addParametorToGui(ofxPanel & gui) {
	SensingArea::bangThreshold = new ofxFloatSlider();

	SensingArea::bangThreshold->setup("Bang threshold", 0.05, 0, 1);
	gui.add(SensingArea::bangThreshold);
	brightnessThreshold.setup("Brightness threshold", 50, 0, 255);
	gui.add(&brightnessThreshold);
	verticalFlip.setup("Vertical flip", false);
	gui.add(&verticalFlip);
	horizontalFlip.setup("Horizontal flop", true);
	gui.add(&horizontalFlip);
	soundEnabled.setup("Sound enable", false);
	gui.add(&soundEnabled);
}

void SensingAreaManager::start(ofVideoGrabber * _videoGrabber) {
	videoGrabber = _videoGrabber;
	imageProcessor.setup(videoGrabber->getWidth(), videoGrabber->getHeight());

	setMouseKeyEnabled(true);
}

void SensingAreaManager::stop() {
	makingNewSensingArea = false;

	setMouseKeyEnabled(false);
}

void SensingAreaManager::update() {
	if (videoGrabber != nullptr && videoGrabber->isFrameNew()) {
		imageProcessor.setBrightnessThreshold(brightnessThreshold);
		imageProcessor.setNewTexture(videoGrabber->getTexture(), horizontalFlip, verticalFlip);

		bool needSound = false;
		currentScene->updateSensingAreas(imageProcessor.getDiffPixels(), needSound);

		// play sound if needed
		if (soundEnabled && needSound) {
			playSound();
		}
	}

	currentScene->deleteAreaIfNeeded();
}

void SensingAreaManager::drawDiffImage() {
	imageProcessor.drawDiffVisualize();
}

void SensingAreaManager::drawSensingArea() {
	ofPushStyle();
	// sensing area
	ofDisableAntiAliasing();
	currentScene->drawSensingAreas();

	// dragging rectangle
	if (makingNewSensingArea) {
		ofVec2f m = DrawScaleManager::getScaledMouse();
		ofVec2f clicked(
			DrawScaleManager::getScaledX(clickedX),
			DrawScaleManager::getScaledY(clickedY)
		);
		int width = abs(m.x - clicked.x);
		int height = abs(m.y - clicked.y);

		// set bold if rectangle is over makable size
		if (width > SensingArea::minimumWidth && height > SensingArea::minimumHeight) ofSetLineWidth(3);
		else ofSetLineWidth(1);
		ofSetColor(255, 255, 0);
		ofDrawRectangle(clicked.x, clicked.y, m.x - clicked.x, m.y - clicked.y);
	}
    ofEnableAntiAliasing();
	ofPopStyle();
}

void SensingAreaManager::drawSceneIndex() {
	// scene index
	ofDrawBitmapStringHighlight("Scene " + ofToString(currentScene->index), ofGetWidth() - 100, 20, ofColor(0, 150), ofColor(200));
}

ofxOscBundle SensingAreaManager::getOscBundle() {
	return currentScene->getOscBundle();
}

void SensingAreaManager::mouseMoved(ofMouseEventArgs & mouse) {

}

void SensingAreaManager::mousePressed(ofMouseEventArgs & mouse) {
	if (mouse.button == 0) {
		SensingArea::selected = nullptr;
		ofVec2f scaled = DrawScaleManager::getScaledMouse();

		if (SensingArea::selected == nullptr) {
			// エリアのタイトルバーをクリックしていたらそれを選択。移動可能になる
			for (auto &a : currentScene->sensingAreas) {
				if (a->selectIfTitlebarHitted(scaled.x, scaled.y)) {
					break;
				}
			}
		}

		// もしどのウインドウも選択してなく、かつクリックしたところがビデオ内なら、生成を開始
		if (SensingArea::selected == nullptr
			&& 0 < DrawScaleManager::getScaledMouseX()
			&& DrawScaleManager::getScaledMouseX() < videoGrabber->getWidth()
			&& 0 < DrawScaleManager::getScaledMouseY()
			&& DrawScaleManager::getScaledMouseY() < videoGrabber->getHeight()) {

			makingNewSensingArea = true;
			clickedX = mouse.x;
			clickedY = mouse.y;
			return;
		}
	}
	else if (mouse.button == 2) {
	}
}

void SensingAreaManager::mouseDragged(ofMouseEventArgs & mouse) {
}

void SensingAreaManager::mouseReleased(ofMouseEventArgs & mouse) {

	// 選択中のものがあれば Release 処理
	if (SensingArea::selected != nullptr) {
		SensingArea::selected->mouseReleasedIfSelected();
	}

	// もし何も掴まずドラッグ中で、距離が微小でなければ、areaを生成する
	else if (makingNewSensingArea) {
		// drawScale を考慮して変換
		// ついでに video からはみ出さないようにエリアを調整する
		ofRectangle rect;
		rect.x = MAX(0, MIN(DrawScaleManager::getScaledMouseX(), videoGrabber->getWidth()));
		rect.y = MAX(0, MIN(DrawScaleManager::getScaledMouseY(), videoGrabber->getHeight()));
		rect.width = DrawScaleManager::getScaledX(clickedX) - rect.x;
		rect.height = DrawScaleManager::getScaledY(clickedY) - rect.y;

		// width, height が負の値にならないように、左上を始点とする長方形にする
		if (rect.width < 0) {
			rect.x += rect.width;
			rect.width = -rect.width;
		}
		if (rect.height < 0) {
			rect.y += rect.height;
			rect.height = -rect.height;
		}

		// 生成できる最小サイズ以上なら実際に生成する
		if (rect.width >= SensingArea::minimumWidth && rect.height >= SensingArea::minimumHeight) {
			makeSensingArea(rect);
		}

		makingNewSensingArea = false;
	}
}

void SensingAreaManager::keyPressed(ofKeyEventArgs & key) {
	switch (key.key) {
	case OF_KEY_RIGHT:
		setScene((currentScene->index + 1) % scenes.size());
		break;
	case OF_KEY_LEFT:
		setScene((currentScene->index - 1) % scenes.size());
		break;
	case OF_KEY_ESC:setScene(0); break;
	case OF_KEY_F1:setScene(1); break;
	case OF_KEY_F2:setScene(2); break;
	case OF_KEY_F3:setScene(3); break;
	case OF_KEY_F4:setScene(4); break;
	case OF_KEY_F5:setScene(5); break;
	case OF_KEY_F6:setScene(6); break;
	case OF_KEY_F7:setScene(7); break;
	case OF_KEY_F8:setScene(8); break;
	case OF_KEY_F9:setScene(9); break;
	case OF_KEY_F10:setScene(10); break;
	case OF_KEY_F11:setScene(11); break;
	case OF_KEY_F12:setScene(12); break;
	}
}

void SensingAreaManager::setMouseKeyEnabled(bool _enabled) {
	if (mouseEnabled == _enabled) return;

	mouseEnabled = _enabled;
	if (mouseEnabled) {
		ofAddListener(ofEvents().mouseMoved, this, &SensingAreaManager::mouseMoved);
		ofAddListener(ofEvents().mousePressed, this, &SensingAreaManager::mousePressed);
		ofAddListener(ofEvents().mouseDragged, this, &SensingAreaManager::mouseDragged);
		ofAddListener(ofEvents().mouseReleased, this, &SensingAreaManager::mouseReleased);
		ofAddListener(ofEvents().keyPressed, this, &SensingAreaManager::keyPressed);
	}
	else {
		ofRemoveListener(ofEvents().mouseMoved, this, &SensingAreaManager::mouseMoved);
		ofRemoveListener(ofEvents().mousePressed, this, &SensingAreaManager::mousePressed);
		ofRemoveListener(ofEvents().mouseDragged, this, &SensingAreaManager::mouseDragged);
		ofRemoveListener(ofEvents().mouseReleased, this, &SensingAreaManager::mouseReleased);
		ofRemoveListener(ofEvents().keyPressed, this, &SensingAreaManager::keyPressed);
	}
}

void SensingAreaManager::clear() {
	scenes.clear();
}

void SensingAreaManager::clearThisScene() {
	currentScene->clear();
}

void SensingAreaManager::clearScene(int sceneIndex) {
	if (0 <= sceneIndex && sceneIndex < scenes.size()) {
		scenes[sceneIndex]->clear();
	}
}

void SensingAreaManager::setScene(int sceneIndex) {
	ofLog() << "setScene(" << ofToString(sceneIndex) << "\"\n";

	if (sceneIndex < 0 || scenes.size() <= sceneIndex) {
		ofLogError() << "this scene is invalid\n";
		return;
	}

	SensingArea::selected = nullptr;
	makingNewSensingArea = false;

	currentScene = scenes[sceneIndex];
	currentScene->initialize();
    
    DisplayMessage::setMessage("SCENE " + ofToString(sceneIndex));
}

void SensingAreaManager::makeSensingArea(ofRectangle rect) {
	// decide new index small.
	int index = 0;
	while (true) {
		bool loop = false;
		for (auto a : currentScene->sensingAreas) {
			if (a->index == index) {
				loop = true;
			}
		}
		if (loop) index++;
		else break;
	}

	// make a sensing area and select
	currentScene->sensingAreas.push_back(new SensingArea(index, rect));
	SensingArea::selected = currentScene->sensingAreas.back();
}

void SensingAreaManager::playSound() {
	if (sound.isLoaded() && sound.isPlaying() == false) {
		sound.play();
	}
}

void SensingAreaManager::loadConfig() {

	for (auto &s : scenes) {
		s->clear();
	}

	verticalFlip = false;
	horizontalFlip = true;
	brightnessThreshold = 30;

	ofXml xml;
	if (xml) {
		xml.load("sensingArea.xml");
		auto configTag = xml.findFirst("config");
		if (configTag) {
			auto flipTag = configTag.findFirst("flip");
			if (flipTag) {
				horizontalFlip = flipTag.getAttribute("horizontal").getBoolValue();
				verticalFlip = flipTag.getAttribute("vertical").getBoolValue();
			}

			auto thresholdTag = configTag.findFirst("threshold");
			if (thresholdTag) {
				if (SensingArea::bangThreshold == nullptr) SensingArea::bangThreshold = new ofxFloatSlider();
				*SensingArea::bangThreshold = thresholdTag.getAttribute("bang").getFloatValue();
				brightnessThreshold = thresholdTag.getAttribute("brightness").getIntValue();
			}

			auto scenesTag = configTag.findFirst("scenes");
			if (scenesTag) {
				for (auto &sceneTag : scenesTag.getChildren("scene")) {
					int sceneIndex = sceneTag.getAttribute("index").getIntValue();

					auto sensingAreaTags = sceneTag.getChildren("sensingArea");
					for (auto &sensingAreaTag : sensingAreaTags) {
						int number = sensingAreaTag.getAttribute("number").getIntValue();
						int type = sensingAreaTag.getAttribute("type").getIntValue();
						auto rectTag = sensingAreaTag.findFirst("rect");
						float x = rectTag.getAttribute("x").getFloatValue();
						float y = rectTag.getAttribute("y").getFloatValue();
						float width = rectTag.getAttribute("width").getFloatValue();
						float height = rectTag.getAttribute("height").getFloatValue();

						ofRectangle rect(x, y, width, height);
						scenes[sceneIndex]->sensingAreas.push_back(new SensingArea(number, rect, SensingAreaType(type)));
					}
				}
			}
		}
	}
}

void SensingAreaManager::saveConfig() {
	ofXml xml;
	auto configTag = xml.appendChild("config");
	{
		auto flipTag = configTag.appendChild("flip");
		flipTag.setAttribute("horizontal", ofToString(int(horizontalFlip)));
		flipTag.setAttribute("vertical", ofToString(int(verticalFlip)));

		auto thresholdTag = configTag.appendChild("threshold");
		{
			thresholdTag.setAttribute("bang", ofToString(SensingArea::bangThreshold->getParameter()));
			thresholdTag.setAttribute("brightness", ofToString(brightnessThreshold.getParameter()));
		}

		auto scenesTag = configTag.appendChild("scenes");
		{
			for (int i = 0; i < scenes.size(); ++i) {
				auto sceneTag = scenesTag.appendChild("scene");
				sceneTag.setAttribute("index", ofToString(i));
				{
					for (auto &a : scenes[i]->sensingAreas) {
						auto sensingAreaTag = sceneTag.appendChild("sensingArea");
						{
							sensingAreaTag.setAttribute("number", ofToString(a->index));
							sensingAreaTag.setAttribute("type", ofToString(int(a->type)));
							auto rectTag = sensingAreaTag.appendChild("rect");
							{
								rectTag.setAttribute("x", ofToString(a->rect.x));
								rectTag.setAttribute("y", ofToString(a->rect.y));
								rectTag.setAttribute("width", ofToString(a->rect.width));
								rectTag.setAttribute("height", ofToString(a->rect.height));
							}
						}
					}
				}
			}
		}
	}

	xml.save("sensingArea.xml");
}

void SensingAreaManager::SensingAreaScene::deleteAreaIfNeeded() {
	for (int i = 0; i < sensingAreas.size(); ++i) {
		if (sensingAreas[i]->deleteFlag) {
			SensingArea::selected = nullptr;

			// select other sensing area if destroy with delete key.
			if (sensingAreas[i]->deleteKeyPressed) {
				// select next one if it's exists.
				if (i + 1 < sensingAreas.size()) {
					SensingArea::selected = sensingAreas[i + 1];
				}
				// select past one if next one isn't exists
				else if (0 <= i - 1) {
					SensingArea::selected = sensingAreas[i - 1];
				}
			}

			delete sensingAreas[i];
			sensingAreas.erase(sensingAreas.begin() + i);
		}
	}
}

void SensingAreaManager::SensingAreaScene::updateSensingAreas(ofPixels & pixels, bool &needSound) {
	for (auto &s : sensingAreas) {
		s->update(pixels);
		needSound = needSound | s->needSendBang;
	}
}

void SensingAreaManager::SensingAreaScene::drawSensingAreas() {
	ofDisableAntiAliasing();
	ofNoFill();
	for (auto &a : sensingAreas) {
		a->draw();
	}
}

void SensingAreaManager::SensingAreaScene::initialize() {
	for (auto &s : sensingAreas) {
		s->initialize();
	}
}

void SensingAreaManager::SensingAreaScene::clear() {
	for (auto &s : sensingAreas) {
		delete s;
	}
	sensingAreas.clear();
}

ofxOscBundle SensingAreaManager::SensingAreaScene::getOscBundle() {
	ofxOscBundle bundle;
	bool needSound = false;
	for (auto &a : sensingAreas) {
		a->appendOscBundle(bundle);
		if (a->needSendBang) needSound = true;
	}
	return bundle;
}
