#include "HTTPClient.h"

using namespace midnight::http;

std::shared_ptr<HTTPClient> HTTPClient::make() {
	std::shared_ptr<HTTPClient> client(new HTTPClient());
	return client;
}

HTTPClient::HTTPClient() : mErrorBuffer(CURL_ERROR_SIZE) {
	mMaxNumberOfThreads = 15;
	mCurrentNumberOfThreads = 0;
	curl_global_init(CURL_GLOBAL_ALL);
	mMultiCurl = curl_multi_init();
	mResponseBuffer = "";
	mHeaderBuffer = "";
	CURLMcode multiCode;
	multiCode = curl_multi_setopt(mMultiCurl, CURLMOPT_MAXCONNECTS, mMaxNumberOfThreads);
	checkForMultiErrors(multiCode);
	mUserAgent = "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)";
	mUpdateThread = std::thread(&HTTPClient::updateThreads, this);
	mFile = NULL;
}

HTTPClient::~HTTPClient() {
	curl_multi_cleanup(mMultiCurl);
	curl_global_cleanup();
}

HTTPResponse HTTPClient::makeRequest(const HTTPRequest &request) {
	/*
	Makes a blocking request.  This function will cause your program to wait for the request to be completed and then return a string.
	*/
	CURL* c = curl_easy_init();
	setOptions(c, request);

	CURLcode curlCode = curl_easy_perform(c);
	checkForErrors(curlCode);

	
	HTTPResponse response;
	
	// corresponding request
	response.mRequest = request;

	// response code
	curlCode = curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &response.mResponseCode);
	checkForErrors(curlCode);

	// response headers
	std::vector<std::string> headerLines = splitString(mHeaderBuffer, '\n');
	for (auto &line : headerLines) {
		std::vector<std::string> pair = splitString(line, ':');
		if (pair.size() == 2) {
			cleanupString(pair[0]);
			cleanupString(pair[1]);
			response.mHeaders[pair[0]] = pair[1];
		}
	}

	// response body
	Json::Value body = stringToJson(mResponseBuffer);
	if (body != NULL) {
		response.mBody = body;
	}
	else {
		Json::Value textBody;
		textBody["response_body"] = mResponseBuffer;
		response.mBody = textBody;
	}

	// callback attached to request
	request.mCallback(&response, this);

	curl_easy_cleanup(c);

	return response;
}


void HTTPClient::addHTTPRequest(const HTTPRequest request) {
	/*
	Adds a non-blocking request to the request queue.  Requests will be made when a spot in the queue is available, and then the callback attached to the request object will be called.
	*/
	mUpdateMutex.lock();
	mRequestQueue.push(request);
	mUpdateMutex.unlock();
}

size_t HTTPClient::getNumberOfRequests() {
	return mRequestQueue.size();
}

void HTTPClient::setMaxNumberOfThreads(int numThreads) {
	mMaxNumberOfThreads = numThreads;
	CURLMcode multiCode;
	multiCode = curl_multi_setopt(mMultiCurl, CURLMOPT_MAXCONNECTS, mMaxNumberOfThreads);
	checkForMultiErrors(multiCode);
}

int HTTPClient::getMaxNumberOfThreads() {
	return mMaxNumberOfThreads;
}

void HTTPClient::setUserAgent(const std::string &agent) {
	mUserAgent = agent;
}

std::string HTTPClient::getUserAgent() {
	return mUserAgent;
}

std::string HTTPClient::mapToString(const std::map<std::string, std::string> &map) {
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

void HTTPClient::makeStringSafe(std::string &input) {
	input = curl_easy_escape(mMultiCurl, input.c_str(), 0);
}

std::string HTTPClient::methodToString(const HTTPMethod &method) {
	switch (method) {
	case HTTP_DELETE:
		return "DELETE";
	case HTTP_PUT:
		return "PUT";
	case HTTP_POST:
		return "POST";
	case HTTP_PATCH:
		return "PATCH";
	case HTTP_GET:
		return "GET";
	case HTTP_HEADERS:
		return "HEADERS";
	default:
		return NULL;
	}
}

std::string HTTPClient::jsonToString(const Json::Value &value) {
	std::string output = mJsonWriter.write(value);
	return output;
}

Json::Value HTTPClient::stringToJson(const std::string &string) {
	Json::Value output;
	bool parsingSuccessful = mJsonReader.parse(mResponseBuffer, output);
	if (!parsingSuccessful) {
		std::printf("midnight-HTTP::HTTPClient ERROR: Failed to parse JSON %s\n", mJsonReader.getFormattedErrorMessages().c_str());
		return NULL;
	}
	else {
		return output;
	}
}


void HTTPClient::updateThreads() {
	while (true) {
		CURLMcode multiCode;
		mUpdateMutex.lock();
		while (mCurrentNumberOfThreads < mMaxNumberOfThreads && mRequestQueue.size() > 0) {
			loadRequest();
		}
		mUpdateMutex.unlock();
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

				HTTPResponse response;
				
				// corresponding request
				response.mRequest = mHandleMap[curlInstance];

				// response code
				curlCode = curl_easy_getinfo(curlInstance, CURLINFO_RESPONSE_CODE, &response.mResponseCode);
				checkForErrors(curlCode);

				// response headers
				std::vector<std::string> headerLines = splitString(mHeaderBuffer, '\n');
				for (auto &line : headerLines) {
					std::vector<std::string> pair = splitString(line, ':');
					if (pair.size() == 2) {
						cleanupString(pair[0]);
						cleanupString(pair[1]);
						response.mHeaders[pair[0]] = pair[1];
					}
				}

				// response body
				Json::Value body = stringToJson(mResponseBuffer);
				if (body != NULL) {
					response.mBody = body;
				}
				else {
					Json::Value textBody;
					textBody["response_body"] = mResponseBuffer;
					response.mBody = textBody;
				}

				// callback attached to request
				mHandleMap[curlInstance].mCallback(&response, this);

				// cleanup
				if (mFile != NULL) {
					std::fclose(mFile);
				}
				else {
					mResponseBuffer.clear();
				}
				curl_multi_remove_handle(mMultiCurl, curlInstance);
				curl_easy_cleanup(curlInstance);
			}
			else {
				std::printf("midnight-HTTP::HTTPClient ERROR: After curl_multi_info_read(), CURLMsg=%d\n", message->msg);
			}
		}

	}
}

