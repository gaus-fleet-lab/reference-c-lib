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

class GausCheckForUpdates : public ::testing::Test {
private:
  std::string fakeNoUpdateResponse = "{\"updates\": []}";
protected:
  virtual void SetUp() {
    setupMocks();
    resetCurlMockHistory();
    //Setup a default fake response that will work for all tests.
    free(fakeResponse);
    fakeResponse = strdup(fakeNoUpdateResponse.c_str());
  }

  virtual

  void TearDown() {
    gaus_global_cleanup();
    cleanupMocks();
  }
};

static void freeUpdates(unsigned int updateCount, gaus_update_t **updates);

static void freeUpdate(gaus_update_t *update);

TEST_F(GausCheckForUpdates, fails_without_initialize) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_NO_INIT_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, fails_without_session) {
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_check_for_updates(NULL, filterCount, NULL, &updateCount, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, fails_without_device_guid) {
  gaus_session_t fakeSession = {
      NULL,
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, fails_without_product_guid) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      NULL,
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, fails_without_token) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      NULL
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, fails_if_filter_count_and_no_filter) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 1;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, fails_if_no_update_count) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, NULL, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, fails_if_no_updates) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;

  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, NULL);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, gets_from_correct_address) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_EQ(curlPerformData[0].CURLOPT_URL,
            (serverUrl + "/device/" + fakeProductGuid + "/" + fakeDeviceGuid + "/check-for-updates").c_str());
  EXPECT_EQ(curlPerformData[0].CURLOPT_HTTPGET, 1L);

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, has_correct_auth_header) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  auto curlopt_headers = curlPerformData[0].CURLOPT_HEADER;
  EXPECT_GE(curlopt_headers.size(), 1); //At least one header
  EXPECT_NE(std::end(curlopt_headers),
            std::find_if(std::begin(curlopt_headers), std::end(curlopt_headers),
                         [&fakeToken](const std::string a) {
                           return a == ("Authorization: Bearer " + fakeToken);
                         }
            ));

  EXPECT_EQ(curlPerformData[0].CURLOPT_URL,
            (serverUrl + "/device/" + fakeProductGuid + "/" + fakeDeviceGuid + "/check-for-updates").c_str());
  EXPECT_EQ(curlPerformData[0].CURLOPT_HTTPGET, 1L);

//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

static CURLcode mock_curl_easy_perform_failed(CURL *curl) {
  return CURLE_HTTP_RETURNED_ERROR;
}

TEST_F(GausCheckForUpdates, handles_errors_on_perform_without_status) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_perform = mock_curl_easy_perform_failed;

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
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

TEST_F(GausCheckForUpdates, handles_errors_on_perform_with_status_500) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_getinfo = mock_curl_easy_getinfo_return_500;

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_HTTP_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
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

TEST_F(GausCheckForUpdates, handles_errors_on_perform_with_status_400) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_getinfo = mock_curl_easy_getinfo_return_400;

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_HTTP_ERROR, status->error_type);
  EXPECT_EQ(400, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status->description);
  free(status);
}


TEST_F(GausCheckForUpdates, retreives_no_updates_correctly_from_server) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  ASSERT_EQ(updateCount, 0);
  EXPECT_EQ(updates, static_cast<gaus_update_t *>(NULL));

//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

static void freeUpdate(gaus_update_t update) {
  for (int i = 0; i < update.metadata_count; i++) {
    free(update.metadata[i].key);
    free(update.metadata[i].value);
  }
  update.metadata_count = 0;
  update.size = 0;
  free(update.metadata);
  free(update.update_type);
  free(update.package_type);
  free(update.md5);
  free(update.update_id);
  free(update.version);
  free(update.download_url);
}

static void freeUpdates(unsigned int updateCount, gaus_update_t **updates) {
  for (int i = 0; i < updateCount; i++) {
    freeUpdate((*updates)[i]);
  }
  free(*updates);
}

