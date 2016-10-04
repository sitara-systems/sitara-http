#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	mCurl = ofxCurl::Curl::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::printf("Loading POST request...\n");

	ofxCurl::HTTPRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = ofxCurl::HTTP_POST;
	Post.mParameterString = mCurl->mapToString(requestParameters);

	ofxCurl::HTTPResponse response = mCurl->makeRequest(Post);
	std::printf("Response headers are %s\n", mCurl->JsonToString(response.mHeaders).c_str());
}

//--------------------------------------------------------------
void ofApp::update(){

}

//--------------------------------------------------------------
void ofApp::draw(){

}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
