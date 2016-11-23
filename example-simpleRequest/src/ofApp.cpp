#include "ofApp.h"

//--------------------------------------------------------------
using namespace midnight::http;

void ofApp::setup(){
	mHttpClient = HttpClient::make();

	std::printf("Loading GET request...\n");

	HttpRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = HTTP_GET;
	Get.mCallback = [=](HttpResponse* response, HttpClient* client) { std::printf("GET request complete with code %d\n", response->mResponseCode); };

	HttpResponse get_response = mHttpClient->makeRequest(Get);

	std::printf("Loading POST request...\n");

	HttpRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = HTTP_POST;
	Post.mCallback = [=](HttpResponse* response, HttpClient* client) { std::printf("POST request complete with code %d\n", response->mResponseCode); };

	HttpResponse post_response = mHttpClient->makeRequest(Post);

	std::printf("All requests complete.\n");
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