TEST_F(GausCheckForUpdates, retreives_one_updates_correctly_from_server) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);


  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  ASSERT_EQ(updateCount, 1);
  ASSERT_NE(updates, static_cast<gaus_update_t *>(NULL));
  EXPECT_EQ(updates[0].metadata_count, 1);
  EXPECT_EQ(updates[0].metadata[0].key, fakeMetaKey);
  EXPECT_EQ(updates[0].metadata[0].value, fakeMetaValue);
  EXPECT_EQ(updates[0].size, fakeSize);
  EXPECT_EQ(updates[0].update_type, fakeUpdateType);
  EXPECT_EQ(updates[0].package_type, fakePackageType);
  EXPECT_EQ(updates[0].md5, fakeMd5);
  EXPECT_EQ(updates[0].update_id, fakeUpdateId);
  EXPECT_EQ(updates[0].version, fakeVersion);
  EXPECT_EQ(updates[0].download_url, fakeDownloadUrl);

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, retreives_two_updates_correctly_from_server) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);


  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";

  unsigned int fakeSize2 = 456;
  std::string fakeUpdateType2 = "firmware2";
  std::string fakePackageType2 = "file";
  std::string fakeMd52 = "FAKEMD52";
  std::string fakeUpdateId2 = "FAKEUPDATEID2";
  std::string fakeVersion2 = "FAKEVERSION2";
  std::string fakeDownloadUrl2 = "FAKEDOWNLOADURL2";
  std::string fakeMetaKey2 = "FAKEMETAKEY2";
  std::string fakeMetaValue2 = "FAKEMETAVALUE2";
  std::string
      fakeTwoUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}," +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey2 + "\":" + "\"" + fakeMetaValue2 + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize2) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType2 + "\"," +
                              "\"packageType\": \"" + fakePackageType2 + "\"," +
                              "\"md5\": \"" + fakeMd52 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId2 + "\"," +
                              "\"version\": \"" + fakeVersion2 + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl2 + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeTwoUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  ASSERT_EQ(updateCount, 2);
  ASSERT_NE(updates, static_cast<gaus_update_t *>(NULL));
  EXPECT_EQ(updates[0].metadata_count, 1);
  EXPECT_EQ(updates[0].metadata[0].key, fakeMetaKey);
  EXPECT_EQ(updates[0].metadata[0].value, fakeMetaValue);
  EXPECT_EQ(updates[0].size, fakeSize);
  EXPECT_EQ(updates[0].update_type, fakeUpdateType);
  EXPECT_EQ(updates[0].md5, fakeMd5);
  EXPECT_EQ(updates[0].update_id, fakeUpdateId);
  EXPECT_EQ(updates[0].version, fakeVersion);
  EXPECT_EQ(updates[0].download_url, fakeDownloadUrl);
  EXPECT_EQ(updates[1].metadata_count, 1);
  EXPECT_EQ(updates[1].metadata[0].key, fakeMetaKey2);
  EXPECT_EQ(updates[1].metadata[0].value, fakeMetaValue2);
  EXPECT_EQ(updates[1].size, fakeSize2);
  EXPECT_EQ(updates[1].update_type, fakeUpdateType2);
  EXPECT_EQ(updates[1].md5, fakeMd52);
  EXPECT_EQ(updates[1].update_id, fakeUpdateId2);
  EXPECT_EQ(updates[1].version, fakeVersion2);
  EXPECT_EQ(updates[1].download_url, fakeDownloadUrl2);
  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}


TEST_F(GausCheckForUpdates, handles_multiple_metadata_correctly) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);


  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey1 = "FAKEMETAKEY";
  std::string fakeMetaValue1 = "FAKEMETAVALUE";
  std::string fakeMetaKey2 = "FAKEMETAVALUE2";
  std::string fakeMetaValue2 = "FAKEMETAVALUE2";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey1 + "\":" + "\"" + fakeMetaValue1 + "\"," +
                              "\"" + fakeMetaKey2 + "\":" + "\"" + fakeMetaValue2 + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  ASSERT_EQ(updateCount, 1);
  ASSERT_NE(updates, static_cast<gaus_update_t *>(NULL));
  EXPECT_EQ(updates[0].metadata_count, 2);
  std::map<std::string, std::string> metaKeys;
  for (int i = 0; i < updates[0].metadata_count; i++) {
    //Add each pair to a map so we can easily test
    metaKeys[std::string(updates[0].metadata[i].key)] = std::string(updates[0].metadata[i].value);
  }
  EXPECT_EQ(metaKeys[fakeMetaKey1], fakeMetaValue1);
  EXPECT_EQ(metaKeys[fakeMetaKey2], fakeMetaValue2);

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_missing_update_type) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);


  unsigned int fakeSize = 123;
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));


