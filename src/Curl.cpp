#include "Curl.h"

using namespace ofxCurl;

std::shared_ptr<Curl> Curl::make() {
	std::shared_ptr<Curl> curl(new Curl());
	return curl;
}

Curl::Curl() : mErrorBuffer(CURL_ERROR_SIZE) {
	curl_global_init(CURL_GLOBAL_ALL);
	mCurl = curl_easy_init();
	mOutputBuffer = "";
	mErrorBuffer[0] = 0;
	mUserAgent = "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)";
}

Curl::~Curl() {
	curl_easy_cleanup(mCurl);
	curl_global_cleanup();
}

std::string Curl::post(std::string &url, std::map<std::string, std::string> &parameters) {
	std::string paramString = mapToString(parameters);

	return easyPerform(url, HTTP_POST, paramString);
}

std::string Curl::post(std::string &url, std::string &parameters) {
	return easyPerform(url, HTTP_POST, parameters);
}

std::string Curl::post(char* url, std::map<std::string, std::string> &parameters) {
	std::string paramString = mapToString(parameters);

	std::string newUrl(url);

	return easyPerform(newUrl, HTTP_POST, paramString);
}

std::string Curl::post(char* url, std::string &parameters) {
	std::string newUrl(url);

	return easyPerform(newUrl, HTTP_POST, parameters);
}

std::string Curl::put(std::string &url, std::map<std::string, std::string> &parameters) {
	std::string paramString = mapToString(parameters);

	return easyPerform(url, HTTP_PUT, paramString);
}

std::string Curl::put(std::string &url, std::string &parameters) {
	return easyPerform(url, HTTP_PUT, parameters);
}

std::string Curl::put(char* url, std::map<std::string, std::string> &parameters) {
	std::string paramString = mapToString(parameters);

	std::string newUrl(url);

	return easyPerform(newUrl, HTTP_PUT, paramString);
}

std::string Curl::put(char* url, std::string &parameters) {
	std::string newUrl(url);

	return easyPerform(newUrl, HTTP_PUT, parameters);
}

std::string Curl::get(std::string &url, std::map<std::string, std::string> &parameters) {
	std::string paramString = mapToString(parameters);
	return easyPerform(url, HTTP_GET, paramString);
}

std::string Curl::get(std::string &url, std::string &parameters) {
	return easyPerform(url, HTTP_GET, parameters);
}

std::string Curl::get(std::string &url) {
	return easyPerform(url, HTTP_GET, std::string(""));
}

std::string Curl::get(char* url, std::map<std::string, std::string> &parameters) {
	std::string paramString = mapToString(parameters);
	std::string newUrl(url);

	return easyPerform(newUrl, HTTP_GET, paramString);
}

std::string Curl::get(char* url, std::string &parameters) {
	std::string newUrl(url);

	return easyPerform(newUrl, HTTP_GET, parameters);
}

std::string Curl::get(char* url) {
	std::string newUrl(url);

	return easyPerform(newUrl, HTTP_GET, std::string(""));
}

std::string Curl::del(std::string &url) {
	return easyPerform(url, HTTP_DELETE, std::string(""));
}

std::string Curl::del(char* url) {
	std::string newUrl(url);

	return easyPerform(newUrl, HTTP_DELETE, std::string(""));
}

std::string Curl::head(std::string &url) {
	return easyPerform(url, HTTP_HEAD, std::string(""));
}

std::string Curl::head(char* url) {
	std::string newUrl(url);

	return easyPerform(newUrl, HTTP_HEAD, std::string(""));
}

std::string Curl::easyPerform(std::string& url, HTTPMethod method, std::string &postParamString) {
	if (!mCurl) {
		return "ofxCurl ERROR: Curl hasn't been instantiated!";
	}

	setOptions();

	curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());

	std::string param_url = url + "?" + postParamString;

	switch (method) {
		case HTTP_POST:
			curl_easy_setopt(mCurl, CURLOPT_POST, 1);
			curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, postParamString.c_str());
			curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
			break;
		case HTTP_GET:
			curl_easy_setopt(mCurl, CURLOPT_URL, param_url.c_str());
			break;
		case HTTP_PUT:
			curl_easy_setopt(mCurl, CURLOPT_CUSTOMREQUEST, "PUT");
			curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
			break;
		case HTTP_DELETE:
			curl_easy_setopt(mCurl, CURLOPT_CUSTOMREQUEST, "DELETE");
			curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
			curl_easy_setopt(mCurl, CURLOPT_POSTFIELDS, postParamString.c_str());
			break;
		case HTTP_HEAD:
			curl_easy_setopt(mCurl, CURLOPT_NOBODY, 1);
			curl_easy_setopt(mCurl, CURLOPT_URL, url.c_str());
		default:
			break;
	}

	CURLcode curlResult = curl_easy_perform(mCurl);

	if (curlResult == CURLE_OK) {
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
	mOutputBuffer.clear();
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
