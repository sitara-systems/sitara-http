#include "HttpClient.h"

using namespace sitara::http;

std::shared_ptr<HttpClient> HttpClient::make() {
	std::shared_ptr<HttpClient> client(new HttpClient());
	return client;
}

HttpClient::HttpClient() {
	mMaxNumberOfThreads = 8;
	mCurrentNumberOfThreads = 0;
	curl_global_init(CURL_GLOBAL_ALL);
	mMultiCurl = curl_multi_init();
	CURLMcode multiCode;
	multiCode = curl_multi_setopt(mMultiCurl, CURLMOPT_MAXCONNECTS, mMaxNumberOfThreads);
	checkForMultiErrors(multiCode);
	mUserAgent = "Mozilla/4.0 (compatible; MSIE 5.01; Windows NT 5.0)";
	mRunUpdateThread = true;
	mUpdateThread = std::thread(&HttpClient::updateThreads, this);
	mFile = NULL;
}

HttpClient::~HttpClient() {
	destroyThreads();
	curl_multi_cleanup(mMultiCurl);
}

HttpResponse HttpClient::makeRequest(const HttpRequest &request) {
	/*
	Makes a blocking request.  This function will cause your program to wait for the request to be completed and then return a string.
	*/
	CURL* c = curl_easy_init();
	setOptions(c, request);

	CURLcode curlCode = curl_easy_perform(c);
	checkForErrors(curlCode);

	HttpResponse response;

	// corresponding request
	response.mRequest = request;

	// response code
	curlCode = curl_easy_getinfo(c, CURLINFO_RESPONSE_CODE, &response.mResponseCode);
	checkForErrors(curlCode);

	// response headers
	std::vector<std::string> headerLines = splitString(mResponseHeaderMap[c], '\n');
	for (auto &line : headerLines) {
		std::vector<std::string> pair = splitString(line, ':');
		if (pair.size() == 2) {
			cleanupString(pair[0]);
			cleanupString(pair[1]);
			response.mHeaders[pair[0]] = pair[1];
		}
	}

	// response body
	Json::Value body = stringToJson(mResponseBodyMap[c]);
	if (body != NULL) {
		response.mBody = body;
	}
	else {
		Json::Value textBody;
		textBody["response_body"] = mResponseBodyMap[c];
		response.mBody = textBody;
	}

	// callback attached to request
	if(request.mCallback) {
		request.mCallback(&response, this);
	}
	else {
		//std::printf("sitara-http::HttpClient WARNING: no callback provided, continuing\n");
	}

	cleanupRequest(c);

	curl_easy_cleanup(c);

	return response;
}


void HttpClient::addRequest(const HttpRequest &request) {
	/*
	Adds a non-blocking request to the request queue.  Requests will be made when a spot in the queue is available, and then the callback attached to the request object will be called.
	*/
	mUpdateMutex.lock();
	mRequestQueue.push(request);
	mUpdateMutex.unlock();
}

size_t HttpClient::getNumberOfRequests() {
	return mRequestQueue.size();
}

void HttpClient::setMaxNumberOfThreads(int numThreads) {
	mMaxNumberOfThreads = numThreads;
	CURLMcode multiCode;
	multiCode = curl_multi_setopt(mMultiCurl, CURLMOPT_MAXCONNECTS, mMaxNumberOfThreads);
	checkForMultiErrors(multiCode);
}

int HttpClient::getMaxNumberOfThreads() {
	return mMaxNumberOfThreads;
}

void HttpClient::setUserAgent(const std::string &agent) {
	mUserAgent = agent;
}

std::string HttpClient::getUserAgent() {
	return mUserAgent;
}

