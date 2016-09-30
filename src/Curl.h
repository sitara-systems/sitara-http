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
		std::string post(std::string &url, std::map<std::string, std::string> &parameters);
		std::string post(std::string &url, std::string &parameters);
		std::string post(char* url, std::map<std::string, std::string> &parameters);
		std::string post(char* url, std::string &parameters);
		std::string put(std::string &url, std::map<std::string, std::string> &parameters);
		std::string put(std::string &url, std::string &parameters);
		std::string put(char* url, std::map<std::string, std::string> &parameters);
		std::string put(char* url, std::string &parameters);
		std::string get(std::string &url, std::map<std::string, std::string> &parameters);
		std::string get(std::string &url, std::string &parameters);
		std::string get(std::string &url);
		std::string get(char* url, std::map<std::string, std::string> &parameters);
		std::string get(char* url, std::string &parameters);
		std::string get(char* url);
		std::string del(std::string &url);
		std::string del(char* url);
		std::string head(std::string &url);
		std::string head(char* url);
		void setUserAgent(std::string &agent);
		std::string mapToString(std::map<std::string, std::string> map);
		std::string makeStringSafe(std::string input);
	protected:
		Curl();
		void setOptions();
		std::string easyPerform(std::string &url, HTTPMethod method, std::string &postParamString);
		static size_t writeCallback(char* contents, size_t size, size_t nmemb, std::string* buffer);
		CURL* mCurl;
		std::string mOutputBuffer;
		std::string mUserAgent;
		std::vector<char> mErrorBuffer;
	};

}