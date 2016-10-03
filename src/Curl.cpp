#include "Curl.h"

using namespace ofxCurl;

std::shared_ptr<Curl> Curl::make() {
	std::shared_ptr<Curl> curl(new Curl());
	return curl;
}

Curl::Curl() : mErrorBuffer(CURL_ERROR_SIZE) {
	mMaxNumberOfThreads = 5;
	mCurrentNumberOfThreads = 0;
	curl_global_init(CURL_GLOBAL_ALL);
	mMultiCurl = curl_multi_init();
	curl_multi_setopt(mMultiCurl, CURLMOPT_MAXCONNECTS, mMaxNumberOfThreads);
	mOutputBuffer = "";
	mErrorBuffer[0] = 0;
	mUserAgent = "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)";
	mUpdateThread = std::thread(&Curl::updateThreads, this);
}

Curl::~Curl() {
	curl_multi_cleanup(mMultiCurl);
	curl_global_cleanup();
}

void Curl::addHTTPRequest(const HTTPRequest request) {
	mRequestQueue.push(request);
}

size_t Curl::getNumberOfRequests() {
	return mRequestQueue.size();
}

void Curl::setMaxNumberOfThreads(int numThreads) {
	mMaxNumberOfThreads = numThreads;
}

int Curl::getMaxNumberOfThreads() {
	return mMaxNumberOfThreads;
}

void Curl::setUserAgent(const std::string &agent) {
	mUserAgent = agent;
}

std::string ofxCurl::Curl::mapToString(const std::map<std::string, std::string> map) {
	std::string output = "";

	for (auto it = map.begin(); it != map.end(); ++it) {
		if (it != map.begin()) {
			output += "&";
		}
		std::string key = curl_easy_escape(mMultiCurl, it->first.c_str(), 0);
		std::string value = curl_easy_escape(mMultiCurl, it->second.c_str(), 0);
		output += key;
		output += "=";
		output += value;
	}

	return output;
}

void Curl::makeStringSafe(std::string input) {
	input = curl_easy_escape(mMultiCurl, input.c_str(), 0);
}

void Curl::updateThreads() {
	while (true) {
		while (mCurrentNumberOfThreads < mMaxNumberOfThreads && mRequestQueue.size() > 0) {
			loadRequest();
		}
		do {
			int numfds = 0;
			int result = curl_multi_wait(mMultiCurl, NULL, 0, MAX_WAIT_MSECS, &numfds);
			if (result != CURLM_OK) {
				std::printf("ofxCurl ERROR: curl_multi_wait() return %d\n", result);
			}
			curl_multi_perform(mMultiCurl, &mCurrentNumberOfThreads);
		} while (mCurrentNumberOfThreads);

		CURLMsg* message = NULL;
		int  messagesRemaining = 0;

		while ((message = curl_multi_info_read(mMultiCurl, &messagesRemaining))) {
			if (message->msg == CURLMSG_DONE) {
				
				CURL* curlInstance;
				curlInstance = message->easy_handle;

				CURLcode returnCode;
				returnCode = message->data.result;

				if (returnCode != CURLE_OK) {
					std::printf("CURL error code: %d\n", message->data.result);
					continue;
				}

				// Get information about the call we just made
				long responseCode = 0;
				char* url = "";

				curl_easy_getinfo(curlInstance, CURLINFO_RESPONSE_CODE, &responseCode);
				curl_easy_getinfo(curlInstance, CURLINFO_EFFECTIVE_URL, &url);

				std::printf("ofxCurl::updateThreads HTTP Request returned HTTP/1.1 %d for %s\n", responseCode, url);

				curl_multi_remove_handle(mMultiCurl, curlInstance);
				curl_easy_cleanup(curlInstance);
			}
			else {
				fprintf(stderr, "error: after curl_multi_info_read(), CURLMsg=%d\n", message->msg);
			}
		}

	}
}

void Curl::loadRequest() {
	HTTPRequest request = mRequestQueue.front();
			
	CURL* c = curl_easy_init();
	setOptions(c, request);

	curl_multi_add_handle(mMultiCurl, c);

	mRequestQueue.pop();
}

void Curl::setOptions(CURL* curl, HTTPRequest request) {

	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, mErrorBuffer);
	curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
	mOutputBuffer.clear();
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mOutputBuffer);
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, mUserAgent.c_str());
	curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // this line makes it work under https

	if (!mMultiCurl) {
		std::printf("ofxCurl ERROR: Curl hasn't been instantiated!");
	}

	std::string param_url = request.mUrl + "?" + request.mParameterString;

	switch (request.mMethod) {
	case HTTP_POST:
		curl_easy_setopt(curl, CURLOPT_POST, 1);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.mParameterString.c_str());
		curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		break;
	case HTTP_GET:
		if (!request.mParameterString.empty()) {
			curl_easy_setopt(curl, CURLOPT_URL, param_url.c_str());
		}
		else {
			curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		}
		break;
	case HTTP_PUT:
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		break;
	case HTTP_DELETE:
		curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.mParameterString.c_str());
		break;
	case HTTP_HEAD:
		curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
	default:
		break;
	}
}

size_t Curl::writeCallback(const char* contents, size_t size, size_t nmemb, std::string* buffer) {
	size_t realsize = size * nmemb;
	buffer->append(contents, realsize);
	return realsize;
}