std::string HttpClient::mapToString(const std::map<std::string, std::string> &map) {
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

void HttpClient::makeStringSafe(std::string &input) {
	input = curl_easy_escape(mMultiCurl, input.c_str(), 0);
}

std::string HttpClient::methodToString(const HttpMethod &method) {
	switch (method) {
	case HttpMethod::HTTP_DELETE:
		return "DELETE";
	case HttpMethod::HTTP_PUT:
		return "PUT";
	case HttpMethod::HTTP_POST:
		return "POST";
	case HttpMethod::HTTP_PATCH:
		return "PATCH";
	case HttpMethod::HTTP_GET:
		return "GET";
	case HttpMethod::HTTP_HEADERS:
		return "HEADERS";
	default:
		return NULL;
	}
}

std::string HttpClient::jsonToString(const Json::Value &value) {
	std::string output = mJsonWriter.write(value);
	return output;
}

Json::Value HttpClient::stringToJson(const std::string &string) {
	Json::Value output;
	bool parsingSuccessful = mJsonReader.parse(string, output);
	if (!parsingSuccessful) {
		std::printf("sitara::http::HttpClient ERROR: Failed to parse JSON %s\n", mJsonReader.getFormattedErrorMessages().c_str());
		return NULL;
	}
	else {
		return output;
	}
}

std::string HttpClient::checkHttpStatus(int responseCode) {
	switch (responseCode) {
	case 100:
		return "HTTP/1.1 100 Continue";
		break;
	case 101:
		return "HTTP/1.1 101 Switching Protocols";
		break;
	case 200:
		return "HTTP/1.1 200 OK";
		break;
	case 201:
		return "HTTP/1.1 201 Created";
		break;
	case 202:
		return "HTTP/1.1 202 Accepted";
		break;
	case 203:
		return "HTTP/1.1 203 Non-Authoritative Information";
		break;
	case 204:
		return "HTTP/1.1 204 No Content";
		break;
	case 205:
		return "HTTP/1.1 205 Reset Content";
		break;
	case 206:
		return "HTTP/1.1 206 Partial Content";
		break;
	case 300:
		return "HTTP/1.1 300 Multiple Choices";
		break;
	case 301:
		return "HTTP/1.1 301 Moved Permanently";
		break;
	case 302:
		return "HTTP/1.1 302 Found";
		break;
	case 303:
		return "HTTP/1.1 303 See Other";
		break;
	case 304:
		return "HTTP/1.1 304 Not Modified";
		break;
	case 305:
		return "HTTP/1.1 305 Use Proxy";
		break;
	case 307:
		return "HTTP/1.1 307 Temporary Redirect";
		break;
	case 400:
		return "HTTP/1.1 400 Bad Request";
		break;
	case 401:
		return "HTTP/1.1 401 Unauthorized";
		break;
	case 402:
		return "HTTP/1.1 402 Payment Required";
		break;
	case 403:
		return "HTTP/1.1 403 Forbidden";
		break;
	case 404:
		return "HTTP/1.1 404 Not Found";
		break;
	case 405:
		return "HTTP/1.1 405 Method Not Allowed";
		break;
	case 406:
		return "HTTP/1.1 406 Not Acceptable";
		break;
	case 407:
		return "HTTP/1.1 407 Proxy Authenication Required";
		break;
	case 408:
		return "HTTP/1.1 408 Request Timeout";
		break;
	case 409:
		return "HTTP/1.1 409 Conflict";
		break;
	case 410:
		return "HTTP/1.1 410 Gone";
		break;
	case 411:
		return "HTTP/1.1 411 Length Required";
		break;
	case 412:
		return "HTTP/1.1 412 Precondition Failed";
		break;
	case 413:
		return "HTTP/1.1 413 Payload Too Large";
		break;
	case 414:
		return "HTTP/1.1 414 URI Too Long";
		break;
	case 415:
		return "HTTP/1.1 415 Unsupported Media Type";
		break;
	case 416:
		return "HTTP/1.1 416 Rrange Not Satisfiable";
		break;
	case 417:
		return "HTTP/1.1 417 Expectation Failed";
		break;
	case 418:
		return "HTTP/1.1 418 I'm a teapot";
		break;
	case 426:
		return "HTTP/1.1 426 Upgrade Required";
		break;
	case 500:
		return "HTTP/1.1 500 Internal Server Error";
		break;
	case 501:
		return "HTTP/1.1 501 Not Implemented";
		break;
	case 502:
		return "HTTP/1.1 502 Bad Gateway";
		break;
	case 503:
		return "HTTP/1.1 503 Service Unavailable";
		break;
	case 504:
		return "HTTP/1.1 504 Gate Timeout";
		break;
	case 505:
		return "HTTP/1.1 505 HTTP Version Not Supported";
		break;
	}
}

void HttpClient::updateThreads() {
	while (mRunUpdateThread) {
		CURLMcode multiCode;
		mUpdateMutex.lock();
		while (mCurrentNumberOfThreads < mMaxNumberOfThreads && mRequestQueue.size() > 0) {
			loadRequest();
			//std::printf("sitara-http::HttpClient Adding request; current requests are at %d / %d\n", mCurrentNumberOfThreads, mMaxNumberOfThreads);
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

				HttpResponse response;

				// corresponding request
				response.mRequest = mHandleMap[curlInstance];

				// response code
				curlCode = curl_easy_getinfo(curlInstance, CURLINFO_RESPONSE_CODE, &response.mResponseCode);
				checkForErrors(curlCode);

				// response headers
				std::vector<std::string> headerLines = splitString(mResponseHeaderMap[curlInstance], '\n');
				for (auto &line : headerLines) {
					std::vector<std::string> pair = splitString(line, ':');
					if (pair.size() == 2) {
						cleanupString(pair[0]);
						cleanupString(pair[1]);
						response.mHeaders[pair[0]] = pair[1];
					}
				}

				if (!(((response.mResponseCode / 100) % 10) == 2)) {
					// HTTP 1.1/2xx is a successful response code; if the code is not 2xx, then the call was not successful.
					response.mBody = NULL;

					cleanupRequest(curlInstance);
					curl_multi_remove_handle(mMultiCurl, curlInstance);
					curl_easy_cleanup(curlInstance);

					std::printf("HttpClient ERROR -- response not successful (code %d), not returning valid JSON.\n", response.mResponseCode);
					std::printf("HttpClient ERROR -- %s", checkHttpStatus(response.mResponseCode).c_str());

					break;
				}

				// response body
				Json::Value body = stringToJson(mResponseBodyMap[curlInstance]);
				if (body != NULL) {
					response.mBody = body;
				}
				else {
					Json::Value textBody;
					textBody["response_body"] = mResponseBodyMap[curlInstance];
					response.mBody = textBody;
				}

				// callback attached to request
				if (mHandleMap[curlInstance].mCallback) {
					mHandleMap[curlInstance].mCallback(&response, this);
				}

				cleanupRequest(curlInstance);

				curl_multi_remove_handle(mMultiCurl, curlInstance);
				curl_easy_cleanup(curlInstance);
			}
			else {
				std::printf("sitara-http::HttpClient ERROR: After curl_multi_info_read(), CURLMsg=%d\n", message->msg);
			}
		}

	}
}

