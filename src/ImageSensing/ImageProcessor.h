#pragma once
#include "ofMain.h"

#include "DiffShader.h"
#include "DiffVisualizeShader.h"

class ImageProcessor {
public:
	ImageProcessor();
	~ImageProcessor();

	void setup(int _width, int _height);
	void setNewTexture(ofTexture &newTexture, bool hFlip, bool vFlip);
	ofPixels &getDiffPixels();

	void setBrightnessThreshold(float _brightnessThreshold);
	void drawDiffVisualize();
private:
	int width, height;
	ofFbo * current, *past;
	ofFbo fboA, fboB, diff;
	ofPixels diffPixels;
	void swapFbo();

    DiffShader diffShader;
	DiffVisualizeShader diffVisualizeShader;

	float brightnessThreshold01;
};