//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeUpdates(updateCount, &updates);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_missing_package_type) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);

  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));


//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeUpdates(updateCount, &updates);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_missing_md5) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);

  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));


//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeUpdates(updateCount, &updates);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_missing_updateId) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);

  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));


//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeUpdates(updateCount, &updates);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_missing_version) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);

  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));


//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeUpdates(updateCount, &updates);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_missing_download_url) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);

  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));


//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeUpdates(updateCount, &updates);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_missing_meta_data) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);

  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));


//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeUpdates(updateCount, &updates);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_malformed_json_from_server) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);

  std::string
      fakeOneUpdateResponse = "Totally not json!";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));


//Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeUpdates(updateCount, &updates);
  free(status->description);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_one_filter_correctly) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  unsigned int filterCount = 1;
  gaus_header_filter_t
      filters[1] = {strdup("firmware-version"), strdup("0.0.0")};

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, filters, &updateCount, &updates);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_EQ(curlPerformData[0].CURLOPT_URL,
            (serverUrl + "/device/" + fakeProductGuid + "/" + fakeDeviceGuid + "/check-for-updates?" +
             filters[0].filter_name + "=" + filters[0].filter_value).c_str());
  EXPECT_EQ(curlPerformData[0].CURLOPT_HTTPGET, 1L);

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(filters[0].filter_name);
  free(filters[0].filter_value);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_several_filter_correctly) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  unsigned int filterCount = 3;
  gaus_header_filter_t
      filters[3] = {{strdup("FILTER_1"), strdup("VALUE1")},
                    strdup("FILTER_2"), strdup("VALUE2"),
                    strdup("FILTER_3"), strdup("VALUE3")};

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, filters, &updateCount, &updates);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_EQ(curlPerformData[0].CURLOPT_URL,
            (serverUrl + "/device/" + fakeProductGuid + "/" + fakeDeviceGuid + "/check-for-updates?" +
             filters[0].filter_name + "=" + filters[0].filter_value + "&" +
             filters[1].filter_name + "=" + filters[1].filter_value + "&" +
             filters[2].filter_name + "=" + filters[2].filter_value
            ).c_str()


  );
  EXPECT_EQ(curlPerformData[0].CURLOPT_HTTPGET, 1L);

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  for (int i = 0; i < filterCount; i++) {
    free(filters[i].filter_name);
    free(filters[i].filter_value);
  }
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, uses_proxy_from_init) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);


  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());

  std::string fakeProxy = "fakeProxy";
  gaus_initialization_options_t options = {
      fakeProxy.c_str()
  };
  gaus_global_init(serverUrl.c_str(), &options);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlPerformData.size());
  EXPECT_EQ(fakeProxy, curlPerformData[0].CURLOPT_PROXY);

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, uses_ca_cert_from_init) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);


  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());

  std::string fakeCAPath = "fakeCAPath";
  gaus_initialization_options_t options = {
      .proxy = NULL,
      .ca_path = fakeCAPath.c_str()
  };

  gaus_global_init(serverUrl.c_str(), &options);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlPerformData.size());
  EXPECT_EQ(fakeCAPath, curlPerformData[0].CURLOPT_CAPATH);

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, does_not_use_proxy_if_null) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);


  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());

  gaus_initialization_options_t options = {
      NULL
  };
  gaus_global_init(serverUrl.c_str(), &options);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlPerformData.size());
  EXPECT_EQ(MOCK_NOT_SET, curlPerformData[0].CURLOPT_PROXY);

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, handles_really_long_tokens) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken(1000, 'a'); //Generate a massive token of 1000 a's
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);


  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "file";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  ASSERT_EQ(updateCount, 1);
  ASSERT_NE(updates, static_cast<gaus_update_t *>(NULL));
  EXPECT_EQ(updates[0].metadata_count, 1);
  EXPECT_EQ(updates[0].metadata[0].key, fakeMetaKey);
  EXPECT_EQ(updates[0].metadata[0].value, fakeMetaValue);
  EXPECT_EQ(updates[0].size, fakeSize);
  EXPECT_EQ(updates[0].update_type, fakeUpdateType);
  EXPECT_EQ(updates[0].package_type, fakePackageType);
  EXPECT_EQ(updates[0].md5, fakeMd5);
  EXPECT_EQ(updates[0].update_id, fakeUpdateId);
  EXPECT_EQ(updates[0].version, fakeVersion);
  EXPECT_EQ(updates[0].download_url, fakeDownloadUrl);

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, gets_a_nul_for_missing_things_if_not_file_type) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  free(fakeResponse);


  unsigned int fakeSize = 123;
  std::string fakeUpdateType = "firmware";
  std::string fakePackageType = "TOTALLY_RANDOM_TYPE";
  std::string fakeMd5 = "FAKEMD5";
  std::string fakeUpdateId = "FAKEUPDATEID";
  std::string fakeVersion = "FAKEVERSION";
  std::string fakeDownloadUrl = "FAKEDOWNLOADURL";
  std::string fakeMetaKey = "FAKEMETAKEY";
  std::string fakeMetaValue = "FAKEMETAVALUE";
  std::string
      fakeOneUpdateResponse = std::string("") + //Start with an empty string
                              "{" +
                              "\"updates\":[" +
                              "{" +
                              "\"metadata\": {" +
                              "\"" + fakeMetaKey + "\":" + "\"" + fakeMetaValue + "\"" +
                              "}," +
                              "\"size\": " + std::to_string(fakeSize) +
                              "," +
                              "\"updateType\": \"" + fakeUpdateType + "\"," +
                              "\"packageType\": \"" + fakePackageType + "\"," +
                              "\"md5\": \"" + fakeMd5 + "\"," +
                              "\"updateId\": \"" + fakeUpdateId + "\"," +
                              "\"version\": \"" + fakeVersion + "\"," +
                              "\"downloadUrl\": \"" + fakeDownloadUrl + "\"" +
                              "}" +
                              "]" +
                              "}";

  fakeResponse = strdup(fakeOneUpdateResponse.c_str());
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(updates[0].size, 0);
  EXPECT_EQ(updates[0].md5, static_cast<char *>(NULL));
  EXPECT_EQ(updates[0].download_url, static_cast<char *>(NULL));

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(status);
}