void HttpClient::destroyThreads() {
	mRunUpdateThread = false;
	if (mUpdateThread.joinable()) {
		mUpdateThread.join();
	}
}

void HttpClient::loadRequest() {
	HttpRequest request = mRequestQueue.front();

	//makeStringSafe(request.mRequestBody);

	CURL* c = curl_easy_init();
	setOptions(c, request);

	mHandleMap[c] = request;

	CURLMcode multiCode;
	multiCode = curl_multi_add_handle(mMultiCurl, c);
	checkForMultiErrors(multiCode);

	mRequestQueue.pop();
	mCurrentNumberOfThreads++;
}

void HttpClient::setOptions(CURL* curl, const HttpRequest &request) {
	if (!mMultiCurl) {
		std::printf("sitara-http::HttpClient ERROR: MultiCurl hasn't been instantiated!");
	}

	CURLcode curlCode;

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Content-Type: application/json; charset=UTF-8");
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
	//curlCode = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0); // this line makes it work under Https
	//checkForErrors(curlCode);


	if (request.mTarget == MEMORY) {
		curlCode = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &mResponseBodyMap[curl]);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeToMemory);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_HEADERDATA, &mResponseHeaderMap[curl]);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, &writeToHeaders);
		checkForErrors(curlCode);
	}
	else if (request.mTarget == FILE) {
		// write headers to file also
		curlCode = curl_easy_setopt(curl, CURLOPT_HEADER, 1);
		checkForErrors(curlCode);

		fopen_s(&mFile, request.mFilename.c_str(), "w");
		if (mFile == NULL) {
			std::printf("sitara-http::HttpClient ERROR: Cannot open file %s\n", request.mFilename.c_str());
		}
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, mFile);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &writeToFile);
	}

	switch (request.mMethod) {
	case HttpMethod::HTTP_POST:
		curlCode = curl_easy_setopt(curl, CURLOPT_POST, 1);
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
		checkForErrors(curlCode);
		// NOTE -- using COPYPOSTFIELDS because the multicurl handle is running on a separate thread and we need to guarantee the data still exists.
		curlCode = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, request.mRequestBody.c_str());
		checkForErrors(curlCode);
		break;
	case HttpMethod::HTTP_GET:
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		if (!request.mRequestBody.empty()) {
			curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
			checkForErrors(curlCode);
			curlCode = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, request.mRequestBody.c_str());
			checkForErrors(curlCode);
		}
		break;
	case HttpMethod::HTTP_PUT:
		curlCode = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, request.mRequestBody.c_str());
		checkForErrors(curlCode);
		break;
	case HttpMethod::HTTP_DELETE:
		curlCode = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, request.mRequestBody.c_str());
		checkForErrors(curlCode);
		break;
	case HttpMethod::HTTP_HEADERS:
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		break;
	case HttpMethod::HTTP_PATCH:
		curlCode = curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PATCH");
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_URL, request.mUrl.c_str());
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(request.mRequestBody.c_str()));
		checkForErrors(curlCode);
		curlCode = curl_easy_setopt(curl, CURLOPT_COPYPOSTFIELDS, request.mRequestBody.c_str());
		checkForErrors(curlCode);
		break;
	default:
		std::printf("sitara-http::HttpClient Http Method not implemented.\n");
		break;
	}
}

