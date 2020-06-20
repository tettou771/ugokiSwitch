#include "DiffVisualizeShader.h"



DiffVisualizeShader::DiffVisualizeShader() {
}


DiffVisualizeShader::~DiffVisualizeShader() {
}

void DiffVisualizeShader::makeShader() {
	string fragmentShader = GLSL120(

		uniform sampler2DRect source;
	uniform sampler2DRect diff;

	void main(void) {
		vec2 st = gl_FragCoord.xy;
		//st.y = st.y;

		vec4 s = texture2DRect(source, st);
		vec4 d = texture2DRect(diff, st);

		float gray = (s.r + s.g + s.b) / 3.0 * 0.8;
		gl_FragColor = vec4(gray + d.r, gray, gray + d.r, 1);
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

void DiffVisualizeShader::drawDiffVisualize(ofFbo & source, ofFbo & diff) {
    ofPushStyle();
	ofFill();
	shader.begin();
	shader.setUniformTexture("source", source, source.getId());
	shader.setUniformTexture("diff", diff, diff.getId());
	ofDrawRectangle(0, 0, diff.getWidth(), diff.getHeight());
	shader.end();
	ofPopStyle();
}
