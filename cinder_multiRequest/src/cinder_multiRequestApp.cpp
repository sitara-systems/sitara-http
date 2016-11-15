#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "HTTPClient.h"

using namespace ci;
using namespace ci::app;
using namespace std;
using namespace midnight::http;

class cinder_multiRequestApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	std::shared_ptr<HTTPClient> mCurl;
	double mTime;
};

void cinder_multiRequestApp::setup() {
	mTime = cinder::app::getElapsedSeconds();

	mCurl = HTTPClient::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::printf("Loading GET request...\n");

	HTTPRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = HTTP_GET;
	Get.mRequestBody = mCurl->mapToString(requestParameters);
	Get.mCallback = [=](HTTPResponse* response, HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Get);

	std::printf("Loading POST request...\n");

	HTTPRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = HTTP_POST;
	Post.mCallback = [=](HTTPResponse* response, HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Post);

	std::printf("Loading PUT request...\n");

	HTTPRequest Put;
	Put.mUrl = "http://www.httpbin.org/put";
	Put.mMethod = HTTP_PUT;
	Put.mCallback = [=](HTTPResponse* response, HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Put);

	std::printf("Loading DELETE request...\n");

	HTTPRequest Delete;
	Delete.mUrl = "http://www.httpbin.org/delete";
	Delete.mMethod = HTTP_DELETE;
	Delete.mCallback = [=](HTTPResponse* response, HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Delete);

	std::printf("Loading HEAD request...\n");

	HTTPRequest Head;
	Head.mUrl = "http://www.httpbin.org/headers";
	Head.mMethod = HTTP_HEADERS;
	Head.mCallback = [=](HTTPResponse* response, HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); }; mCurl->addHTTPRequest(Head);

}

void cinder_multiRequestApp::mouseDown(MouseEvent event)
{
}

void cinder_multiRequestApp::update() {
}

void cinder_multiRequestApp::draw()
{
	gl::clear(Color(0, 0, 0));
}

CINDER_APP(cinder_multiRequestApp, RendererGl, [=](cinder::app::App::Settings* settings) {settings->setConsoleWindowEnabled(); })