//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef UPDATE_CLIENT_C_GAUS_CLIENT_RESPONSE_TYPES_H
#define UPDATE_CLIENT_C_GAUS_CLIENT_RESPONSE_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

//Reporting types:
/*************************************************************//**
 *
 * \brief A type to represent a name/value pair of type integer
 *
 *************************************************************/
typedef struct {
  char *name; //!< A null terminated name
  int value;  //!< An integer value
} gaus_v_int_t;


/*************************************************************//**
 *
 * \brief A type to represent a name/value pair of type float
 *
 *************************************************************/
typedef struct {
  char *name; //!< A null terminated name
  float value;  //!< An float value
} gaus_v_float_t;

/*************************************************************//**
 *
 * \brief A type to represent a name/value pair of type string
 *
 *************************************************************/
typedef struct {
  char *name; //!< A null terminated name
  char *value;  //!< A null terminated string value
} gaus_v_string_t;

/*************************************************************//**
 *
 * \brief A type to represent a name/value pair of tag in a report
 *
 *************************************************************/
typedef struct {
  char *name; //!< A null terminated name
  char *value;  //!< A null terminated string value
} gaus_report_tag_t;

/*************************************************************//**
 *
 * \brief A type to represent possible types of reports.
 *
 * This type is used in order to create a tagged union to pass to \c gaus_report.
 *
 *************************************************************/
typedef enum {
  GAUS_REPORT_COUNTER, //!< metric.counter.* type
  GAUS_REPORT_GAUGE,   //!< metric.gauge.* type
  GAUS_REPORT_GENERIC, //!< metric.generic.* type
  GAUS_REPORT_UPDATE   //!< metric.update.* type
} gaus_report_type_t;

/*************************************************************//**
 *
 * \brief The sub type used to make a report of type `metric.counter.*`
 *
 *************************************************************/
typedef struct {
  /*!A null terminated type for this counter report
   *
   * This will result in a type of `metric.counter.<type>` being sent to the gaus backend. Therefore you do not need to
   * include the `metric.counter.` part this will automatically be added for you.
   */
  char *type;

  /*!A null terminated timestamp specifying collection time.
   *
   * Must be formatted in ISO 8601 as UTC (Ending with a `Z`)
   */
  char *ts;
  /*!A count of how many v_ints are in the gaus_report_metric_counter_t::v_ints array
   */
  unsigned int v_int_count;
  /*! Null if no v_ints to report, or a pointer to an array of v_ints
   */
  gaus_v_int_t *v_ints;
  /*! A count of how many v_floats are in the gaus_report_metric_counter_t::v_floats array
   */
  unsigned int v_float_count;
  /*! Null if no v_floats to report, or a pointer to an array of v_floats
   */
  gaus_v_float_t *v_floats;
  /*! A count of how many tags are in gaus_report_metric_counter_t::tags array
   */
  unsigned int tag_count;
  /*! Null if no tags to report, or a pointer to an array of `gaus_report_tag_t`s
   */
  gaus_report_tag_t *tags;
} gaus_report_metric_counter_t;

/*************************************************************//**
 *
 * \brief The sub type used to make a report of type `metric.gauge.*`
 *
 *************************************************************/
typedef struct {
  /*!A null terminated type for this counter report
   *
   * This will result in a type of `metric.gauge.<type>` being sent to the gaus backend. Therefore you do not need to
   * include the `metric.gauge.` part this will automatically be added for you.
   */
  char *type;

  /*!A null terminated timestamp specifying collection time.
   *
   * Must be formatted in ISO 8601 as UTC (Ending with a `Z`)
   */
  char *ts;
  /*!A count of how many v_ints are in the gaus_report_metric_gauge::v_ints array
   */
  unsigned int v_int_count;
  /*! Null if no v_ints to report, or a pointer to an array of v_ints
   */
  gaus_v_int_t *v_ints;
  /*! A count of how many v_floats are in the gaus_report_metric_gauge::v_floats array
   */
  unsigned int v_float_count;
  /*! Null if no v_floats to report, or a pointer to an array of v_float_count
   */
  gaus_v_float_t *v_floats;
  /*! A count of how many tags are in gaus_report_metric_gauge_t::tags array
   */
  unsigned int tag_count;
  /*! Null if no tags to report, or a pointer to an array of `gaus_report_tag_t`s
   */
  gaus_report_tag_t *tags;
} gaus_report_metric_gauge_t;

/*************************************************************//**
 *
 * \brief The sub type used to make a report of type `event.generic.*`
 *
 *************************************************************/
