#pragma once

#include <string>
#include <cctype>
#include <vector>
#include <queue>
#include <map>
#include <thread>
#include <mutex>
#include <memory>
#include <iostream>
#include "jsoncpp/json.h"
#include "curl/curl.h"

namespace midnight {
	namespace http {

		enum HttpMethod {
			HTTP_NONE,
			HTTP_DELETE,
			HTTP_PUT,
			HTTP_POST,
			HTTP_PATCH,
			HTTP_GET,
			HTTP_HEADERS,
		};

		enum DataTarget {
			MEMORY,
			FILE
		};

		//forward declare
		struct HttpResponse;
		class HttpClient;

		struct HttpRequest {
			HttpRequest() : mMethod(HTTP_NONE), mUrl(""), mRequestBody(""), mTarget(MEMORY), mFilename(""), mCallback(NULL) {};

			HttpMethod mMethod;
			std::string mUrl;
			std::string mRequestBody;
			DataTarget mTarget;
			std::string mFilename;
			std::function<void(HttpResponse*, HttpClient*)> mCallback;
		};

		struct HttpResponse {
			long mResponseCode;
			Json::Value mBody;
			Json::Value mHeaders;
			HttpRequest mRequest;
		};

		class HttpClient {
		public:
			static std::shared_ptr<HttpClient> make();
			~HttpClient();
			HttpResponse makeRequest(const HttpRequest &request);
			void addHttpRequest(const HttpRequest &request);
			size_t getNumberOfRequests();
			void setMaxNumberOfThreads(int numThreads);
			int getMaxNumberOfThreads();
			void setUserAgent(const std::string &agent);
			std::string getUserAgent();
			std::string mapToString(const std::map<std::string, std::string> &map);
			void makeStringSafe(std::string &input);
			std::string methodToString(const HttpMethod &method);
			std::string jsonToString(const Json::Value &value);
			Json::Value stringToJson(const std::string &string);
			void checkHttpStatus(int responseCode);
		protected:
			HttpClient();
			void updateThreads();
			void destroyThreads();
			void loadRequest();
			void setOptions(CURL* curl, const HttpRequest &request);
			void checkForErrors(const CURLcode error_code);
			void checkForMultiErrors(const CURLMcode error_code);
			void cleanupRequest(CURL* curl);
			static size_t writeToMemory(const char* contents, size_t size, size_t nmemb, std::string* buffer);
			static size_t writeToFile(const char* contents, size_t size, size_t nmemb, std::FILE* stream);
			static size_t writeToHeaders(const char *contents, size_t size, size_t nmemb, std::string* data);
			static void splitString(const std::string &string, char delimiter, std::vector<std::string> &output);
			static std::vector<std::string> splitString(const std::string &string, char delimiter);
			static void cleanupString(std::string &string);
			CURL* mMultiCurl;
			Json::Reader mJsonReader;
			Json::FastWriter mJsonWriter;
			int mMaxNumberOfThreads;
			int mCurrentNumberOfThreads;
			std::FILE* mFile;
			bool mRunUpdateThread;
			std::thread mUpdateThread;
			std::mutex mUpdateMutex;
			std::string mUserAgent;
			std::map<CURL*, HttpRequest> mHandleMap;
			std::map<CURL*, std::string> mResponseBodyMap;
			std::map<CURL*, std::string> mResponseHeaderMap;
			std::queue<HttpRequest> mRequestQueue;
			static const int MAX_WAIT_MSECS = 30000;
		};
	}
}