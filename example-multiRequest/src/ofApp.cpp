#include "ofApp.h"

//--------------------------------------------------------------
using namespace midnight::http;

void ofApp::setup(){
	mIsComplete = false;
	mHttpClient = HttpClient::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::printf("Loading GET request...\n");

	HttpRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = HTTP_GET;
	Get.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; response is %s\n", response->mResponseCode, curl->jsonToString(response->mBody).c_str()); };
	mHttpClient->addHttpRequest(Get);

	std::printf("Loading POST request...\n");

	HttpRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = HTTP_POST;
	Post.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; response is %s\n", response->mResponseCode, curl->jsonToString(response->mBody).c_str()); };
	mHttpClient->addHttpRequest(Post);

	std::printf("Loading PUT request...\n");

	HttpRequest Put;
	Put.mUrl = "http://www.httpbin.org/put";
	Put.mMethod = HTTP_PUT;
	Put.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; response is %s\n", response->mResponseCode, curl->jsonToString(response->mBody).c_str()); };
	mHttpClient->addHttpRequest(Put);

	std::printf("Loading DELETE request...\n");

	HttpRequest Delete;
	Delete.mUrl = "http://www.httpbin.org/delete";
	Delete.mMethod = HTTP_DELETE;
	Delete.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; response is %s\n", response->mResponseCode, curl->jsonToString(response->mBody).c_str()); };
	mHttpClient->addHttpRequest(Delete);

	std::printf("Loading HEAD request...\n");

	HttpRequest Head;
	Head.mUrl = "http://www.httpbin.org/headers";
	Head.mMethod = HTTP_HEADERS;
	Head.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; response is %s\n", response->mResponseCode, curl->jsonToString(response->mBody).c_str()); }; mHttpClient->addHttpRequest(Head);

}

//--------------------------------------------------------------
void ofApp::update(){
	if (mHttpClient->getNumberOfRequests() > 0) {
		std::printf("Waiting for %d responses...\n", mHttpClient->getNumberOfRequests());
	}
	else {
		if (!mIsComplete) {
			std::printf("Done!");
			mIsComplete = true;
		}
	}
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
