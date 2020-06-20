#include "ImageProcessor.h"



ImageProcessor::ImageProcessor() {
}


ImageProcessor::~ImageProcessor() {
}

void ImageProcessor::setup(int _width, int _height) {
	width = _width;
	height = _height;

	fboA.allocate(width, height, GL_RGB);
	fboB.allocate(width, height, GL_RGB);
	diff.allocate(width, height, GL_RGB);
    diffPixels.allocate(width, height, OF_IMAGE_GRAYSCALE);

	swapFbo();

	diffShader.makeShader();
	diffVisualizeShader.makeShader();
}

void ImageProcessor::setNewTexture(ofTexture & newTexture, bool hFlip, bool vFlip) {
	swapFbo();

	// make current fbo
	current->begin();
	ofSetColor(255);
	ofRectangle rect(0, 0, newTexture.getWidth(), newTexture.getHeight());

	if (hFlip) {
		rect.x += rect.width;
		rect.width = -rect.width;
	}
	if (vFlip) {
		rect.y += rect.height;
		rect.height = -rect.height;
	}

	newTexture.draw(rect);
	current->end();

	// make diff fbo
	diffShader.makeDiff(*current, *past, diff, brightnessThreshold01);

	diff.readToPixels(diffPixels);
}

ofPixels & ImageProcessor::getDiffPixels() {
	return diffPixels;
}

void ImageProcessor::setBrightnessThreshold(float _brightnessThreshold) {
	brightnessThreshold01 = _brightnessThreshold / 255.0;
}

void ImageProcessor::drawDiffVisualize() {
	diffVisualizeShader.drawDiffVisualize(*current, diff);
}

void ImageProcessor::swapFbo() {
	if (current == &fboA) {
		current = &fboB;
		past = &fboA;
	}
	else {
		current = &fboA;
		past = &fboB;
	}
}
