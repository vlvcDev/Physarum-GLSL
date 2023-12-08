#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	// Load the shader

	ofBackground(0, 0, 0);

	windowWidth = 1200;
	windowHeight = 1080;

	numSlimes = 100000; // The number of slime objects

	slimes.resize(numSlimes);

	if (!slimeShader.loadCompute("slimeShader.glsl")) {
		ofLogError("Shader Program") << "Could not load diffuse shader";
	}
	else {
		ofLogNotice("Shader Program") << "Compute shader loaded successfully";
	}


	if (!diffuseShader.loadCompute("diffuseShader.glsl")) { // Assuming shaders are in "shaders" folder
		ofLogError("Shader Program") << "Could not load diffuse shader";
	}
	else {
		ofLogNotice("Shader Program") << "Diffuse shader loaded successfully";
	}

	// Allocate the texture
	// Initialize the texture that will be used to store the trail map
	trailMapTexture.allocate(windowWidth, windowHeight, GL_RGBA32F); // Assuming 'width' and 'height' are defined
	trailMapTexture.bindAsImage(1, GL_READ_WRITE);

	// Initialize uniform variables
	moveSpeed = 2.0f; // The speed at which slimes move
	sensorSize = 1;
	sensorAngleOffset = 0.0288f;
	sensorOffsetDistance = 28.0f;
	turnSpeed = 0.005f;

	deltaTime = 1.0f / 60;

	diffuseWeight = 0.8f;
	decayRate = 0.00006f;

	//for (int i = 0; i < numSlimes; ++i) {
	//	// Set initial position and angle
	//	slimes[i].position = glm::vec2(windowWidth/2, windowHeight/2);
	//	slimes[i].angle = ofRandom(2 * 3.1415); // Random angle between 0 and 2*PI
	//}

	// Parameters for the circle
	glm::vec2 circleCenter = glm::vec2(windowWidth / 2, windowHeight / 2);
	float circleRadius = std::min(windowWidth, windowHeight) / 4.0f; // Set the radius of the circle

	for (int i = 0; i < numSlimes; ++i) {
		// Generate a random angle and radius
		float angle = ofRandom(TWO_PI);
		float radius = ofRandom(circleRadius); // Random radius within the circle

		// Position each slime inside the circle
		slimes[i].position = glm::vec2(
			circleCenter.x + radius * cos(angle),
			circleCenter.y + radius * sin(angle)
		);

		// Set the angle of each slime to point towards the center of the circle
		slimes[i].angle = atan2(circleCenter.y - slimes[i].position.y, circleCenter.x - slimes[i].position.x);
	}

	// Parameters for the circle
	//glm::vec2 circleCenter = glm::vec2(windowWidth / 2, windowHeight / 2);
	//float circleRadius = 300.0f; // Set the radius of the circle

	//for (int i = 0; i < numSlimes; ++i) {
	//	// Generate a random angle
	//	float angle = ofRandom(TWO_PI);

	//	// The radius is fixed at circleRadius to spawn on the circumference
	//	float radius = circleRadius;

	//	// Position each slime on the circumference of the circle
	//	slimes[i].position = glm::vec2(
	//		circleCenter.x + radius * cos(angle),
	//		circleCenter.y + radius * sin(angle)
	//	);

	//	// Set the angle of each slime to point towards the center of the circle
	//	slimes[i].angle = atan2(circleCenter.y - slimes[i].position.y, circleCenter.x - slimes[i].position.x);
	//}


	// Upload slimes data to GPU
	slimesBuffer.allocate(slimes, GL_STATIC_DRAW); // Use GL_DYNAMIC_DRAW if the data will change often
	// ...


	std::cout << "Size of Slime: " << sizeof(Slime) << " bytes" << std::endl;
	std::cout << "Offset of position: " << offsetof(Slime, position) << " bytes" << std::endl;
	std::cout << "Offset of angle: " << offsetof(Slime, angle) << " bytes" << std::endl;


}

//--------------------------------------------------------------
void ofApp::update() {
	deltaTime = 1.0f / ofGetLastFrameTime();

	// Dispatch shader for slime updates
	slimeShader.begin();
	slimeShader.setUniform1i("numSlimes", numSlimes);
	slimeShader.setUniform1f("deltaTime", deltaTime);
	slimeShader.setUniform1f("moveSpeed", moveSpeed);
	slimeShader.setUniform1i("width", windowWidth);
	slimeShader.setUniform1i("height", windowHeight);
	slimeShader.setUniform1f("diffuseWeight", diffuseWeight);
	slimeShader.setUniform1f("decayRate", decayRate);
	slimeShader.setUniform1i("sensorSize", sensorSize);
	slimeShader.setUniform1f("sensorAngleOffset", sensorAngleOffset);
	slimeShader.setUniform1f("sensorOffsetDistance", sensorOffsetDistance);
	slimeShader.setUniform1f("turnSpeed", turnSpeed);
	slimesBuffer.bindBase(GL_SHADER_STORAGE_BUFFER, 0);
	int numGroupsSlime = (numSlimes + 8 - 1) / 8;
	slimeShader.dispatchCompute(numGroupsSlime, 1, 1);
	slimesBuffer.unbindBase(GL_SHADER_STORAGE_BUFFER, 0);


	// Additional dispatch for texture diffusion
	diffuseShader.begin();
	diffuseShader.setUniform1i("width", windowWidth);
	diffuseShader.setUniform1i("height", windowHeight);
	diffuseShader.setUniform1f("deltaTime", deltaTime);
	diffuseShader.setUniform1f("diffuseWeight", diffuseWeight);
	diffuseShader.setUniform1f("decayRate", decayRate);
	trailMapTexture.bindAsImage(1, GL_READ_WRITE); // Or whatever binding point is used
	int numGroupsX = (windowWidth + 8 - 1) / 8;
	int numGroupsY = (windowHeight + 8 - 1) / 8;
	diffuseShader.dispatchCompute(numGroupsX, numGroupsY, 1);
	diffuseShader.end();
	slimeShader.end();
}

//--------------------------------------------------------------
void ofApp::draw() {
	trailMapTexture.draw(0, 0);

	float fps = ofGetFrameRate();

	ofSetColor(255, 255, 255);
	ofDrawBitmapString("FPS: " + ofToString(fps), 15, 15);

	ofSetColor(255);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
