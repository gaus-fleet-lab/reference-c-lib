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

class GausInit : public ::testing::Test {
protected:
  virtual void SetUp() {
    setupMocks();
    resetCurlMockHistory();
  }

  virtual void TearDown() {
    gaus_global_cleanup();
    cleanupMocks();
  }
};


TEST_F(GausInit, initialize) {
  EXPECT_EQ(static_cast<gaus_error_t *>(NULL), gaus_global_init("fakeServerUrl", NULL));
  EXPECT_EQ(1, curlCallCounter.globalInit);
}

TEST_F(GausInit, multiple_global_initialize) {
  EXPECT_EQ(static_cast<gaus_error_t *>(NULL), gaus_global_init("fakeServerUrl", NULL));
  EXPECT_EQ(static_cast<gaus_error_t *>(NULL), gaus_global_init("fakeServerUrl", NULL));
  EXPECT_EQ(1, curlCallCounter.globalInit);
}

TEST_F(GausInit, multiple_global_cleanup) {
  EXPECT_EQ(static_cast<gaus_error_t *>(NULL), gaus_global_init("fakeServerUrl", NULL));
  gaus_global_cleanup();
  gaus_global_cleanup();
  EXPECT_EQ(1, curlCallCounter.globalCleanup);
}

CURLcode mock_failing_curl_global_init(long flags) {
  curlCallCounter.globalInit++;
  return CURLE_FAILED_INIT;
}

TEST_F(GausInit, failed_global_curl_init) {
  gaus_curl_global_init = mock_failing_curl_global_init;
  gaus_error_t *status = gaus_global_init("fakeServerUrl", NULL);
  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlCallCounter.globalInit);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  free(status->description);
  free(status);
}

TEST_F(GausInit, global_init_handles_null_proxy_in_options) {
  gaus_initialization_options_t options = {
      NULL
  };
  gaus_error_t *status = gaus_global_init("fakeServerUrl", &options);
  ASSERT_EQ(NULL, status);

  free(status);
}

TEST_F(GausInit, global_init_handles_null_ca_path_in_options) {
  gaus_initialization_options_t options = {
      .proxy = NULL,
      .ca_path= NULL
  };
  gaus_error_t *status = gaus_global_init("fakeServerUrl", &options);
  ASSERT_EQ(NULL, status);

  free(status);
}

TEST_F(GausInit, global_init_handles_proxy_in_options) {
  gaus_initialization_options_t options = {
      "fakeproxy"
  };
  gaus_error_t *status = gaus_global_init("fakeServerUrl", &options);
  ASSERT_EQ(NULL, status);

  free(status);
}

TEST_F(GausInit, global_init_handles_ca_path_in_options) {
  gaus_initialization_options_t options = {
      .proxy = NULL,
      .ca_path= "fake-ca-cert-path"
  };
  gaus_error_t *status = gaus_global_init("fakeServerUrl", &options);
  ASSERT_EQ(NULL, status);

  free(status);
}
