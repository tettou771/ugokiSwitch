#pragma once
#include "ofMain.h"
#include "ofxGui.h"
#include "ofxOsc.h"
#include "DrawScaleManager.h"
#include "Graph.h"

enum SensingAreaType {
	rectangleArea,
	circleArea
};

class SensingArea {
public:
	SensingArea();
	SensingArea(int _index, ofRectangle _rect, SensingAreaType _type = rectangleArea) {
		initialize();
		index = _index;
		setRect(_rect);
		type = _type;
	};
	~SensingArea();

	void update(ofPixels &pixels);
	void draw();
	bool selectIfTitlebarHitted(float x, float y);
	bool mouseDraggedIfSelected();
	bool mouseReleasedIfSelected();
	bool keyPressedIfSelected(char key);
	void appendOscBundle(ofxOscBundle &bundle);
	void setRect(ofRectangle _rect);
	void initialize();

	//void setRect(ofRectangle _rect) {rect = _rect;}
	//ofRectangle getRect() { return rect; }

	int index;
	float activity;
	int areaSize;
	ofRectangle rect;

	SensingAreaType type;
	void toggleType() {
		if (type == rectangleArea) type = circleArea;
		else if (type == circleArea) type = rectangleArea;
	}

	bool titleGrabbed, cornerGrabbed;
	bool needSendBang;

	// 削除予定フラグ
	bool deleteFlag;

	// 削除をdeleteKeyでしたかどうか
	// もしそうなら、一つ後か一つ前のsensingAreaに選択のフォーカスが移る
	bool deleteKeyPressed;

	// 選択されているエリア
	static SensingArea *selected;

	// UIで操作する部分
	static ofxFloatSlider *bangThreshold; // 面積のしきい値。5%くらいが妥当

	// line weight (global)
	static const int lineWidth = 2;
	static const int lineWidthHighlighted = 4;

	// エリアのタイトルバーとコーナーのサイズ
	static const int titlebarSize = 12;
	static const int cornerSize = 10;
	static const int minimumWidth = 15;
	static const int minimumHeight = 15;

private:
	Graph graph;
	bool isFirstUpdate = true;
	float initializedTime;

	// to calc as circle area
	ofVec2f center;

	float sq(float x);
	void scanPixels(int ixStart, int ixEnd, ofPixels &pixels, atomic<long> &count);
};

