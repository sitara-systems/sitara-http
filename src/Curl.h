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
		std::string post(const std::string &url, const std::map<std::string, std::string> &parameters);
		std::string post(const std::string &url, const std::string &parameters);
		std::string post(const char* url, const std::map<std::string, std::string> &parameters);
		std::string post(const char* url, const std::string &parameters);
		std::string put(const std::string &url, const std::map<std::string, std::string> &parameters);
		std::string put(const std::string &url, const std::string &parameters);
		std::string put(const char* url, const std::map<std::string, std::string> &parameters);
		std::string put(const char* url, const std::string &parameters);
		std::string get(const std::string &url, const std::map<std::string, std::string> &parameters);
		std::string get(const std::string &url, const std::string &parameters);
		std::string get(const std::string &url);
		std::string get(const char* url, const std::map<std::string, std::string> &parameters);
		std::string get(const char* url, const std::string &parameters);
		std::string get(const char* url);
		std::string del(const std::string &url);
		std::string del(const char* url);
		std::string head(const std::string &url);
		std::string head(const char* url);
		void setOptions(CURL* curl);
		std::string easyPerform(const std::string &url, HTTPMethod method, const std::string &postParamString);
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