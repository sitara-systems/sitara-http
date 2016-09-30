#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	mCurl = ofxCurl::Curl::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	ofxCurl::HTTPRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = ofxCurl::HTTP_GET;
	Get.mParameterString = "";
	mCurl->addHTTPRequest(Get);

	ofxCurl::HTTPRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = ofxCurl::HTTP_POST;
	Post.mParameterString = "";
	mCurl->addHTTPRequest(Post);

	ofxCurl::HTTPRequest Put;
	Put.mUrl = "http://www.httpbin.org/put";
	Put.mMethod = ofxCurl::HTTP_PUT;
	Put.mParameterString = "";
	mCurl->addHTTPRequest(Put);

	ofxCurl::HTTPRequest Delete;
	Delete.mUrl = "http://www.httpbin.org/delete";
	Delete.mMethod = ofxCurl::HTTP_DELETE;
	Delete.mParameterString = "";
	mCurl->addHTTPRequest(Delete);

	ofxCurl::HTTPRequest Head;
	Head.mUrl = "http://www.httpbin.org/head";
	Head.mMethod = ofxCurl::HTTP_HEAD;
	Head.mParameterString = "";
	mCurl->addHTTPRequest(Head);

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
