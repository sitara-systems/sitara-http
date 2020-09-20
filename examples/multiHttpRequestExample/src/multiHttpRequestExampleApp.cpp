#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "HttpClient.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class multiHttpRequestExampleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	std::shared_ptr<sitara::http::HttpClient> mCurl;
	double mTime;
};

void multiHttpRequestExampleApp::setup() {
	mTime = cinder::app::getElapsedSeconds();

	mCurl = sitara::http::HttpClient::make();

	std::map<std::string, std::string> requestParameters;
	requestParameters["foo"] = "bar baz";

	std::printf("Loading GET request...\n");

	sitara::http::HttpRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = sitara::http::HTTP_GET;
	Get.mCallback = [=](sitara::http::HttpResponse* response, sitara::http::HttpClient* curl) { std::printf("GET Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addRequest(Get);


	std::printf("Loading POST request...\n");

	sitara::http::HttpRequest Post;
	Post.mUrl = "http://www.Httpbin.org/post";
	Post.mMethod = sitara::http::HTTP_POST;
	Post.mCallback = [=](sitara::http::HttpResponse* response, sitara::http::HttpClient* curl) { std::printf("POST Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addRequest(Post);

	std::printf("Loading PUT request...\n");

	sitara::http::HttpRequest Put;
	Put.mUrl = "http://www.Httpbin.org/put";
	Put.mMethod = sitara::http::HTTP_PUT;
	Put.mCallback = [=](sitara::http::HttpResponse* response, sitara::http::HttpClient* curl) { std::printf("PUT Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addRequest(Put);

	std::printf("Loading DELETE request...\n");

	sitara::http::HttpRequest Delete;
	Delete.mUrl = "http://www.Httpbin.org/delete";
	Delete.mMethod = sitara::http::HTTP_DELETE;
	Delete.mCallback = [=](sitara::http::HttpResponse* response, sitara::http::HttpClient* curl) { std::printf("DELETE Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addRequest(Delete);

	std::printf("Loading HEAD request...\n");

	sitara::http::HttpRequest Head;
	Head.mUrl = "http://www.Httpbin.org/headers";
	Head.mMethod = sitara::http::HTTP_HEADERS;
	Head.mCallback = [=](sitara::http::HttpResponse* response, sitara::http::HttpClient* curl) { std::printf("HEAD Request complete with code %d; headers are %s\n", response->mResponseCode, curl->jsonToString(response->mHeaders).c_str()); };
	mCurl->addRequest(Head);
}

void multiHttpRequestExampleApp::mouseDown( MouseEvent event )
{
}

void multiHttpRequestExampleApp::update()
{
}

void multiHttpRequestExampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( multiHttpRequestExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) {settings->setConsoleWindowEnabled(); })
