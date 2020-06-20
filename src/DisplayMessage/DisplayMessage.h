#pragma once
#include "ofMain.h"

// 何かしらのメッセージを画面下に表示するクラス
// 一定時間後に消える

class DisplayMessage
{
public:
	DisplayMessage();
	~DisplayMessage();

	void draw(ofEventArgs &e);
	static void setMessage(string _message);
    static void setFont(string _path, float _fontSize);
	static DisplayMessage instance;

private:
	string message;
	float messageShowTimef;
	float pastSetMessageTimef;
    float messageWidth, messageHeight;
    ofRectangle messageBoundingBox;
	ofTrueTypeFont font;
    
    void m_setMessage(string _message);
    void m_setFont(string _path, float _fontSize);
	
	bool initialized = false;
	void initialize();
};

