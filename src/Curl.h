#pragma once

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <thread>
#include <memory>
#include <iostream>
#include "json/json.h"
#include "curl/curl.h"

namespace ofxCurl {

	enum HTTPMethod {
		HTTP_DELETE,
		HTTP_PUT,
		HTTP_POST,
		HTTP_GET,
		HTTP_HEAD
	};

	struct HTTPResponse;

	struct HTTPRequest {
		HTTPMethod mMethod;
		std::string mUrl;
		std::string mParameterString;
		std::function<void(HTTPResponse*)> mCallback;
	};

	struct HTTPResponse {
		long mResponseCode;
		std::string mContentType;
		Json::Value mBody;
		Json::Value mHeaders;
		double mDataSize;
		HTTPRequest mRequest;
	};

	class Curl {
	public:
		static std::shared_ptr<Curl> make();
		~Curl();
		void addHTTPRequest(const HTTPRequest request);
		size_t getNumberOfRequests();
		void setMaxNumberOfThreads(int numThreads);
		int getMaxNumberOfThreads();
		void setUserAgent(const std::string &agent);
		std::string mapToString(const std::map<std::string, std::string> map);
		void makeStringSafe(std::string input);
	protected:
		Curl();
		void updateThreads();
		void loadRequest();
		void setOptions(CURL* curl, HTTPRequest request);
		static size_t writeCallback(const char* contents, size_t size, size_t nmemb, std::string* buffer);
		CURL* mMultiCurl;
		Json::Reader mJsonReader;
		int mMaxNumberOfThreads;
		int mCurrentNumberOfThreads;
		std::string mOutputBuffer;
		std::thread mUpdateThread;
		std::string mUserAgent;
		std::map<CURL*, HTTPRequest> mHandleMap;
		std::queue<HTTPRequest> mRequestQueue;
		static const int MAX_WAIT_MSECS = 30000;
	};

}