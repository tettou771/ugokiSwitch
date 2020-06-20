#include "SensingArea.h"

SensingArea *SensingArea::selected;
ofxFloatSlider *SensingArea::bangThreshold;

SensingArea::SensingArea() {
	initialize();
}


SensingArea::~SensingArea() {
}

void SensingArea::update(ofPixels &pixels) {
	// 変化しているピクセルの数
	atomic<long> count;
	count = 0;

	// エリア内のピクセルを総スキャンする

	/*
	// run in multi thread
	vector<thread *> threads;
	int max = 4;
	for (int index = 0; index < max; ++index) {
		threads.push_back(new thread([&] {
			int ixStart = rect.x + (rect.width / max) * index;
			int ixEnd = rect.x + (rect.width / max) * (index + 1);
			scanPixels(ixStart, ixEnd, pixels, count);
		}));
	}

	for (auto t : threads) {
		t->join();
	}
	*/

	// run in single thread
	scanPixels(rect.x, rect.x + rect.width, pixels, count);

	// 直前の状態
	float pastActivity = activity;

	// 明るさの変わっているピクセルの割合（100%なら1.0）を計算
	activity = (float)count / areaSize;

	if (isFirstUpdate) {
		isFirstUpdate = false;
		return;
	}

	// bangを出すかどうか
	needSendBang = pastActivity < *SensingArea::bangThreshold && activity >= *SensingArea::bangThreshold;

	graph.addValue(activity, needSendBang);
}

void SensingArea::draw() {
	ofPushMatrix();
	ofTranslate(rect.x, rect.y);

	// appear effect
	float time = ofGetElapsedTimef() - initializedTime;
	const float duration = 0.12;
	if (time < duration) {
		float t = time / duration;
		ofScale(1.0, -t*t + 2*t);
	}

	// 状態によって線の太さと色を変える
	if (activity > *bangThreshold) {
		ofSetLineWidth(lineWidthHighlighted);
		ofSetColor(255);
	}
	else {
		ofSetLineWidth(lineWidth);
		ofSetColor(0, 255, 0);
	}

	if (this == selected) {
		ofSetColor(255, 255, 0);
	}

	// 描画。タイトルバーの線なども描く
	float titlebarSizeScaled = titlebarSize / DrawScaleManager::drawScale.y;
	float cornerSizeScaled = cornerSize / DrawScaleManager::drawScale.y;
	ofDrawRectangle(0, 0, rect.width, rect.height);
	ofDrawLine(0, titlebarSizeScaled, rect.width, titlebarSizeScaled);
	ofDrawLine(rect.width, rect.height - cornerSizeScaled, rect.width - cornerSizeScaled, rect.height);
	// バッテン
	ofDrawLine(rect.width, 0, rect.width - titlebarSizeScaled, titlebarSizeScaled);
	ofDrawLine(rect.width - titlebarSizeScaled, 0, rect.width, titlebarSizeScaled);

	// index
	ofDrawBitmapString(ofToString(index), 1, titlebarSizeScaled - 2 / DrawScaleManager::drawScale.y);

	// 楕円
	if (type == circleArea) {
		ofDrawEllipse(rect.width / 2, rect.height / 2,
			rect.width, rect.height);
	}

	ofPopMatrix();

	graph.draw();
}

bool SensingArea::selectIfTitlebarHitted(float x, float y) {
	// タイトルバーをクリックしていたら移動開始
	// 角をクリックしていたらそれを選択。サイズを変更可能になる
	titleGrabbed = false;
	cornerGrabbed = false;

	if (rect.inside(x, y)) {
		float titlebarSizeScaled = titlebarSize / DrawScaleManager::drawScale.x;
		float cornerSizeScaled = cornerSize / DrawScaleManager::drawScale.y;

		// バッテンをクリックしていたら削除
		if (x > rect.x + rect.width - titlebarSizeScaled && y < rect.y + titlebarSizeScaled) {
			deleteFlag = true;
		}

		// コーナーをクリックしていたら、ドラッグでサイズ変更できるように掴む
		// タイトルバーのドラッグよりも優先する
		if (x > rect.x + rect.width - cornerSizeScaled && y > rect.y + rect.height - cornerSizeScaled) {
			SensingArea::selected = this;
			cornerGrabbed = true;
		}

		// タイトルバーをクリックしていたら、ドラッグ移動できるように掴む
		else if (y < rect.y + titlebarSizeScaled) {
			SensingArea::selected = this;
			titleGrabbed = true;
			return true;
		}
		else {
			return false;
		}
	}
}

