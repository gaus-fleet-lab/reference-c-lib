#ifndef GAUS_CURL_MOCK_H
#define GAUS_CURL_MOCK_H

#include <cstdarg>
#include <map>
#include <iostream>
#include <vector>

//Get access to ability to mock curl inside libgaus
#include "../src/libgaus/curl_wrapper.h"

//Allow backing up original functions to so we can restore them
extern bool mocks_setup;
extern curl_global_init_t *original_curl_global_init;
extern curl_easy_init_t *original_curl_easy_init;
extern curl_easy_perform_t *original_curl_easy_perform;
extern curl_easy_setopt_t *original_curl_easy_setopt;
extern curl_easy_cleanup_t *original_curl_easy_cleanup;
extern curl_global_cleanup_t *original_curl_global_cleanup;
extern curl_easy_getinfo_t *original_curl_easy_getinfo;

//Data structures for mocks:
typedef void (*write_function_t)(char *ptr, size_t size, size_t nmemb, void *userdata);

#define MOCK_NOT_SET "NOT_SET"
#define MOCK_NOT_SET_LONG -1L

class CurlOptionsData {
public:
  std::string CURLOPT_URL = MOCK_NOT_SET; //If this is set multiple times we overwrite old value
  std::string CURLOPT_POSTFIELDS = MOCK_NOT_SET; //If this is set multiple times we overwrite old value
  void *CURLOPT_WRITEDATA = {nullptr}; //If this is set multiple times we overwrite old value
  write_function_t CURLOPT_WRITEFUNCTION;
  std::string CURLOPT_PROXY = MOCK_NOT_SET; //If this is set multiple times we overwrite old value
  long CURLOPT_HTTPGET = MOCK_NOT_SET_LONG;
  std::vector<std::string> CURLOPT_HEADER;
};

class CurlCallCounter {
public:
  int globalInit = {0};
  int globalCleanup = {0};

  void reset(void);
};

class CurlMockData {
public:
  CurlOptionsData setOptions;
};

//Hold results of curl operations
extern std::map<CURL *, CurlMockData> allCurlData;
extern std::vector<CurlOptionsData> curlPerformData;
extern CurlCallCounter curlCallCounter;

//Used to send a response to the CURLOPT_WRITE_FUNCTION
extern char *fakeResponse;

//Mock functions
CURLcode mock_curl_global_init(long flags);

CURL *mock_curl_easy_init(void);

CURLcode mock_curl_easy_perform(CURL *curl);

CURLcode mock_curl_easy_setopt(CURL *curl, CURLoption option, ...);

void mock_curl_easy_cleanup(CURL *curl);

void mock_curl_global_cleanup(void);

CURLcode mock_curl_easy_getinfo(CURL *curl, CURLINFO info, ...);

//Setup/Teardown:
void setupMocks();

void cleanupMocks();

//Mock interaction functions
void resetCurlMockHistory();

#endif //GAUS_CURL_MOCK_H
