#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "HttpClient.h"

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

	std::shared_ptr<HttpClient> mCurl;
	double mTime;
};

void cinder_multiRequestApp::setup() {
	mTime = cinder::app::getElapsedSeconds();

	mCurl = HttpClient::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::printf("Loading GET request...\n");

	HttpRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = HTTP_GET;
s	Get.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addHttpRequest(Get);

	/*
	std::printf("Loading POST request...\n");

	HttpRequest Post;
	Post.mUrl = "http://www.Httpbin.org/post";
	Post.mMethod = HTTP_POST;
	Post.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addHttpRequest(Post);

	std::printf("Loading PUT request...\n");

	HttpRequest Put;
	Put.mUrl = "http://www.Httpbin.org/put";
	Put.mMethod = HTTP_PUT;
	Put.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addHttpRequest(Put);

	std::printf("Loading DELETE request...\n");

	HttpRequest Delete;
	Delete.mUrl = "http://www.Httpbin.org/delete";
	Delete.mMethod = HTTP_DELETE;
	Delete.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addHttpRequest(Delete);

	std::printf("Loading HEAD request...\n");

	HttpRequest Head;
	Head.mUrl = "http://www.Httpbin.org/headers";
	Head.mMethod = HTTP_HEADERS;
	Head.mCallback = [=](HttpResponse* response, HttpClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); }; mCurl->addHttpRequest(Head);
	*/
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