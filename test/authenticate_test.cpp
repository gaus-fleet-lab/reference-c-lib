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

class GausAuthenticate : public ::testing::Test {
protected:
  virtual void SetUp() {
    setupMocks();
    resetCurlMockHistory();
    //Setup a default fake response that will work for all tests.
    free(fakeResponse);
    fakeResponse = strdup("{"
                          "\"deviceGUID\": \"FAKEDEVICEGUID\","
                          "\"productGUID\":\"FAKEPRODUCTGUID\","
                          "\"token\": \"FAKETOKEN\""
                          "}");
  }

  virtual void TearDown() {
    gaus_global_cleanup();
    cleanupMocks();
  }
};

TEST_F(GausAuthenticate, fails_without_initialize) {
  gaus_session_t session;

  gaus_error_t *status = gaus_authenticate("fakeDeviceAccess", "fakeDeviceSecret", &session);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_NO_INIT_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausAuthenticate, fails_without_device_access) {
  gaus_session_t session;
  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_authenticate(NULL, "fakeDeviceSecret", &session);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausAuthenticate, fails_without_device_secret) {
  gaus_session_t session;
  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_authenticate("fakeDeviceAccess", NULL, &session);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausAuthenticate, fails_without_session) {
  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_authenticate("fakeDeviceAccess", "fakeDeviceSecret", NULL);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausAuthenticate, posts_to_correct_address) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_authenticate("fakeDeviceAccess", "fakeDeviceSecret", &session);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_EQ(curlPerformData[0].CURLOPT_URL, serverUrl.append("/authenticate").c_str());

  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
}

TEST_F(GausAuthenticate, posts_correct_json) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"deviceAuthParameters\":{"));
  EXPECT_NE(std::string::npos,
            curlPerformData[0].CURLOPT_POSTFIELDS.find("\"accessKey\":\"" + fakeDeviceAccess + "\""));
  EXPECT_NE(std::string::npos,
            curlPerformData[0].CURLOPT_POSTFIELDS.find("\"secretKey\":\"" + fakeDeviceSecret + "\""));

  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
}


static CURLcode mock_curl_easy_perform_failed(CURL *curl) {
  return CURLE_HTTP_RETURNED_ERROR;
}

TEST_F(GausAuthenticate, handles_errors_on_perform_without_status) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_perform = mock_curl_easy_perform_failed;

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

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

TEST_F(GausAuthenticate, handles_errors_on_perform_with_status_500) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_getinfo = mock_curl_easy_getinfo_return_500;

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

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

TEST_F(GausAuthenticate, handles_errors_on_perform_with_status_400) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_getinfo = mock_curl_easy_getinfo_return_400;

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_HTTP_ERROR, status->error_type);
  EXPECT_EQ(400, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausAuthenticate, retreives_correctly_from_server) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(0, strcmp("FAKEDEVICEGUID", session.device_guid));
  EXPECT_EQ(0, strcmp("FAKEPRODUCTGUID", session.product_guid));
  EXPECT_EQ(0, strcmp("FAKETOKEN", session.token));

  //Cleanup after test
  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
  free(status);
}

TEST_F(GausAuthenticate, handles_no_device_guid_from_server) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_global_init(serverUrl.c_str(), NULL);

  free(fakeResponse);
  fakeResponse = strdup("{"
                        "\"productGUID\":\"FAKEPRODUCTGUID\","
                        "\"token\": \"FAKETOKEN\""
                        "}");

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
  free(status->description);
  free(status);
}

TEST_F(GausAuthenticate, handles_no_product_guid_from_server) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_global_init(serverUrl.c_str(), NULL);

  free(fakeResponse);
  fakeResponse = strdup("{"
                        "\"deviceGUID\": \"FAKEDEVICEGUID\","
                        "\"token\": \"FAKETOKEN\""
                        "}");

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
  free(status->description);
  free(status);
}

