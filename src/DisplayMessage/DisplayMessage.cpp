#include "DisplayMessage.h"

DisplayMessage DisplayMessage::instance;

DisplayMessage::DisplayMessage() {
}


DisplayMessage::~DisplayMessage() {
}

void DisplayMessage::draw(ofEventArgs &e) {
	if (ofGetElapsedTimef() - pastSetMessageTimef < messageShowTimef) {
        ofPushStyle();
        ofSetColor(0, 150);
        ofFill();
        float marginX = 20;
        float marginY = 6;
        ofDrawRectangle(messageBoundingBox.x - marginX,
                        messageBoundingBox.y - marginY,
                        messageBoundingBox.width + marginX * 2,
                        messageBoundingBox.height + marginY * 2);
        
		ofSetColor(255);
        if (font.isLoaded()) {
            font.drawString(message, messageBoundingBox.x, messageBoundingBox.y + messageBoundingBox.height);
        }
        else{
            ofDrawBitmapString(message, messageBoundingBox.x, messageBoundingBox.y + messageBoundingBox.height);
        }
        ofPopStyle();
	}
}

void DisplayMessage::setMessage(string _message){
	if (!instance.initialized) instance.initialize();
    instance.m_setMessage(_message);
}

void DisplayMessage::m_setMessage(string _message) {
	message = _message;
    if (font.isLoaded()) {
    messageWidth = font.stringWidth(message);
    messageHeight = font.stringHeight(message);
    messageBoundingBox = font.getStringBoundingBox(message, (ofGetWidth() - messageWidth)/2, (ofGetHeight() + messageHeight)/2);
	pastSetMessageTimef = ofGetElapsedTimef();
    }
    else{
        messageWidth = message.length() * 8;
        messageHeight = 13.5;
        messageBoundingBox.set(
                               (ofGetWidth() - messageWidth) / 2,
                              ofGetHeight() / 2 - messageHeight * 2,
                               messageWidth,
                               messageHeight
                               );
    }
    
    ofLog() << message;
}

void DisplayMessage::setFont(string _path, float _fontSize) {
    if (!instance.initialized) instance.initialize();
    instance.m_setFont(_path, _fontSize);
}

void DisplayMessage::m_setFont(string _path, float _fontSize) {
    font.load(_path, _fontSize);
}

void DisplayMessage::initialize() {
	messageShowTimef = 3;
	ofAddListener(ofEvents().draw, this, &DisplayMessage::draw);
	initialized = true;
}

