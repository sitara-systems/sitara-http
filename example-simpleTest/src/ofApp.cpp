#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	mCurl = ofxCurl::Curl::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::printf("Loading GET request...\n");

	ofxCurl::HTTPRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = ofxCurl::HTTP_GET;
	Get.mParameterString = mCurl->mapToString(requestParameters);
	Get.mCallback = [=](ofxCurl::HTTPResponse* response) { std::printf("Request complete with code %d; response was %s.\n", response->mResponseCode, response->mBody.asCString()); };
	mCurl->addHTTPRequest(Get);

	std::printf("Loading POST request...\n");

	ofxCurl::HTTPRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = ofxCurl::HTTP_POST;
	Post.mParameterString = "";
	Post.mCallback = [=](ofxCurl::HTTPResponse* response) { std::printf("Request complete with code %d; response was %s.\n", response->mResponseCode, response->mBody.asCString()); };
	mCurl->addHTTPRequest(Post);

	std::printf("Loading PUT request...\n");

	ofxCurl::HTTPRequest Put;
	Put.mUrl = "http://www.httpbin.org/put";
	Put.mMethod = ofxCurl::HTTP_PUT;
	Put.mParameterString = "";
	Put.mCallback = [=](ofxCurl::HTTPResponse* response) { std::printf("Request complete with code %d; response was %s.\n", response->mResponseCode, response->mBody.asCString()); };
	mCurl->addHTTPRequest(Put);

	std::printf("Loading DELETE request...\n");

	ofxCurl::HTTPRequest Delete;
	Delete.mUrl = "http://www.httpbin.org/delete";
	Delete.mMethod = ofxCurl::HTTP_DELETE;
	Delete.mParameterString = "";
	Delete.mCallback = [=](ofxCurl::HTTPResponse* response) { std::printf("Request complete with code %d; response was %s.\n", response->mResponseCode, response->mBody.asCString()); };
	mCurl->addHTTPRequest(Delete);

	std::printf("Loading HEAD request...\n");

	ofxCurl::HTTPRequest Head;
	Head.mUrl = "http://www.httpbin.org/head";
	Head.mMethod = ofxCurl::HTTP_HEAD;
	Head.mParameterString = "";
	Head.mCallback = [=](ofxCurl::HTTPResponse* response) { std::printf("Request complete with code %d; response was %s.\n", response->mResponseCode, response->mBody.asCString()); };
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
