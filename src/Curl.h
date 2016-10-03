#pragma once

#include <string>
#include <vector>
#include <queue>
#include <map>
#include <thread>
#include <memory>
#include <iostream>
#include "curl/curl.h"

namespace ofxCurl {

	enum HTTPMethod {
		HTTP_DELETE,
		HTTP_PUT,
		HTTP_POST,
		HTTP_GET,
		HTTP_HEAD
	};

	struct HTTPRequest {
		HTTPMethod mMethod;
		std::string mUrl;
		std::string mParameterString;
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
		int mMaxNumberOfThreads;
		int mCurrentNumberOfThreads;
		std::thread mUpdateThread;
		std::string mOutputBuffer;
		std::string mUserAgent;
		std::vector<char> mErrorBuffer;
		std::queue<HTTPRequest> mRequestQueue;
		static const int MAX_WAIT_MSECS = 30000;
	};

}