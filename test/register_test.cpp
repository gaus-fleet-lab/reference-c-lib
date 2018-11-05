//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include <gtest/gtest.h>
#include "gaus/gaus_client.h"
#include "curl_mock.h"

//Access gaus curl wrapper
#include "../src/libgaus/curl_wrapper.h"

#include <cstdarg>

#include <map>
#include <iostream>

class GausRegister : public ::testing::Test {
protected:
  virtual void SetUp() {
    setupMocks();
    resetCurlMockHistory();
    //Setup a default fake response that will work for all tests.
    free(fakeResponse);
    fakeResponse = strdup("{"
                          "\"pollIntervalSeconds\": 12345,"
                          "\"deviceAuthParameters\":{"
                          "\"accessKey\":\"FAKEACCESSKEY\","
                          "\"secretKey\": \"FAKESECRETKEY\""
                          "}"
                          "}");
  }

  virtual void TearDown() {
    gaus_global_cleanup();
    cleanupMocks();
  }
};


TEST_F(GausRegister, fails_without_initialize) {
  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register("fakeProductAccess", "fakeProductSecret", "fakeDeviceId",
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_NO_INIT_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, fails_without_product_secret) {
  std::string serverUrl = "fakeServerUrl";
  gaus_global_init(serverUrl.c_str(), NULL);

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register("fakeProductAccess", NULL, "fakeDeviceId",
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, fails_without_product_access) {
  std::string serverUrl = "fakeServerUrl";
  gaus_global_init(serverUrl.c_str(), NULL);

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(NULL, "fakeProdSecret", "fakeDeviceId",
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, fails_without_product_id) {
  std::string serverUrl = "fakeServerUrl";
  gaus_global_init(serverUrl.c_str(), NULL);

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register("fakeProdAccess", "fakeProdSecret", NULL,
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, fails_without_device_access) {
  std::string serverUrl = "fakeServerUrl";
  gaus_global_init(serverUrl.c_str(), NULL);

  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register("fakeProdAccess", "fakeProdSecret", "fakeDeviceId",
                                       NULL, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}


TEST_F(GausRegister, fails_without_device_secret) {
  std::string serverUrl = "fakeServerUrl";
  gaus_global_init(serverUrl.c_str(), NULL);

  char *device_access;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register("fakeProdAccess", "fakeProdSecret", "fakeDeviceId",
                                       &device_access, NULL, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, fails_without_poll_interval) {
  std::string serverUrl = "fakeServerUrl";
  gaus_global_init(serverUrl.c_str(), NULL);

  char *device_access;
  char *device_secret;
  gaus_error_t *status = gaus_register("fakeProdAccess", "fakeProdSecret", "fakeDeviceId",
                                       &device_access, &device_secret, NULL);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, posts_to_correct_address) {
  std::string serverUrl = "fakeServerUrl";
  gaus_global_init(serverUrl.c_str(), NULL);

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register("fakeProductAccess", "fakeProductSecret", "fakeDeviceId",
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_EQ(curlPerformData[0].CURLOPT_URL, serverUrl.append("/register").c_str());

  //Cleanup
  free(device_access);
  free(device_secret);
}

TEST_F(GausRegister, posts_correct_json) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";
  gaus_global_init(serverUrl.c_str(), NULL);

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"productAuthParameters\":{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"deviceId\":\"" + fakeDeviceId + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"accessKey\":\"" + fakeProdAccess + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"secretKey\":\"" + fakeProdSecret + "\""));

  //Cleanup
  free(device_access);
  free(device_secret);
}

static CURLcode mock_curl_easy_perform_failed(CURL *curl) {
  //Always return ok
  return CURLE_HTTP_RETURNED_ERROR;
}


TEST_F(GausRegister, handles_errors_on_perform_without_status) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_perform = mock_curl_easy_perform_failed;

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

static CURLcode mock_curl_easy_getinfo_return_500(CURL *curl, CURLINFO info, ...) {
  long *code;
  va_list valist;
  va_start(valist, info);
  switch (info) {
    case CURLINFO_RESPONSE_CODE:
      //Always return 200 for last status
      code = va_arg(valist, long*);
      *code = 500;
      break;
    default:
      //doNothing unless this is a param we need to handle
      break;
  }
  //Always return ok:
  return CURLE_OK;
}

TEST_F(GausRegister, handles_errors_on_perform_with_status_500) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_getinfo = mock_curl_easy_getinfo_return_500;

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_HTTP_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}


static CURLcode mock_curl_easy_getinfo_return_400(CURL *curl, CURLINFO info, ...) {
  long *code;
  va_list valist;
  va_start(valist, info);
  switch (info) {
    case CURLINFO_RESPONSE_CODE:
      //Always return 200 for last status
      code = va_arg(valist, long*);
      *code = 400;
      break;
    default:
      //doNothing unless this is a param we need to handle
      break;
  }
  //Always return ok:
  return CURLE_OK;
}

TEST_F(GausRegister, handles_errors_on_perform_with_status_400) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_getinfo = mock_curl_easy_getinfo_return_400;

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_HTTP_ERROR, status->error_type);
  EXPECT_EQ(400, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, retreives_correctly_from_server) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";
  gaus_global_init(serverUrl.c_str(), NULL);

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(0, strcmp("FAKEACCESSKEY", device_access));
  EXPECT_EQ(0, strcmp("FAKESECRETKEY", device_secret));
  EXPECT_EQ(12345, poll_interval);

  //Cleanup after test
  free(device_access);
  free(device_secret);
  free(status);
}

TEST_F(GausRegister, handles_no_pollIntervalSeconds_from_server) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";
  gaus_global_init(serverUrl.c_str(), NULL);

  free(fakeResponse);
  fakeResponse = strdup("{"
                        "\"deviceAuthParameters\":{"
                        "\"accessKey\":\"FAKEACCESSKEY\","
                        "\"secretKey\": \"FAKESECRETKEY\""
                        "}"
                        "}");

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, handles_no_accessKey_from_server) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";
  gaus_global_init(serverUrl.c_str(), NULL);

  free(fakeResponse);
  fakeResponse = strdup("{"
                        "\"pollIntervalSeconds\": 12345,"
                        "\"deviceAuthParameters\":{"
                        "\"secretKey\": \"FAKESECRETKEY\""
                        "}"
                        "}");

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, handles_no_secretKey_from_server) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";
  gaus_global_init(serverUrl.c_str(), NULL);

  free(fakeResponse);
  fakeResponse = strdup("{"
                        "\"pollIntervalSeconds\": 12345,"
                        "\"deviceAuthParameters\":{"
                        "\"accessKey\":\"FAKEACCESSKEY\","
                        "}"
                        "}");

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, handles_no_malformed_json_from_server) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";
  gaus_global_init(serverUrl.c_str(), NULL);

  free(fakeResponse);
  fakeResponse = strdup("Totally not json!");

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausRegister, uses_proxy_from_init) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";

  std::string fakeProxy = "fakeProxy";
  gaus_initialization_options_t options = {
      fakeProxy.c_str()
  };

  gaus_global_init(serverUrl.c_str(), &options);

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlPerformData.size());
  EXPECT_EQ(fakeProxy, curlPerformData[0].CURLOPT_PROXY);

  //Cleanup after test
  free(device_access);
  free(device_secret);
  free(status);
}

TEST_F(GausRegister, does_not_use_proxy_if_null) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeProdSecret = "fakeSecret";
  std::string fakeProdAccess = "fakeProductAccess";
  std::string fakeDeviceId = "fakeDeviceId";

  gaus_initialization_options_t options = {
      NULL
  };

  gaus_global_init(serverUrl.c_str(), &options);

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;
  gaus_error_t *status = gaus_register(fakeProdAccess.c_str(), fakeProdSecret.c_str(), fakeDeviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlPerformData.size());
  EXPECT_EQ(MOCK_NOT_SET, curlPerformData[0].CURLOPT_PROXY);

  //Cleanup after test
  free(device_access);
  free(device_secret);
  free(status);
}
