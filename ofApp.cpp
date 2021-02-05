#include "ofApp.h"	

//--------------------------------------------------------------
void ofApp::setup() {

	ofSetFrameRate(60);
	ofSetWindowTitle("openFrameworks");

	ofBackground(0);
	ofEnableBlendMode(ofBlendMode::OF_BLENDMODE_ADD);
	ofSetColor(255);

	this->number_of_targets = 30;
	for (int i = 0; i < this->number_of_targets; i++) {

		this->target_list.push_back(glm::vec2());
		this->color_list.push_back(glm::vec3(1, 1, 1));
	}

	this->shader.load("shader/shader.vert", "shader/shader.frag");
}

//--------------------------------------------------------------
void ofApp::update() {

	ofSeedRandom(39);
}

//--------------------------------------------------------------
void ofApp::draw() {

	vector<glm::vec2> location_list;
	vector<float> deg_list;

	for (int i = 0; i < this->number_of_targets; i++) {

		location_list.push_back(glm::vec2(
			ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.002), 0, 1, 50, ofGetWidth() - 50),
			ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.002), 0, 1, 50, ofGetHeight() - 50)));
		deg_list.push_back(ofMap(ofNoise(ofRandom(1000), ofGetFrameNum() * 0.005), 0, 1, -180, 180));
	}

	for (int i = 0; i < location_list.size(); i++) {

		int near_count = 0;
		for (int k = 0; k < location_list.size(); k++) {

			if (i == k) { continue; }

			auto distance = glm::distance(location_list[i], location_list[k]);
			if (distance < 100) {

				auto handle_1 = location_list[i] + glm::vec2(50 * cos(deg_list[i] * DEG_TO_RAD), 50 * sin(deg_list[i] * DEG_TO_RAD));
				auto handle_2 = location_list[k] + glm::vec2(50 * cos(deg_list[k] * DEG_TO_RAD), 50 * sin(deg_list[k] * DEG_TO_RAD));

				ofNoFill();
				ofBeginShape();
				ofVertex(location_list[i]);
				ofBezierVertex(handle_1, handle_2, location_list[k]);
				ofEndShape();

				near_count++;
			}
		}

		float value = near_count > 3 ? 1.5 : ofMap(near_count, 0, 3, 0.2, 1.5);
		this->target_list[i] = location_list[i];
		this->color_list[i] = glm::vec3(value);
	}

	ofFill();
	this->shader.begin();
	this->shader.setUniform1f("time", ofGetElapsedTimef());
	this->shader.setUniform2f("resolution", ofGetWidth(), ofGetHeight());
	this->shader.setUniform2fv("targets", &this->target_list[0].x, this->number_of_targets);
	this->shader.setUniform3fv("colors", &this->color_list[0].x, this->number_of_targets);

	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());

	this->shader.end();
}

//--------------------------------------------------------------
bool ofApp::calculate_angle(glm::vec2 location_1, float radius_1, glm::vec2 location_2, float radius_2, double& theta, double& a) {

	auto distance = glm::distance(location_1, location_2);
	if (distance > radius_1 + radius_2 || distance < radius_1 - radius_2) { return false; }

	theta = atan2(location_2.y - location_1.y, location_2.x - location_1.x);
	auto cos_a = (pow(distance, 2) + pow(radius_1, 2) - pow(radius_2, 2)) / (2 * distance * radius_1);
	a = acos(cos_a);
	return true;
}

//--------------------------------------------------------------
int main() {

	ofGLWindowSettings settings;
	settings.setGLVersion(3, 2);
	settings.setSize(720, 720);
	ofCreateWindow(settings);
	ofRunApp(new ofApp());
}