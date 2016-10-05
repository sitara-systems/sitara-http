#include "Curl.h"

using namespace ofxCurl;

std::shared_ptr<Curl> Curl::make() {
	std::shared_ptr<Curl> curl(new Curl());
	return curl;
}

Curl::Curl() : mErrorBuffer(CURL_ERROR_SIZE) {
	mMaxNumberOfThreads = 15;
	mCurrentNumberOfThreads = 0;
	curl_global_init(CURL_GLOBAL_ALL);
	mMultiCurl = curl_multi_init();
	mOutputBuffer = "";
	CURLMcode multiCode;
	multiCode = curl_multi_setopt(mMultiCurl, CURLMOPT_MAXCONNECTS, mMaxNumberOfThreads);
	checkForMultiErrors(multiCode);
	mUserAgent = "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)";
	mUpdateThread = std::thread(&Curl::updateThreads, this);
}

Curl::~Curl() {
	curl_multi_cleanup(mMultiCurl);
	curl_global_cleanup();
}

HTTPResponse Curl::makeRequest(const HTTPRequest request) {
	/*
	Makes a blocking request.  This function will cause your program to wait for the request to be completed and then return a string.
	*/
	CURL* c = curl_easy_init();
	setOptions(c, request);

	CURLcode curlCode = curl_easy_perform(c);
	checkForErrors(curlCode);

	HTTPResponse response;
	response.mRequest = request;
	curlCode = curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &response.mResponseCode);
	checkForErrors(curlCode);

	response.mBody = mOutputBuffer;

	return response;
}


void Curl::addHTTPRequest(const HTTPRequest request) {
	/*
	Adds a non-blocking request to the request queue.  Requests will be made when a spot in the queue is available, and then the callback attached to the request object will be called.
	*/
	mRequestQueue.push(request);

	HTTPResponse response;
	response.mRequest = request;
}

size_t Curl::getNumberOfRequests() {
	return mRequestQueue.size();
}

void Curl::setMaxNumberOfThreads(int numThreads) {
	mMaxNumberOfThreads = numThreads;
	CURLMcode multiCode;
	multiCode = curl_multi_setopt(mMultiCurl, CURLMOPT_MAXCONNECTS, mMaxNumberOfThreads);
	checkForMultiErrors(multiCode);
}

int Curl::getMaxNumberOfThreads() {
	return mMaxNumberOfThreads;
}

void Curl::setUserAgent(const std::string &agent) {
	mUserAgent = agent;
}

