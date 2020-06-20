#include "DrawScaleManager.h"

ofVec2f DrawScaleManager::drawScale;
ofVec2f DrawScaleManager::drawPos;

DrawScaleManager::DrawScaleManager() {
}

DrawScaleManager::~DrawScaleManager() {
}

void DrawScaleManager::push() {
	ofPushMatrix();
	ofTranslate(drawPos);
	ofScale(drawScale.x, drawScale.y);
}

void DrawScaleManager::pop() {
	ofPopMatrix();
}
