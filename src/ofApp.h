#pragma once

#include "ofMain.h"

class ofApp : public ofBaseApp {

public:
	void setup();
	void update();
	void draw();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y);
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

	ofShader slimeShader;
	ofShader diffuseShader;
	ofTexture trailMapTexture;

	int numSlimes;
	float deltaTime;
	float moveSpeed;
	float diffuseWeight;
	float decayRate;

	int sensorSize;
	float sensorAngleOffset;
	float sensorOffsetDistance;
	float turnSpeed;

	int windowWidth;
	int windowHeight;

	struct Slime {
		glm::vec2 position;
		float angle;
		float padding;
	};
	std::vector<Slime> slimes;
	ofBufferObject slimesBuffer;
};