#pragma once
#include "ofMain.h"

struct ActivityAndBang {
	ActivityAndBang(float _activity, bool _bang)
		:activity(_activity), bang(_bang) {
	}
	float activity;
	bool bang;
};

class Graph {
public:
	Graph();
	Graph(ofRectangle _rect);
	~Graph();

	void draw();
	void setPosition(ofVec2f _pos);
	void setRect(ofRectangle _rect);
	void addValue(float _activity, bool _bang);
	void setLastValueBang();
	void clear();

	ActivityAndBang getLastValue();

private:
	ofRectangle rect;
	vector<ActivityAndBang> values;
	const int maxValueNum = 200;
};

