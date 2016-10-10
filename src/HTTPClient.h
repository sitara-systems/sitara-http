#pragma once

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <thread>
#include <memory>
#include <iostream>
#include "jsoncpp/json.h"
#include "curl/curl.h"

namespace Curl {

	enum HTTPMethod {
		HTTP_DELETE,
		HTTP_PUT,
		HTTP_POST,
		HTTP_GET,
		HTTP_HEAD
	};

	//forward declare
	struct HTTPResponse;
	class HTTPClient;

	struct HTTPRequest {
		HTTPRequest() : mMethod(HTTP_GET), mUrl(""), mParameterString(""), mCallback(NULL) {};

		HTTPMethod mMethod;
		std::string mUrl;
		std::string mParameterString;
		//DataTarget
		std::function<void(HTTPResponse*, HTTPClient*)> mCallback;
	};

	struct HTTPResponse {
		long mResponseCode;
		Json::Value mBody;
		Json::Value mHeaders;
		HTTPRequest mRequest;
	};

	class HTTPClient {
	public:
		static std::shared_ptr<HTTPClient> make();
		~HTTPClient();
		HTTPResponse makeRequest(const HTTPRequest request);
		void addHTTPRequest(const HTTPRequest request);
		size_t getNumberOfRequests();
		void setMaxNumberOfThreads(int numThreads);
		int getMaxNumberOfThreads();
		void setUserAgent(const std::string &agent);
		std::string mapToString(const std::map<std::string, std::string> &map);
		void makeStringSafe(std::string &input);
		std::string MethodToString(const HTTPMethod &method);
		std::string JsonToString(const Json::Value &value);
	protected:
		HTTPClient();
		void updateThreads();
		void loadRequest();
		void setOptions(CURL* curl, const HTTPRequest request);
		void checkForErrors(const CURLcode error_code);
		void checkForMultiErrors(const CURLMcode error_code);
		static size_t writeCallback(const char* contents, size_t size, size_t nmemb, std::string* buffer);
		CURL* mMultiCurl;
		Json::Reader mJsonReader;
		Json::StyledWriter mJsonWriter;
		int mMaxNumberOfThreads;
		int mCurrentNumberOfThreads;
		std::string mOutputBuffer;
		std::vector<char> mErrorBuffer;	
		std::thread mUpdateThread;
		std::string mUserAgent;
		std::map<CURL*, HTTPRequest> mHandleMap;
		std::queue<HTTPRequest> mRequestQueue;
		static const int MAX_WAIT_MSECS = 30000;
	};

}