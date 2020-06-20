#include "UIButton.h"

UIButton::UIButton() {
}

UIButton::~UIButton() {
	setEnabled(false);
}

void UIButton::setup(string _name, ofRectangle _rect) {
	cout << "UIButton::setup()" << endl;
	name = _name;
	rect.set(_rect);
	enabled = false;
	pushed = false;
	setEnabled(true);
}

void UIButton::draw() {
	if (!enabled) return;

	// 押されていたらちょっとだけ下に動く
	if (pushed) {
		ofPushMatrix();
		ofTranslate(0, 2);
	}
	ofDrawRectangle(rect);
	ofDrawBitmapString(name, rect.x + 4, rect.y + rect.height/2 + 4);
	if (pushed) {
		ofPopMatrix();
	}
}

bool UIButton::inside(int x, int y) {
	return enabled && rect.inside(x, y);
}

void UIButton::mousePressed(ofMouseEventArgs & m) {
	if (inside(m.x, m.y)) {
		pushed = true;
	}
}

void UIButton::mouseReleased(ofMouseEventArgs & m) {
	pushed = false;
}

ofRectangle UIButton::getRect() {
	return rect;
}

void UIButton::setRect(ofRectangle _rect) {
	rect = _rect;
}

string UIButton::getName() {
	return name;
}

void UIButton::setName(string _name) {
	name = _name;
}

bool UIButton::getEnabled() {
	return enabled;
}

void UIButton::setEnabled(bool _enabled) {
	if (enabled == _enabled) return;
	enabled = _enabled;

	if (enabled) {
		ofAddListener(ofEvents().mousePressed, this, &UIButton::mousePressed);
		ofAddListener(ofEvents().mouseReleased, this, &UIButton::mouseReleased);
		cout << "UIButton " << name << " is enabled." << endl;
	}
	else{
		ofRemoveListener(ofEvents().mousePressed, this, &UIButton::mousePressed);
		ofRemoveListener(ofEvents().mouseReleased, this, &UIButton::mouseReleased);
		cout << "UIButton " << name << " is disabled." << endl;
	}
}