typedef struct {
  /*!A null terminated type for this generic event report
   *
   * This will result in a type of `event.generic.<type>` being sent to the gaus backend. Therefore you do not need to
   * include the `event.generic.` part this will automatically be added for you.
   */
  char *type;

  /*!A null terminated timestamp specifying collection time.
   *
   * Must be formatted in ISO 8601 as UTC (Ending with a `Z`)
   */
  char *ts;
  /*!A count of how many v_ints are in the gaus_report_event_generic_t::v_ints array
   */
  unsigned int v_int_count;
  /*! Null if no v_ints to report, or a pointer to an array of v_ints
   */
  gaus_v_int_t *v_ints;
  /*! A count of how many v_floats are in the gaus_report_event_generic_t::v_floats array
   */
  unsigned int v_float_count;
  /*! Null if no v_floats to report, or a pointer to an array of v_floats
   */
  gaus_v_float_t *v_floats;
  /*! A count of how many v_strings are in the gaus_report_event_generic_t::v_strings array
   */
  unsigned int v_string_count;
  /*! Null if no v_strings to report, or a pointer to an array of v_strings
   */
  gaus_v_string_t *v_strings;
  /*! A count of how many tags are in gaus_report_event_generic_t::tags array
   */
  unsigned int tag_count;
  /*! Null if no tags to report, or a pointer to an array of `gaus_report_tag_t`s
   */
  gaus_report_tag_t *tags;
} gaus_report_event_generic_t;


/*************************************************************//**
 *
 * \brief The sub type used to make a report of type `event.update.*`
 *
 *************************************************************/
typedef struct {
  /*!A null terminated type for this generic event report
   *
   * This will result in a type of `event.update.<type>` being sent to the gaus backend. This is expected to be set to
   * 'Status'
   */
  char *type;

  /*!A null terminated timestamp specifying collection time.
   *
   * Must be formatted in ISO 8601 as UTC (Ending with a `Z`)
   */
  char *ts;
  /*!A count of how many v_ints are in the gaus_report_event_update_status_t::v_ints array
   */
  unsigned int v_int_count;
  /*! Null if no v_ints to report, or a pointer to an array of v_ints
   */
  gaus_v_int_t *v_ints;
  /*! A count of how many v_floats are in the gaus_report_event_update_status_t::v_floats array
   */
  unsigned int v_float_count;
  /*! Null if no v_floats to report, or a pointer to an array of v_ints
   */
  gaus_v_float_t *v_floats;
  /*! A count of how many v_strings are in the gaus_report_event_update_status_t::v_strings array
   */
  unsigned int v_string_count;
  /*! Null if no v_floats to report, or a pointer to an array of v_floats
   */
  gaus_v_string_t *v_strings;
  /*! A count of how many tags are in gaus_report_event_update_status_t::tags array
 */
  unsigned int tag_count;
  /*! Null if no tags to report, or a pointer to an array of `gaus_report_tag_t`s
   */
  gaus_report_tag_t *tags;
} gaus_report_event_update_status_t;

/*************************************************************//**
 *
 * \brief A union of all report types.
 *
 * This is not intended to be used directly but used through gaus_report_t
 *
 *************************************************************/
typedef union {
  gaus_report_metric_counter_t counter;           //!< The counter metric report if this union currently holds one.
  gaus_report_metric_gauge_t gauge;               //!< The gauge metric report if this union currently holds one.
  gaus_report_event_generic_t generic;            //!< The generic event report if this union currently holds one.
  gaus_report_event_update_status_t update_status;//!< The generic event report if this union currently holds one.
} gaus_report_contents_t;


/*************************************************************//**
 *
 * \brief The type used to make a report with \c gaus_report
 *
 * In order to be valid both the gaus_report_t::report **and** the gaus_report_t::report_type must be set.
 *
 *************************************************************/
typedef struct {
  gaus_report_contents_t report;    //!< The report (type is specified by the gaus_report_t::report_type)
  gaus_report_type_t report_type;   //!< The type of report in this union
} gaus_report_t;

/*************************************************************//**
 *
 * \brief The type used to specify report headers with \c gaus_report
 *
 *************************************************************/
typedef struct {
  /*!A null terminated timestamp specifying when this data was sent. (Client clock)
   *
   * Must be formatted in ISO 8601 as UTC (Ending with a `Z`)
   */
  char *ts;
} gaus_report_header_t;

#ifdef __cplusplus
}
#endif
#endif //UPDATE_CLIENT_C_GAUS_CLIENT_RESPONSE_TYPES_H
