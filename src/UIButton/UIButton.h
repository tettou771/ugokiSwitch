#pragma once
#include "ofMain.h"

class UIButton {
public:
	UIButton();
	~UIButton();

	void setup(string _name, ofRectangle _rect);
	void draw();
	bool inside(int x, int y);
	void mousePressed(ofMouseEventArgs &m);
	void mouseReleased(ofMouseEventArgs &m);

	ofRectangle getRect();
	void setRect(ofRectangle _rect);
	string getName();
	void setName(string _name);
	bool getEnabled();
	void setEnabled(bool _enabled);

private:
	ofRectangle rect;
	string name;
	bool enabled, pushed;
};

