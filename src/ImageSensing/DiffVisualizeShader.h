#pragma once
#include "ofMain.h"

#define GLSL120(shader) "#version 120\n" #shader

class DiffVisualizeShader {
public:
	DiffVisualizeShader();
	~DiffVisualizeShader();

	void makeShader();
	void drawDiffVisualize(ofFbo &source, ofFbo &diff);

private:
	ofShader shader;
};