bool SensingArea::mouseDraggedIfSelected() {
	float x = DrawScaleManager::getScaledMouseX();
	float y = DrawScaleManager::getScaledMouseY();
	float px = DrawScaleManager::getScaledPreviousMouseX();
	float py = DrawScaleManager::getScaledPreviousMouseY();

	// 選択しているところがタイトルだったら
	if (titleGrabbed) {
		ofRectangle newRect(
			rect.x + x - px,
			rect.y + y - py,
			rect.width, rect.height);
		setRect(newRect);

		return true;
	}

	// 選択しているところがコーナーだったら
	else if (cornerGrabbed) {
		ofRectangle newRect(
			rect.x, rect.y,
			rect.width + x - px,
			rect.height + y - py);
		setRect(newRect);

		return true;
	}
	else {
		return false;
	}
}

bool SensingArea::mouseReleasedIfSelected() {
	titleGrabbed = false;
	cornerGrabbed = false;

	return false;
}

bool SensingArea::keyPressedIfSelected(char key) {
	// 数字キーなら
	if (48 <= key && key <= 57) {
		int n = key - 48;

		// 番号を変更する
		index = index * 10 + n;
	}
	else {
		switch (key) {
			// ナンバーを1文字消す
		case OF_KEY_BACKSPACE:
			index = index / 10;
			break;

			// 削除する
		case OF_KEY_DEL:
			deleteFlag = true;
			deleteKeyPressed = true;
			break;

			// エリアの形状を変える
		case 't':
			toggleType();
			break;
		}
	}

	return true;
}

void SensingArea::appendOscBundle(ofxOscBundle & bundle) {
	// 生のデータを送信する
	ofxOscMessage activityMessage;
	activityMessage.setAddress("/activity");
	activityMessage.addIntArg(index);
	activityMessage.addFloatArg(activity);
	bundle.addMessage(activityMessage);

	// もしactivityがbangThreshold以上なら、oscでbangを送信する
	if (needSendBang) {
		ofxOscMessage message;
		message.setAddress("/bang");
		message.addIntArg(index);
		bundle.addMessage(message);
	}
}

void SensingArea::setRect(ofRectangle _rect) {
	// avoid out of camera image
	/*
	if (_rect.x + _rect.width > DrawScaleManager::cameraImageWidth) {
		_rect.width = DrawScaleManager::cameraWidth - _rect.x;
	}
	if (_rect.y + _rect.height > DrawScaleManager::cameraHeight) {
		_rect.height = DrawScaleManager::cameraHeight - _rect.y;
	}
	*/

	// avoid too small
	if (_rect.width < SensingArea::minimumWidth) _rect.width = SensingArea::minimumWidth;
	if (_rect.height < SensingArea::minimumHeight) _rect.height = SensingArea::minimumHeight;

	rect = _rect;
	graph.setRect(ofRectangle(
		rect.x, rect.y + titlebarSize,
		rect.width, rect.height - titlebarSize
	));

	areaSize = rect.width * rect.height;

	center.set(rect.x + rect.width / 2, rect.y + rect.height / 2);
}

void SensingArea::initialize() {
	isFirstUpdate = true;
	graph.clear();
	titleGrabbed = false;
	cornerGrabbed = false;
	deleteFlag = false;
	deleteKeyPressed = false;
	initializedTime = ofGetElapsedTimef();
}

float SensingArea::sq(float x) {
	return x * x;
}

void SensingArea::scanPixels(int ixStart, int ixEnd, ofPixels &pixels, atomic<long> &count) {
	int channels = pixels.getNumChannels();
	int localCount = 0;
	for (int ix = ixStart; ix < ixEnd; ++ix) {
		for (int iy = rect.y; iy < rect.y + rect.height; ++iy) {
			// typeがcircleAreの場合は、範囲を楕円形に切り取る
			if (type == circleArea) {
				float cx = ix - center.x;
				float cy = iy - center.y;
				float crSquared = sq(cx / rect.width * 2) + sq(cy / rect.height * 2);
				if (crSquared > 1) continue;
			}

			int pixelIndex = (ix + iy * pixels.getWidth()) * channels;

			// 1フレーム前から明るさの変わっているピクセルをカウント
			if (pixels[pixelIndex] != 0) {
				++localCount;
			}
		}
	}
	count += localCount;
}

