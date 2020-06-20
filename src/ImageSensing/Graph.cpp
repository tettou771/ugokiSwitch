#include "Graph.h"



Graph::Graph() {
}


Graph::Graph(ofRectangle _rect) {
	setRect(_rect);
}

Graph::~Graph() {
}

void Graph::draw() {
	ofPushMatrix();
	ofTranslate(rect.x, rect.y);

	ofNoFill();
	ofSetLineWidth(1);
	ofEnableAntiAliasing();

	// activity graph
	if (values.size() > 1) {
		ofBeginShape();
		for (int i = 0; i < values.size(); ++i) {
			// グラフの動きが小さすぎてわかりにくい場合は、ちょっと倍率をかける
			float magnitude = 1;

			float x = i * rect.width / maxValueNum;
			float y = (1.0 - values[i].activity * magnitude) * rect.height;

			ofVertex(x, y);

			// bang
			if (values[i].bang) {
				ofSetColor(255, 255, 0);
				ofDrawLine(x, 0, x, rect.height);
			}
		}

		ofSetColor(0, 255, 0);
		ofEndShape();
	}

	ofPopMatrix();
}

void Graph::setPosition(ofVec2f _pos) {
	rect.x = _pos.x;
	rect.y = _pos.y;
}

void Graph::setRect(ofRectangle _rect) {
	rect = _rect;
}

void Graph::addValue(float _activity, bool _bang) {
	values.push_back(ActivityAndBang(_activity, _bang));
	if (values.size() > maxValueNum) {
		values.erase(values.begin());
	}
}

void Graph::setLastValueBang() {
	if (!values.empty()) {
		values.back().bang = true;
	}
}

void Graph::clear() {
	values.clear();
}

ActivityAndBang Graph::getLastValue() {
	if (values.empty()) {
		return ActivityAndBang(0, false);
	}
	else {
		return values.back();
	}
}
