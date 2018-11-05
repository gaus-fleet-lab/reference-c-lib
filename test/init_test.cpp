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

TEST_F(GausInit, global_init_handles_proxy_in_options) {
  gaus_initialization_options_t options = {
      "fakeproxy"
  };
  gaus_error_t *status = gaus_global_init("fakeServerUrl", &options);
  ASSERT_EQ(NULL, status);

  free(status);
}
