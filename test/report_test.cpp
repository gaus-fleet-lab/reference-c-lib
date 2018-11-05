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

class GausReport : public ::testing::Test {
protected:
  virtual void SetUp() {
    setupMocks();
    resetCurlMockHistory();
    //Setup a default fake response that will work for all tests.
    free(fakeResponse);
    fakeResponse = strdup("{}");
  }

  virtual void TearDown() {
    gaus_global_cleanup();
    cleanupMocks();
  }
};

static void freeReport(gaus_report_t report) {
  switch (report.report_type) {
    case GAUS_REPORT_UPDATE:
      for (int i = 0; i < report.report.update_status.v_string_count; i++) {
        free(report.report.update_status.v_strings[i].name);
        free(report.report.update_status.v_strings[i].value);
      }
      free(report.report.update_status.type);
      free(report.report.update_status.ts);
      break;
    case GAUS_REPORT_GENERIC:
      for (int i = 0; i < report.report.generic.v_int_count; i++) {
        free(report.report.generic.v_ints[i].name);
      }
      for (int i = 0; i < report.report.generic.v_float_count; i++) {
        free(report.report.generic.v_floats[i].name);
      }
      for (int i = 0; i < report.report.generic.v_string_count; i++) {
        free(report.report.generic.v_strings[i].name);
        free(report.report.generic.v_strings[i].value);
      }
      free(report.report.generic.type);
      free(report.report.generic.ts);
      break;
    default:
      throw "Unhandled report type being freed!";
  }
}


static void freeReports(unsigned int reportCount, gaus_report_t *reports) {
  for (int i = 0; i < reportCount; i++) {
    freeReport(reports[i]);
  }
}

TEST_F(GausReport, fails_without_initialize) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 0,
                  .v_strings = NULL,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(status->error_type, GAUS_NO_INIT_ERROR);
  EXPECT_EQ(status->http_error_code, 500);
  EXPECT_NE(strlen(status->description), 0);

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeReports(reportCount, report);
  free(header.ts);
  free(status->description);
  free(status);
}

TEST_F(GausReport, fails_without_session) {
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 0,
                  .v_strings = NULL,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_report(NULL, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(status->error_type, GAUS_UNKNOWN_ERROR);
  EXPECT_EQ(status->http_error_code, 500);
  EXPECT_NE(strlen(status->description), 0);

  //Cleanup after test
  free(status->description);
  freeReports(reportCount, report);
  free(header.ts);
  free(status);
}


TEST_F(GausReport, fails_without_device_guid) {
  gaus_session_t fakeSession = {
      NULL,
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 0,
                  .v_strings = NULL,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(status->error_type, GAUS_UNKNOWN_ERROR);
  EXPECT_EQ(status->http_error_code, 500);
  EXPECT_NE(strlen(status->description), 0);

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeReports(reportCount, report);
  free(header.ts);
  free(status->description);
  free(status);
}

TEST_F(GausReport, fails_without_product_guid) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      NULL,
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 0,
                  .v_strings = NULL,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(status->error_type, GAUS_UNKNOWN_ERROR);
  EXPECT_EQ(status->http_error_code, 500);
  EXPECT_NE(strlen(status->description), 0);

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeReports(reportCount, report);
  free(header.ts);
  free(status->description);
  free(status);
}

TEST_F(GausReport, fails_without_token) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      NULL
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 0,
                  .v_strings = NULL,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(status->error_type, GAUS_UNKNOWN_ERROR);
  EXPECT_EQ(status->http_error_code, 500);
  EXPECT_NE(strlen(status->description), 0);

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeReports(reportCount, report);
  free(header.ts);
  free(status->description);
  free(status);
}


TEST_F(GausReport, fails_if_filter_count_and_no_filter) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 1;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 0,
                  .v_strings = NULL,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(status->error_type, GAUS_UNKNOWN_ERROR);
  EXPECT_EQ(status->http_error_code, 500);
  EXPECT_NE(strlen(status->description), 0);

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeReports(reportCount, report);
  free(header.ts);
  free(status->description);
  free(status);
}

TEST_F(GausReport, fails_if_no_header) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  unsigned int reportCount = 1;
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 0,
                  .v_strings = NULL,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, NULL, reportCount, report);

  ASSERT_NE(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(status->error_type, GAUS_UNKNOWN_ERROR);
  EXPECT_EQ(status->http_error_code, 500);
  EXPECT_NE(strlen(status->description), 0);

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeReports(reportCount, report);
  free(status->description);
  free(status);
}


TEST_F(GausReport, fails_if_report_count_lt_1) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 0;
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 0,
                  .v_strings = NULL,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };

  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(status->error_type, GAUS_UNKNOWN_ERROR);
  EXPECT_EQ(status->http_error_code, 500);
  EXPECT_NE(strlen(status->description), 0);

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  freeReports(1, report); //Report count is "wrong" here for test
  free(header.ts);
  free(status->description);
  free(status);
}

