#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "ofxCurl.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class cinder_multiRequestApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	std::shared_ptr<ofxCurl::Curl> mCurl;
	double mTime;
};

void cinder_multiRequestApp::setup() {
	mTime = cinder::app::getElapsedSeconds();

	mCurl = ofxCurl::Curl::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::printf("Loading GET request...\n");

	ofxCurl::HTTPRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = ofxCurl::HTTP_GET;
	Get.mParameterString = mCurl->mapToString(requestParameters);
	Get.mCallback = [=](ofxCurl::HTTPResponse* response, ofxCurl::Curl* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Get);

	std::printf("Loading POST request...\n");

	ofxCurl::HTTPRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = ofxCurl::HTTP_POST;
	Post.mParameterString = "";
	Post.mCallback = [=](ofxCurl::HTTPResponse* response, ofxCurl::Curl* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Post);

	std::printf("Loading PUT request...\n");

	ofxCurl::HTTPRequest Put;
	Put.mUrl = "http://www.httpbin.org/put";
	Put.mMethod = ofxCurl::HTTP_PUT;
	Put.mParameterString = "";
	Put.mCallback = [=](ofxCurl::HTTPResponse* response, ofxCurl::Curl* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Put);

	std::printf("Loading DELETE request...\n");

	ofxCurl::HTTPRequest Delete;
	Delete.mUrl = "http://www.httpbin.org/delete";
	Delete.mMethod = ofxCurl::HTTP_DELETE;
	Delete.mParameterString = "";
	Delete.mCallback = [=](ofxCurl::HTTPResponse* response, ofxCurl::Curl* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Delete);

	std::printf("Loading HEAD request...\n");

	ofxCurl::HTTPRequest Head;
	Head.mUrl = "http://www.httpbin.org/headers";
	Head.mMethod = ofxCurl::HTTP_HEAD;
	Head.mParameterString = "";
	Head.mCallback = [=](ofxCurl::HTTPResponse* response, ofxCurl::Curl* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); }; mCurl->addHTTPRequest(Head);

}

void cinder_multiRequestApp::mouseDown(MouseEvent event)
{
}

void cinder_multiRequestApp::update()
{
	if (cinder::app::getElapsedFrames() % 300 == 0) {
		std::printf("Sending new GET request at %f\n", cinder::app::getElapsedSeconds() - mTime);

		std::map<std::string, std::string> requestParameters;
		requestParameters["foo"] = "bar baz";

		ofxCurl::HTTPRequest Get;
		Get.mUrl = "http://www.httpbin.org/get";
		Get.mMethod = ofxCurl::HTTP_GET;
		Get.mParameterString = mCurl->mapToString(requestParameters);
		Get.mCallback = [=](ofxCurl::HTTPResponse* response, ofxCurl::Curl* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
		mCurl->addHTTPRequest(Get);
	}
}

void cinder_multiRequestApp::draw()
{
	gl::clear(Color(0, 0, 0));
}

CINDER_APP(cinder_multiRequestApp, RendererGl, [=](cinder::app::App::Settings* settings) {settings->setConsoleWindowEnabled(); })