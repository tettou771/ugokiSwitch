#include "DiffShader.h"



DiffShader::DiffShader() {
}


DiffShader::~DiffShader() {
}

void DiffShader::makeShader() {
	string fragmentShader = GLSL120(

		uniform sampler2DRect A;
	uniform sampler2DRect B;
	uniform float threshold;

	void main(void) {
		vec2 st = gl_FragCoord.xy;

		vec4 a = texture2DRect(A, st);
		vec4 b = texture2DRect(B, st);

		float diff = (abs(a.r - b.r) + abs(a.g - b.g) + abs(a.b - b.b)) / 3;
		if (diff >= threshold) {
			gl_FragColor = vec4(1, 1, 1, 1);
		}
		else {
			gl_FragColor = vec4(0, 0, 0, 1);
		}
	}
	);

	string vertexShader = GLSL120(
		void main() {
		gl_Position = ftransform();
	}
	);

	ofStringReplace(fragmentShader, ";", ";\n");
	ofStringReplace(vertexShader, ";", ";\n");

	shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragmentShader);
	shader.setupShaderFromSource(GL_VERTEX_SHADER, vertexShader);
	shader.bindDefaults();
	shader.linkProgram();
}

void DiffShader::makeDiff(ofFbo & fboA, ofFbo & fboB, ofFbo & diffFbo, float threshold01) {
	ofPushStyle();
	ofFill();
	diffFbo.begin();
	shader.begin();
	shader.setUniform1f("threshold", threshold01);
	shader.setUniformTexture("A", fboA.getTexture(), fboA.getId());
	shader.setUniformTexture("B", fboB.getTexture(), fboB.getId());
	ofDrawRectangle(0, 0, diffFbo.getWidth(), diffFbo.getHeight());
	shader.end();
	diffFbo.end();
	ofPopStyle();
}
