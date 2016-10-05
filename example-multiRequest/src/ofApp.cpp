#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	mHTTPClient = Curl::HTTPClient::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::printf("Loading GET request...\n");

	Curl::HTTPRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = Curl::HTTP_GET;
	Get.mParameterString = mHTTPClient->mapToString(requestParameters);
	Get.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mHTTPClient->addHTTPRequest(Get);

	std::printf("Loading POST request...\n");

	Curl::HTTPRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = Curl::HTTP_POST;
	Post.mParameterString = "";
	Post.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mHTTPClient->addHTTPRequest(Post);

	std::printf("Loading PUT request...\n");

	Curl::HTTPRequest Put;
	Put.mUrl = "http://www.httpbin.org/put";
	Put.mMethod = Curl::HTTP_PUT;
	Put.mParameterString = "";
	Put.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mHTTPClient->addHTTPRequest(Put);

	std::printf("Loading DELETE request...\n");

	Curl::HTTPRequest Delete;
	Delete.mUrl = "http://www.httpbin.org/delete";
	Delete.mMethod = Curl::HTTP_DELETE;
	Delete.mParameterString = "";
	Delete.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mHTTPClient->addHTTPRequest(Delete);

	std::printf("Loading HEAD request...\n");

	Curl::HTTPRequest Head;
	Head.mUrl = "http://www.httpbin.org/headers";
	Head.mMethod = Curl::HTTP_HEAD;
	Head.mParameterString = "";
	Head.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); }; mHTTPClient->addHTTPRequest(Head);

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