void HTTPClient::loadRequest() {
	HTTPRequest request = mRequestQueue.front();
	
	//makeStringSafe(request.mRequestBody);

	CURL* c = curl_easy_init();
	setOptions(c, request);

	mHandleMap[c] = request;

	CURLMcode multiCode;
	multiCode = curl_multi_add_handle(mMultiCurl, c);
	checkForMultiErrors(multiCode);

	mRequestQueue.pop();
}

void HTTPClient::setOptions(CURL* curl, const HTTPRequest &request) {
	if (!mMultiCurl) {
		std::printf("midnight-HTTP::HTTPClient ERROR: MultiCurl hasn't been instantiated!");
	}

	CURLcode curlCode;

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json");
	curlCode = curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	checkForErrors(curlCode);

	curlCode = curl_easy_setopt(curl, CURLOPT_HEADER, 0);
	checkForErrors(curlCode);
	curlCode = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
	checkForErrors(curlCode);
	//curlCode = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2);
	//checkForErrors(curlCode);
	curlCode = curl_easy_setopt(curl, CURLOPT_USERAGENT, mUserAgent.c_str());
	checkForErrors(curlCode);
	//curlCode = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // this line makes it work under https
	//checkForErrors(curlCode);


	if (request.mTarget == MEMORY) {
		curlCode = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mResponseBuffer);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeToMemory);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_HEADERDATA, &mHeaderBuffer);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &writeToHeaders);
		checkForErrors(curlCode);
	}
	else if (request.mTarget == FILE) {
		// write headers to file also
		curlCode = curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		checkForErrors(curlCode);

		mFile = std::fopen(request.mFilename.c_str(), "w");
		if (mFile == NULL) {
			std::printf("midnight-HTTP::HTTPClient ERROR: Cannot open file %s\n", request.mFilename.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, mFile);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeToFile);
	}

	switch (request.mMethod) {
	case HTTP_POST:
		curlCode = curl_easy_setopt(curl, CURLOPT_POST, 1);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.mRequestBody.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
		checkForErrors(curlCode);
		break;
	case HTTP_GET:
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		if (!request.mRequestBody.empty()) {
			curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.mRequestBody.c_str());
			checkForErrors(curlCode);
			curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
			checkForErrors(curlCode);
		}
		break;
	case HTTP_PUT:
		curlCode = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.mRequestBody.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
		checkForErrors(curlCode);
		break;
	case HTTP_DELETE:
		curlCode = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.mRequestBody.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
		checkForErrors(curlCode);
		break;
	case HTTP_HEADERS:
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		break;
	case HTTP_PATCH:
		curlCode = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request.mRequestBody.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
		checkForErrors(curlCode);
		break;
	default:
		std::printf("midnight-HTTP::HTTPClient HTTP Method not implemented.\n");
		break;
	}
}

void HTTPClient::checkForErrors(const CURLcode error_code) {
	std::string errorString = curl_easy_strerror(error_code);
	if (error_code != CURLE_OK) {
		std::printf("midnight-HTTP::HTTPClient ERROR: cURL failed: %s\n", errorString.c_str());
	}
}

void HTTPClient::checkForMultiErrors(const CURLMcode error_code) {
	std::string errorString = curl_multi_strerror(error_code);
	if (error_code != CURLM_OK) {
		std::printf("midnight-HTTP::HTTPClient ERROR: multi-cURL failed: %s\n", errorString.c_str());
	}
}

size_t HTTPClient::writeToMemory(const char* contents, size_t size, size_t nmemb, std::string* buffer) {
	size_t realsize = size * nmemb;
	buffer->append(contents, realsize);
	return realsize;
}

size_t HTTPClient::writeToFile(const char* contents, size_t size, size_t nmemb, std::FILE* stream) {
	return std::fwrite(contents, size, nmemb, stream);
}

size_t HTTPClient::writeToHeaders(const char *contents, size_t size, size_t nmemb, std::string* data) {
	size_t realsize = size * nmemb;
	data->append(contents, realsize);
	return realsize;
}

void HTTPClient::splitString(const std::string &string, char delimiter, std::vector<std::string> &output) {
	std::stringstream ss;
	ss.str(string);
	std::string item;
	while (std::getline(ss, item, delimiter)) {
		output.push_back(item);
	}
}

std::vector<std::string> HTTPClient::splitString(const std::string &string, char delimiter) {
	std::vector<std::string> output;
	splitString(string, delimiter, output);
	return output;
}

void HTTPClient::cleanupString(std::string &string) {
	while (!string.empty() && std::isspace(*string.begin()))
		string.erase(string.begin());

	while (!string.empty() && std::isspace(*string.rbegin()))
		string.erase(string.length() - 1);

	string.erase(std::remove(string.begin(), string.end(), '\n'), string.end());
	string.erase(std::remove(string.begin(), string.end(), '\r'), string.end());
}