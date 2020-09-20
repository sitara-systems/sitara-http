#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "HttpClient.h"

using namespace ci;
using namespace ci::app;
using namespace std;

class simpleHttpRequestExampleApp : public App {
  public:
	void setup() override;
	void mouseDown( MouseEvent event ) override;
	void update() override;
	void draw() override;

	std::shared_ptr<sitara::http::HttpClient> mHttpClient;
};

void simpleHttpRequestExampleApp::setup() {
	mHttpClient = sitara::http::HttpClient::make();

	std::printf("Loading GET request...\n");

	sitara::http::HttpRequest Get;
	Get.mUrl = "http://www.httpbin.org/get";
	Get.mMethod = sitara::http::HTTP_GET;
	Get.mCallback = [=](sitara::http::HttpResponse* response, sitara::http::HttpClient* client) { std::printf("GET request complete with code %d\n", response->mResponseCode); };

	sitara::http::HttpResponse get_response = mHttpClient->makeRequest(Get);

	std::printf("Loading POST request...\n");

	sitara::http::HttpRequest Post;
	Post.mUrl = "http://www.httpbin.org/post";
	Post.mMethod = sitara::http::HTTP_POST;
	Post.mCallback = [=](sitara::http::HttpResponse* response, sitara::http::HttpClient* client) { std::printf("POST request complete with code %d\n", response->mResponseCode); };

	sitara::http::HttpResponse post_response = mHttpClient->makeRequest(Post);

	std::printf("All requests complete.\n");
}

void simpleHttpRequestExampleApp::mouseDown( MouseEvent event )
{
}

void simpleHttpRequestExampleApp::update()
{
}

void simpleHttpRequestExampleApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) ); 
}

CINDER_APP( simpleHttpRequestExampleApp, RendererGl, [=](cinder::app::App::Settings* settings) {settings->setConsoleWindowEnabled(); })