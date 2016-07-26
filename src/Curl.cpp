#include "Curl.h"

using namespace ofxCurl;

std::shared_ptr<Curl> Curl::make() {
	std::shared_ptr<Curl> curl(new Curl());
	return curl;
}

Curl::Curl() : mErrorBuffer(CURL_ERROR_SIZE) {
	mCurl = curl_easy_init();
	mOutputBuffer = "";
	mErrorBuffer[0] = 0;
	mUserAgent = "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)";
}

Curl::~Curl() {
}

std::string Curl::post(std::string &url, std::map<std::string, std::string> &parameters) {
	bool usePost = true;

	std::string postString = mapToString(parameters);

	return easyPerform(url, usePost, postString);
}

std::string Curl::post(std::string &url, std::string &parameters) {
	bool usePost = true;

	return easyPerform(url, usePost, parameters);
}

std::string Curl::post(std::string &url) {
	bool usePost = true;

	return easyPerform(url, usePost, std::string(""));
}

std::string Curl::post(char* url, std::map<std::string, std::string> &parameters) {
	bool usePost = true;
	std::string postString = mapToString(parameters);

	std::string newUrl(url);

	return easyPerform(newUrl, usePost, postString);
}

std::string Curl::post(char* url, std::string &parameters) {
	bool usePost = true;

	std::string newUrl(url);

	return easyPerform(newUrl, usePost, parameters);
}

std::string Curl::post(char* url) {
	bool usePost = true;

	std::string newUrl(url);

	return easyPerform(newUrl, usePost, std::string(""));
}

std::string Curl::get(std::string &url) {
	return easyPerform(url, false, std::string(""));
}

std::string Curl::get(char* url) {
	std::string newUrl(url);

	return easyPerform(newUrl, false, std::string(""));
}

std::string Curl::easyPerform(std::string& url, bool post, std::string &postParamString) {
	if (!mCurl) {
		return "";
	}

	setOptions();

	curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
	if (post) {
		std::printf("posting url %s", url.c_str());

		curl_easy_setopt(mCurl, CURLOPT_POST, 1);
		curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, postParamString.c_str());
	}
	mCurlResult = curl_easy_perform(mCurl);

	curl_easy_cleanup(mCurl);

	if (mCurlResult == CURLE_OK) {
		return mOutputBuffer;
	}
	else {
		std::printf("ofxCurl ERROR: %s", mErrorBuffer.data());
		return "";
	}
}

void Curl::setUserAgent(std::string &agent) {
	mUserAgent = agent;
}

std::string ofxCurl::Curl::mapToString(std::map<std::string, std::string> map) {
	std::string output = "";

	for (auto it = map.begin(); it != map.end(); ++it) {
		if (it != map.begin()) {
			output += "&";
		}
		std::string key = curl_easy_escape(mCurl, it->first.c_str(), 0);
		std::string value = curl_easy_escape(mCurl, it->second.c_str(), 0);
		output += key;
		output += "=";
		output += value;
	}

	return output;
}

std::string Curl::makeStringSafe(std::string input) {
	std::string output = curl_easy_escape(mCurl, input.c_str(), 0);
	return output;
}

void Curl::setOptions() {
	curl_easy_setopt(mCurl, CURLOPT_ERRORBUFFER, mErrorBuffer);
	curl_easy_setopt(mCurl, CURLOPT_HEADER, 0);
	curl_easy_setopt(mCurl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(mCurl, CURLOPT_WRITEFUNCTION, &writeCallback);
	curl_easy_setopt(mCurl, CURLOPT_WRITEDATA, &mOutputBuffer);
	curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYHOST, 2);
	curl_easy_setopt(mCurl, CURLOPT_USERAGENT, mUserAgent.c_str());
	curl_easy_setopt(mCurl, CURLOPT_SSL_VERIFYPEER, 0); // this line makes it work under https
}

size_t Curl::writeCallback(char* contents, size_t size, size_t nmemb, std::string* buffer) {
	size_t realsize = size * nmemb;
	buffer->append(contents, realsize);
	return realsize;
}
