#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "HTTPClient.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class cinder_multiRequestApp : public App {
public:
	void setup() override;
	void mouseDown(MouseEvent event) override;
	void update() override;
	void draw() override;

	std::shared_ptr<Curl::HTTPClient> mCurl;
	double mTime;
};

void cinder_multiRequestApp::setup() {
	mTime = cinder::app::getElapsedSeconds();

	mCurl = Curl::HTTPClient::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::printf("Loading GET request...\n");

	Curl::HTTPRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = Curl::HTTP_GET;
	Get.mParameterString = mCurl->mapToString(requestParameters);
	Get.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Get);

	std::printf("Loading POST request...\n");

	Curl::HTTPRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = Curl::HTTP_POST;
	Post.mParameterString = "";
	Post.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Post);

	std::printf("Loading PUT request...\n");

	Curl::HTTPRequest Put;
	Put.mUrl = "http://www.httpbin.org/put";
	Put.mMethod = Curl::HTTP_PUT;
	Put.mParameterString = "";
	Put.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Put);

	std::printf("Loading DELETE request...\n");

	Curl::HTTPRequest Delete;
	Delete.mUrl = "http://www.httpbin.org/delete";
	Delete.mMethod = Curl::HTTP_DELETE;
	Delete.mParameterString = "";
	Delete.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); };
	mCurl->addHTTPRequest(Delete);

	std::printf("Loading HEAD request...\n");

	Curl::HTTPRequest Head;
	Head.mUrl = "http://www.httpbin.org/headers";
	Head.mMethod = Curl::HTTP_HEAD;
	Head.mParameterString = "";
	Head.mCallback = [=](Curl::HTTPResponse* response, Curl::HTTPClient* curl) { std::printf("Request complete with code %d; headers are %s\n", response->mResponseCode, curl->JsonToString(response->mHeaders).c_str()); }; mCurl->addHTTPRequest(Head);

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