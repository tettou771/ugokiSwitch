#pragma once
#include "ofMain.h"

class DrawScaleManager {
public:
	DrawScaleManager();
	~DrawScaleManager();

	static void push();
	static void pop();

	// 描画サイズのスケーリングと、マウスの座標変換メソッド
	static ofVec2f drawScale;
	static ofVec2f drawPos;
	static float getScaledX(float x) { return (x - drawPos.x) / drawScale.x; }
	static float getScaledY(float y) { return (y - drawPos.y) / drawScale.y; }
	static float getScaledMouseX() { return getScaledX(ofGetMouseX()); }
	static float getScaledMouseY() { return getScaledY(ofGetMouseY()); }
	static float getScaledPreviousMouseX() { return getScaledX(ofGetPreviousMouseX()); }
	static float getScaledPreviousMouseY() { return getScaledY(ofGetPreviousMouseY()); }
	static ofVec2f getScaledMouse() { return ofVec2f(getScaledMouseX(), getScaledMouseY()); }
};