std::string Curl::mapToString(const std::map<std::string, std::string> &map) {
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

void Curl::makeStringSafe(std::string &input) {
	input = curl_easy_escape(mMultiCurl, input.c_str(), 0);
}

std::string Curl::MethodToString(const HTTPMethod &method) {
	switch (method) {
	case HTTP_DELETE:
		return "DELETE";
	case HTTP_PUT:
		return "PUT";
	case HTTP_POST:
		return "POST";
	case HTTP_GET:
		return "GET";
	case HTTP_HEAD:
		return "HEAD";
	default:
		return NULL;
	}
}

std::string Curl::JsonToString(const Json::Value &value) {
	std::string output = mJsonWriter.write(value);
	return output;
}


void Curl::updateThreads() {
	while (true) {
		CURLMcode multiCode;
		while (mCurrentNumberOfThreads < mMaxNumberOfThreads && mRequestQueue.size() > 0) {
			loadRequest();
		}
		do {
			int numfds = 0;
			multiCode = curl_multi_wait(mMultiCurl, NULL, 0, MAX_WAIT_MSECS, &numfds);
			checkForMultiErrors(multiCode);
		
			multiCode = curl_multi_perform(mMultiCurl, &mCurrentNumberOfThreads);
			checkForMultiErrors(multiCode);

		} while (mCurrentNumberOfThreads);

		CURLMsg* message = NULL;
		int  messagesRemaining = 0;

		while ((message = curl_multi_info_read(mMultiCurl, &messagesRemaining))) {
			if (message->msg == CURLMSG_DONE) {
				
				CURL* curlInstance;
				curlInstance = message->easy_handle;

				CURLcode curlCode;
				curlCode = message->data.result;
				checkForErrors(curlCode);

				// Write information from the request to a HTTPResponse object
				HTTPResponse response;
				response.mRequest = mHandleMap[curlInstance];
				curlCode = curl_easy_getinfo(curlInstance, CURLINFO_RESPONSE_CODE, &response.mResponseCode);
				checkForErrors(curlCode);

				Json::Value full_response;
				bool parsingSuccessful = mJsonReader.parse(mOutputBuffer, full_response);     //parse process
				if (!parsingSuccessful) {
					std::cout << "Failed to parse JSON " << mJsonReader.getFormattedErrorMessages();
				}
				response.mHeaders = full_response["headers"];
				// TO BE IMPLEMENTED -- load body into JSON object
				//response.mBody = full_response["Body"];
				response.mBody = NULL;

				mHandleMap[curlInstance].mCallback(&response, this);

				curl_multi_remove_handle(mMultiCurl, curlInstance);
				curl_easy_cleanup(curlInstance);
			}
			else {
				std::fprintf(stderr, "error: after curl_multi_info_read(), CURLMsg=%d\n", message->msg);
			}
		}

	}
}

void Curl::loadRequest() {
	HTTPRequest request = mRequestQueue.front();
			
	CURL* c = curl_easy_init();
	setOptions(c, request);

	mHandleMap[c] = request;

	CURLMcode multiCode;
	curl_multi_add_handle(mMultiCurl, c);

	mRequestQueue.pop();
}

void Curl::setOptions(CURL* curl, const HTTPRequest request) {
	CURLcode curlCode;
	curlCode = curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	checkForErrors(curlCode);
	curlCode = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	checkForErrors(curlCode);
	curlCode = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeCallback);
	checkForErrors(curlCode);
	mOutputBuffer.clear();
	curlCode = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mOutputBuffer);
	checkForErrors(curlCode);
	curlCode = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
	checkForErrors(curlCode);
	curlCode = curl_easy_setopt(curl, CURLOPT_USERAGENT, mUserAgent.c_str());
	checkForErrors(curlCode);
	curlCode = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // this line makes it work under https
	checkForErrors(curlCode);

	if (!mMultiCurl) {
		std::printf("ofxCurl ERROR: Curl hasn't been instantiated!");
	}

	std::string param_url = request.mUrl + "?" + request.mParameterString;

	switch (request.mMethod) {
	case HTTP_POST:
		curlCode = curl_easy_setopt(curl, CURLOPT_POST, 1);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.mParameterString.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		break;
	case HTTP_GET:
		if (!request.mParameterString.empty()) {
			curlCode = curl_easy_setopt(curl, CURLOPT_URL, param_url.c_str());
			checkForErrors(curlCode);
		}
		else {
			curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
			checkForErrors(curlCode);
		}
		break;
	case HTTP_PUT:
		curlCode = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		break;
	case HTTP_DELETE:
		curlCode = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.mParameterString.c_str());
		checkForErrors(curlCode);
		break;
	case HTTP_HEAD:
		curlCode = curl_easy_setopt(curl, CURLOPT_NOBODY, 1);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
	default:
		break;
	}
}

void Curl::checkForErrors(const CURLcode error_code) {
	std::string errorString = curl_easy_strerror(error_code);
	if (error_code != CURLE_OK) {
		std::printf("ofxCurl::Curl ERROR: cURL failed: %s\n", errorString.c_str());
	}
}

void Curl::checkForMultiErrors(const CURLMcode error_code) {
	std::string errorString = curl_multi_strerror(error_code);
	if (error_code != CURLM_OK) {
		std::printf("ofxCurl::Curl ERROR: multi-cURL failed: %s\n", errorString.c_str());
	}
}

size_t Curl::writeCallback(const char* contents, size_t size, size_t nmemb, std::string* buffer) {
	size_t realsize = size * nmemb;
	buffer->append(contents, realsize);
	return realsize;
}
