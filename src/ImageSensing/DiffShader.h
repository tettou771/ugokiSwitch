#pragma once
#include "ofMain.h"

#define GLSL120(shader) "#version 120\n" #shader

class DiffShader {
public:
	DiffShader();
	~DiffShader();

	void makeShader();
	void makeDiff(ofFbo &fboA, ofFbo &fboB, ofFbo &diffFbo, float threshold01);

private:
	ofShader shader;
};