TEST_F(GausAuthenticate, handles_no_token_from_server) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_global_init(serverUrl.c_str(), NULL);

  free(fakeResponse);
  fakeResponse = strdup("{"
                        "\"deviceGUID\": \"FAKEDEVICEGUID\","
                        "\"productGUID\":\"FAKEPRODUCTGUID\","
                        "}");

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
  free(status->description);
  free(status);
}

TEST_F(GausAuthenticate, handles_malformed_json_from_server) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_global_init(serverUrl.c_str(), NULL);

  free(fakeResponse);
  fakeResponse = strdup("Totally not json!");

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
  free(status->description);
  free(status);
}

TEST_F(GausAuthenticate, uses_proxy_from_init) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  std::string fakeProxy = "fakeProxy";
  gaus_initialization_options_t options = {
      fakeProxy.c_str()
  };

  gaus_global_init(serverUrl.c_str(), &options);

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlPerformData.size());
  EXPECT_EQ(fakeProxy, curlPerformData[0].CURLOPT_PROXY);

  //Cleanup after test
  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
  free(status);
}

TEST_F(GausAuthenticate, does_not_use_proxy_if_null) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;
  std::string fakeDeviceSecret = "fakeDeviceSecret";
  std::string fakeDeviceAccess = "fakeDeviceAccess";

  gaus_initialization_options_t options = {
      NULL
  };

  gaus_global_init(serverUrl.c_str(), &options);

  gaus_error_t *status = gaus_authenticate(fakeDeviceAccess.c_str(), fakeDeviceSecret.c_str(), &session);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlPerformData.size());
  EXPECT_EQ(MOCK_NOT_SET, curlPerformData[0].CURLOPT_PROXY);

  //Cleanup after test
  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
  free(status);
}


TEST_F(GausAuthenticate, has_correct_version_in_header) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t session;

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_authenticate("fakeDeviceAccess", "fakeDeviceSecret", &session);

  gaus_version_t version = gaus_client_library_version();
  std::ostringstream os;
  os << "v" << version.major << "." << version.minor << "." << version.patch;
  std::string versionString = os.str();

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  auto curlopt_headers = curlPerformData[0].CURLOPT_HEADER;
  EXPECT_GE(curlopt_headers.size(), 1); //At least one header
  EXPECT_NE(std::end(curlopt_headers),
            std::find_if(std::begin(curlopt_headers), std::end(curlopt_headers),
                         [&versionString](const std::string a) {
                           return a.find(versionString) != std::string::npos;
                         }

            ));

  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
}

//Test against a real backend
//#define TEST_GAUS_REAL
#ifdef TEST_GAUS_REAL

TEST_F(GausAuthenticate, try_for_real) {
  cleanupMocks();

  std::string serverUrl = "https://gaus-test.dev.gaus.sonymobile.com";
  std::string realProdAccess = "541cd14b-7a66-4aba-9095-91ae8711425d";
  std::string realProdSecret = "9421b7bd217740e4dcd4067d18a656847fe63c50269d09491979e8f98eea7f66";
  std::string deviceId = "fakeDeviceId";

  char *device_access;
  char *device_secret;
  unsigned int poll_interval;

  std::string proxy = "http://proxy.global.sonyericsson.net:8080";
  gaus_initialization_options_t options = {
      proxy.c_str()
  };
  gaus_global_init(serverUrl.c_str(), &options);


  gaus_error_t *status = gaus_register(realProdAccess.c_str(), realProdSecret.c_str(), deviceId.c_str(),
                                       &device_access, &device_secret, &poll_interval);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);


  gaus_session_t session;

  status = gaus_authenticate(device_access, device_secret, &session);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);


  //Cleanup after test
  free(device_access);
  free(device_secret);
  free(session.device_guid);
  free(session.product_guid);
  free(session.token);
  free(status);

  //Put mock back for teardown
  setupMocks();
}

#endif
