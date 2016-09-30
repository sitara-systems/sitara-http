#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	mCurl = ofxCurl::Curl::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::cout << "Testing GET request.." << std::endl;
	std::string get_result = mCurl->get("http://www.httpbin.org/get", requestParameters);
	std::printf("Message received: \n %s", get_result.c_str());

	std::cout << "Testing POST request.." << std::endl;
	std::string post_result = mCurl->post("http://www.httpbin.org/post", requestParameters);
	std::printf("Message received: \n %s", post_result.c_str());

	std::cout << "Testing PUT request.." << std::endl;
	std::string put_result = mCurl->put("http://www.httpbin.org/put", requestParameters);
	std::printf("Message received: \n %s", put_result.c_str());

	std::cout << "Testing DELETE request.." << std::endl;
	std::string delete_result = mCurl->del("http://www.httpbin.org/delete");
	std::printf("Message received: \n %s", delete_result.c_str());

	std::cout << "Testing HEAD request.." << std::endl;
	std::string head_result = mCurl->head("http://www.httpbin.org/head");
	std::printf("Message received: \n %s", head_result.c_str());
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