TEST_F(GausCheckForUpdates, has_correct_version_in_agent_header) {
  std::string serverUrl = "fakeServerUrl";
  std::string fakeDeviceGuid = "fakeDeviceGUID";
  std::string fakeProductGuid = "fakeProductGUID";
  std::string fakeToken = "fakeToken";
  gaus_session_t fakeSession = {
      strdup(fakeDeviceGuid.c_str()),
      strdup(fakeProductGuid.c_str()),
      strdup(fakeToken.c_str())
  };
  unsigned int filterCount = 0;
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;

  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_check_for_updates(&fakeSession, filterCount, NULL, &updateCount, &updates);

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

//Cleanup after test
  free(fakeSession
           .device_guid);
  free(fakeSession
           .product_guid);
  free(fakeSession
           .token);
  free(status);
}

//Test against a real backend
//#define TEST_GAUS_REAL
#ifdef TEST_GAUS_REAL

TEST_F(GausCheckForUpdates, try_for_real) {
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

  unsigned int filterCount = 1;
  gaus_header_filter_t filters[1] = {
      strdup("firmware-version"),
      strdup("0.0.0")
  };
  unsigned int updateCount = 0;
  gaus_update_t *updates = NULL;


  status = gaus_check_for_updates(&session, filterCount, filters, &updateCount, &updates);
  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);

  //Cleanup after test
  freeUpdates(updateCount, &updates);
  free(filters[0].filter_name);
  free(filters[0].filter_value);
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
