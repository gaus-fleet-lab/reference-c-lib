#include "curl_mock.h"
#include <cstring>

//Allow backing up original functions to so we can restore them
bool mocks_setup = false;
curl_global_init_t *original_curl_global_init;
curl_easy_init_t *original_curl_easy_init;
curl_easy_perform_t *original_curl_easy_perform;
curl_easy_setopt_t *original_curl_easy_setopt;
curl_easy_cleanup_t *original_curl_easy_cleanup;
curl_global_cleanup_t *original_curl_global_cleanup;
curl_easy_getinfo_t *original_curl_easy_getinfo;

//Storage for curl mocking
std::map<CURL *, CurlMockData> allCurlData;
std::vector<CurlOptionsData> curlPerformData;
CurlCallCounter curlCallCounter;
char *fakeResponse = strdup("{}");

//** Curl mock functions
CURLcode mock_curl_global_init(long flags) {
  curlCallCounter.globalInit++;
  return CURLE_OK;
}

CURL *mock_curl_easy_init(void) {
  //Allocate a string and use its address to track this curl
  CURL *curl = static_cast<CURL *>(strdup("fakeHandle"));
  CurlMockData emptyData;
  allCurlData.emplace(curl, emptyData);
  return curl;
}

CURLcode mock_curl_easy_perform(CURL *curl) {
  curlPerformData.push_back(allCurlData[curl].setOptions);
  write_function_t writeFunction = allCurlData[curl].setOptions.CURLOPT_WRITEFUNCTION;
  if (writeFunction) {
    void *writeData = allCurlData[curl].setOptions.CURLOPT_WRITEDATA;
    (*writeFunction)(fakeResponse, sizeof(char), strlen(fakeResponse), writeData);
  }
  //Always return ok
  return CURLE_OK;
}

CURLcode mock_curl_easy_setopt(CURL *curl, CURLoption option, ...) {
  va_list valist;
  va_start(valist, option);
  curl_slist *current = NULL;

  switch (option) {
    case CURLOPT_URL:
      allCurlData[curl].setOptions.CURLOPT_URL = va_arg(valist, char*);
      break;
    case CURLOPT_POSTFIELDS:
      allCurlData[curl].setOptions.CURLOPT_POSTFIELDS = va_arg(valist, char*);
      break;
    case CURLOPT_WRITEFUNCTION:
      allCurlData[curl].setOptions.CURLOPT_WRITEFUNCTION = va_arg(valist, write_function_t);
      break;
    case CURLOPT_WRITEDATA:
      allCurlData[curl].setOptions.CURLOPT_WRITEDATA = va_arg(valist, void*);
      break;
    case CURLOPT_PROXY:
      allCurlData[curl].setOptions.CURLOPT_PROXY = va_arg(valist, char*);
      break;
    case CURLOPT_HTTPGET:
      allCurlData[curl].setOptions.CURLOPT_HTTPGET = va_arg(valist, long);
      break;
    case CURLOPT_HTTPHEADER:
      //Loop over options in list and add them to vector for easier testing
      current = va_arg(valist, curl_slist*);
      while (current) {
        std::string temp(current->data);
        allCurlData[curl].setOptions.CURLOPT_HEADER.push_back(temp);
        current = current->next;
      }
      break;
    default:
      //do nothing unless this is a param we want to track later
      break;
  }
  //Always return ok
  return CURLE_OK;
}

void mock_curl_easy_cleanup(CURL *curl) {
  allCurlData.erase(curl);
  free(curl); //Cleanup our fakeHandle string
}

void mock_curl_global_cleanup(void) {
  curlCallCounter.globalCleanup++;
}

CURLcode mock_curl_easy_getinfo(CURL *curl, CURLINFO info, ...) {
  long *code;
  va_list valist;
  va_start(valist, info);
  switch (info) {
    case CURLINFO_RESPONSE_CODE:
      //Always return 200 for last status
      code = va_arg(valist, long*);
      *code = 200;
      break;
    default:
      //doNothing unless this is a param we need to handle
      break;
  }
  //Always return ok:
  return CURLE_OK;
}


//Mock setup/teardown functions:
void setupMocks() {
  if (!mocks_setup) {
    //Backup original functions
    original_curl_global_init = gaus_curl_global_init;
    original_curl_easy_init = gaus_curl_easy_init;
    original_curl_easy_perform = gaus_curl_easy_perform;
    original_curl_easy_setopt = gaus_curl_easy_setopt;
    original_curl_easy_cleanup = gaus_curl_easy_cleanup;
    original_curl_global_cleanup = gaus_curl_global_cleanup;
    original_curl_easy_getinfo = gaus_curl_easy_getinfo;

    //Setup our "mocks"
    gaus_curl_global_init = mock_curl_global_init;
    gaus_curl_easy_init = mock_curl_easy_init;
    gaus_curl_easy_perform = mock_curl_easy_perform;
    gaus_curl_easy_setopt = mock_curl_easy_setopt;
    gaus_curl_easy_cleanup = mock_curl_easy_cleanup;
    gaus_curl_global_cleanup = mock_curl_global_cleanup;
    gaus_curl_easy_getinfo = mock_curl_easy_getinfo;
    mocks_setup = true;
  } else {
    throw "Attempted to setup mocks twice!";
  }
}

void cleanupMocks() {
  if (mocks_setup) {
    //Restore original functions
    gaus_curl_global_init = original_curl_global_init;
    gaus_curl_easy_init = original_curl_easy_init;
    gaus_curl_easy_perform = original_curl_easy_perform;
    gaus_curl_easy_setopt = original_curl_easy_setopt;
    gaus_curl_easy_cleanup = original_curl_easy_cleanup;
    gaus_curl_global_cleanup = original_curl_global_cleanup;
    gaus_curl_easy_getinfo = original_curl_easy_getinfo;
    mocks_setup = false;
  } else {
    throw "Attempting to restore without having mocked!";
  }
}

//Mock control functions:
void resetCurlMockHistory() {
  free(fakeResponse);
  fakeResponse = strdup("{}");
  allCurlData.clear();
  curlPerformData.clear();
  curlCallCounter.reset();
}

void CurlCallCounter::reset(void) {
  globalInit = 0;
  globalCleanup = 0;
}