TEST_F(GausReport, fails_if_no_report) {
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;

  gaus_global_init("fakeServer", NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, NULL);

  ASSERT_NE(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(status->error_type, GAUS_UNKNOWN_ERROR);
  EXPECT_EQ(status->http_error_code, 500);
  EXPECT_NE(strlen(status->description), 0);

  //Cleanup after test
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status->description);
  free(status);
}

TEST_F(GausReport, posts_to_correct_address) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);


  ASSERT_EQ(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_EQ(curlPerformData[0].CURLOPT_URL,
            serverUrl.append("/device/").append(fakeSession.product_guid).append("/").append(fakeSession.device_guid)
                .append("/report").c_str());
  //Cleanup after test
  freeReports(reportCount, report);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status);
}

TEST_F(GausReport, handles_one_filter_correctly) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 1;
  gaus_header_filter_t
      filters[1] = {strdup("firmware-version"), strdup("0.0.0")};
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, filters, &header, reportCount, report);


  ASSERT_EQ(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_EQ(curlPerformData[0].CURLOPT_URL,
            (serverUrl + "/device/" + fakeSession.product_guid + "/" + fakeSession.device_guid + "/report?" +
             filters[0].filter_name + "=" + filters[0].filter_value
            ).c_str());
  //Cleanup after test
  freeReports(reportCount, report);
  free(filters[0].filter_name);
  free(filters[0].filter_value);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status);
}

TEST_F(GausReport, handles_several_filter_correctly) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 3;
  gaus_header_filter_t
      filters[3] = {{strdup("FILTER_1"), strdup("VALUE1")},
                    strdup("FILTER_2"), strdup("VALUE2"),
                    strdup("FILTER_3"), strdup("VALUE3")};
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, filters, &header, reportCount, report);


  ASSERT_EQ(status, static_cast<gaus_error_t *>(NULL));
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_EQ(curlPerformData[0].CURLOPT_URL,
            (serverUrl + "/device/" + fakeSession.product_guid + "/" + fakeSession.device_guid + "/report?" +
             filters[0].filter_name + "=" + filters[0].filter_value + "&" +
             filters[1].filter_name + "=" + filters[1].filter_value + "&" +
             filters[2].filter_name + "=" + filters[2].filter_value
            ).c_str());
  //Cleanup after test
  freeReports(reportCount, report);
  for (int i = 0; i < filterCount; i++) {
    free(filters[i].filter_name);
    free(filters[i].filter_value);
  }
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status);
}

TEST_F(GausReport, posts_correct_json_for_update_report) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"version\":\"1.0.0\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"header\":{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"ts\":\"" + std::string(header.ts) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"data\":[{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"type\":\"event.update.Status\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"ts\":\"" + std::string(report[0].report.update_status.ts) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"v_strings\":{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"" + std::string(vstrings[0].name) + "\":\"" + std::string(vstrings[0].value) + "\""));


  //Cleanup after test
  //Free report:
  freeReports(reportCount, report);

  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status);
}


TEST_F(GausReport, posts_correct_json_for_two_update_report) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 2;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_v_string_t vstrings2[1] = {
      strdup("key2"),
      strdup("value2")
  };
  gaus_report_t report[2] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      },
      {
          .report = {
              .update_status = {
                  .type = strdup("Status2"),
                  .ts = strdup("FAKE_TIME2"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings2,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"version\":\"1.0.0\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"header\":{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"ts\":\"" + std::string(header.ts) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"data\":[{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"type\":\"event.update.Status\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"ts\":\"" + std::string(report[0].report.update_status.ts) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"v_strings\":{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"" + std::string(vstrings[0].name) + "\":\"" + std::string(vstrings[0].value) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"ts\":\"" + std::string(report[1].report.update_status.ts) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"" + std::string(vstrings2[0].name) + "\":\"" + std::string(vstrings2[0].value) + "\""));

  //Cleanup after test
  //Free report:
  freeReports(reportCount, report);
  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status);
}

static CURLcode mock_curl_easy_perform_failed(CURL *curl) {
  //Always return ok
  return CURLE_HTTP_RETURNED_ERROR;
}

TEST_F(GausReport, handles_errors_on_perform_without_status) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_perform = mock_curl_easy_perform_failed;

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_UNKNOWN_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  //Free report:
  freeReports(reportCount, report);

  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
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

TEST_F(GausReport, handles_errors_on_perform_with_status_500) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_getinfo = mock_curl_easy_getinfo_return_500;

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_HTTP_ERROR, status->error_type);
  EXPECT_EQ(500, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  //Free report:
  freeReports(reportCount, report);

  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
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

TEST_F(GausReport, handles_errors_on_perform_with_status_400) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_curl_easy_getinfo = mock_curl_easy_getinfo_return_400;

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_NE(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(GAUS_HTTP_ERROR, status->error_type);
  EXPECT_EQ(400, status->http_error_code);
  EXPECT_NE(0, strlen(status->description));

  //Cleanup after test
  //Free report:
  freeReports(reportCount, report);

  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status->description);
  free(status);
}