void HttpClient::checkForErrors(const CURLcode error_code) {
	std::string errorString = curl_easy_strerror(error_code);
	if (error_code != CURLE_OK) {
		std::printf("sitara-http::HttpClient ERROR: cURL failed: %s\n", errorString.c_str());
	}
}

void HttpClient::checkForMultiErrors(const CURLMcode error_code) {
	std::string errorString = curl_multi_strerror(error_code);
	if (error_code != CURLM_OK) {
		std::printf("sitara-http::HttpClient ERROR: multi-cURL failed: %s\n", errorString.c_str());
	}
}

void HttpClient::cleanupRequest(CURL* curl) {
	// clean up request map, response map, header map
	HttpRequest request = mHandleMap[curl];

	if (request.mTarget == DataTarget::FILE) {
		std::fclose(mFile);
	}
	else {
		mResponseBodyMap.erase(curl);
	}

	mResponseHeaderMap.erase(curl);
	mHandleMap.erase(curl);
}


size_t HttpClient::writeToMemory(const char* contents, size_t size, size_t nmemb, std::string* buffer) {
	size_t realsize = size * nmemb;
	buffer->append(contents, realsize);
	return realsize;
}

size_t HttpClient::writeToFile(const char* contents, size_t size, size_t nmemb, std::FILE* stream) {
	return std::fwrite(contents, size, nmemb, stream);
}

size_t HttpClient::writeToHeaders(const char *contents, size_t size, size_t nmemb, std::string* data) {
	size_t realsize = size * nmemb;
	data->append(contents, realsize);
	return realsize;
}

void HttpClient::splitString(const std::string &string, char delimiter, std::vector<std::string> &output) {
	std::stringstream ss;
	ss.str(string);
	std::string item;
	while (std::getline(ss, item, delimiter)) {
		output.push_back(item);
	}
}

std::vector<std::string> HttpClient::splitString(const std::string &string, char delimiter) {
	std::vector<std::string> output;
	splitString(string, delimiter, output);
	return output;
}

void HttpClient::cleanupString(std::string &string) {
	while (!string.empty() && std::isspace(*string.begin()))
		string.erase(string.begin());

	while (!string.empty() && std::isspace(*string.rbegin()))
		string.erase(string.length() - 1);

	string.erase(std::remove(string.begin(), string.end(), '\n'), string.end());
	string.erase(std::remove(string.begin(), string.end(), '\r'), string.end());
}