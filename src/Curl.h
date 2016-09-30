#pragma once

#include <string>
#include <vector>
#include <map>
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

	class Curl {
	public:
		static std::shared_ptr<Curl> make();
		~Curl();
		std::string post(const const std::string &url, const const std::map<std::string, std::string> &parameters);
		std::string post(const const std::string &url, const std::string &parameters);
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
		void setUserAgent(const std::string &agent);
		std::string mapToString(const std::map<std::string, std::string> map);
		void makeStringSafe(std::string input);
	protected:
		Curl();
		void setOptions();
		std::string easyPerform(const std::string &url, HTTPMethod method, const std::string &postParamString);
		static size_t writeCallback(const char* contents, size_t size, size_t nmemb, std::string* buffer);
		CURL* mCurl;
		std::string mOutputBuffer;
		std::string mUserAgent;
		std::vector<char> mErrorBuffer;
	};

}