TEST_F(GausReport, has_correct_auth_header) {
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
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };

  std::string fakeProxy = "fakeProxy";
  gaus_initialization_options_t options = {
      fakeProxy.c_str()
  };
  gaus_global_init(serverUrl.c_str(), &options);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

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


  //Cleanup after test
  //Free report:
  freeReports(reportCount, report);

  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status);
}

TEST_F(GausReport, uses_proxy_from_init) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };

  std::string fakeProxy = "fakeProxy";
  gaus_initialization_options_t options = {
      fakeProxy.c_str()
  };
  gaus_global_init(serverUrl.c_str(), &options);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlPerformData.size());
  EXPECT_EQ(fakeProxy, curlPerformData[0].CURLOPT_PROXY);

  //Cleanup after test
  //Free report:
  freeReports(reportCount, report);

  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status);
}

TEST_F(GausReport, does_not_use_proxy_if_null) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .update_status = {
                  .type = strdup("Status"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 0,
                  .v_ints = NULL,
                  .v_float_count = 0,
                  .v_floats = NULL,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_UPDATE
      }
  };

  gaus_initialization_options_t options = {
      NULL
  };
  gaus_global_init(serverUrl.c_str(), &options);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);

  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(1, curlPerformData.size());
  EXPECT_EQ(MOCK_NOT_SET, curlPerformData[0].CURLOPT_PROXY);

  //Cleanup after test
  //Free report:
  freeReports(reportCount, report);

  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status);
}

TEST_F(GausReport, posts_correct_json_for_generic_report) {
  std::string serverUrl = "fakeServerUrl";
  gaus_session_t fakeSession = {
      strdup("fakeDeviceGUID"),
      strdup("fakeProductGUID"),
      strdup("fakeToken")
  };
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup("FAKE_TIMESTAMP")
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_v_float_t vfloats[1] = {
      strdup("key"),
      1.23f
  };
  gaus_v_int_t vints[1] = {
      strdup("key"),
      1234
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .generic = {
                  .type = strdup("FAKE_TYPE"),
                  .ts = strdup("FAKE_TIME"),
                  .v_int_count = 1,
                  .v_ints = vints,
                  .v_float_count = 1,
                  .v_floats = vfloats,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_GENERIC
      }
  };
  gaus_global_init(serverUrl.c_str(), NULL);

  gaus_error_t *status = gaus_report(&fakeSession, filterCount, NULL, &header, reportCount, report);


  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);
  EXPECT_EQ(curlPerformData.size(), 1);
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"version\":\"1.0.0\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"header\":{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"ts\":\"" + std::string(header.ts) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"data\":["));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"type\":\"event.generic." + std::string(report[0].report.generic.type) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"ts\":\"" + std::string(report[0].report.generic.ts) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"v_strings\":{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"" + std::string(vstrings[0].name) + "\":\"" + std::string(vstrings[0].value) + "\""));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"v_ints\":{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"" + std::string(vints[0].name) + "\":" + std::to_string(vints[0].value)));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find("\"v_floats\":{"));
  EXPECT_NE(std::string::npos, curlPerformData[0].CURLOPT_POSTFIELDS.find(
      "\"" + std::string(vfloats[0].name) + "\":" + std::to_string(vfloats[0].value)));

  //Cleanup after test
  //Free report:
  freeReports(reportCount, report);

  free(fakeSession.device_guid);
  free(fakeSession.product_guid);
  free(fakeSession.token);
  free(header.ts);
  free(status);
}

//Test against a real backend
//#define TEST_GAUS_REAL
#ifdef TEST_GAUS_REAL

TEST_F(GausReport, try_for_real) {
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

  auto time_seconds = std::time(nullptr);
  char time[80];
  std::strftime(time, sizeof(time), "%FT%T.000Z", std::gmtime(&time_seconds));
  unsigned int filterCount = 0;
  gaus_report_header_t header = {
      strdup(time)
  };
  unsigned int reportCount = 1;
  gaus_v_string_t vstrings[1] = {
      strdup("key"),
      strdup("value")
  };
  gaus_v_float_t vfloats[1] = {
      strdup("key"),
      1.23f
  };
  gaus_v_int_t vints[1] = {
      strdup("key"),
      1234
  };
  gaus_report_t report[1] = {
      {
          .report = {
              .generic = {
                  .type = strdup("Firmware"),
                  .ts = strdup(time),
                  .v_int_count = 1,
                  .v_ints = vints,
                  .v_float_count = 1,
                  .v_floats = vfloats,
                  .v_string_count = 1,
                  .v_strings = vstrings,
                  .tag_count = 0,
                  .tags = NULL
              }
          },
          .report_type = GAUS_REPORT_GENERIC
      }
  };
  status = gaus_report(&session, filterCount, NULL, &header, reportCount, report);
  ASSERT_EQ(static_cast<gaus_error_t *>(NULL), status);

  //Cleanup after test
  freeReports(reportCount, report);
  free(header.ts);
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
