//The MIT License (MIT)
//
//Copyright 2018, Sony Mobile Communications Inc.
//
//Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
//
//The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
//
//THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#ifndef UPDATE_CLIENT_C_GAUS_CLIENT_H
#define UPDATE_CLIENT_C_GAUS_CLIENT_H

/*************************************************************//**
 * \file gaus_client.h
 * \brief The include file to use Gaus C client library
 *
 * \mainpage Gaus C Client Library
 * \section Introduction
 *
 * This example client library is provided to use as is either to create a C or C++ library, or to use as a reference
 * when integrating your own library with Gaus.
 *
 * Good Luck!
 *
 * Include this file to use the Gaus C client library.
 *
 * For help, feedback, or questions please contact gaus@sony.com
 *************************************************************/
#include "gaus_client_types.h"
#include "gaus_client_report_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/*************************************************************//**
 *
 * \brief Get the current version of Gaus client library.
 *
 * \return gaus_version_t: The current version of this library in semver format.
 *
 *************************************************************/
gaus_version_t gaus_client_library_version(void);

/*************************************************************//**
 *
 * \brief Initialize the gaus library
 *
 * This function must be called at least once within a program (a program is all the code that shares a memory space)
 * before the program calls any other function in libgaus. The environment it sets up is constant for the life of the
 * program and is the same for every program, so multiple calls have the same effect as one call.
 *
 * **This function is not thread safe.** You must not call it when any other thread in the program (i.e. a thread
 * sharing the same memory) is running. This doesn't just mean no other thread that is using libgaus.
 * Because ::gaus_global_init calls functions of other libraries that are similarly thread unsafe, it could conflict
 * with any other thread that uses these other libraries.
 *
 * The basic rule for constructing a program that uses libgaus is this: Call ::gaus_global_init immediately after the
 * program starts, while it is still only one thread and before it uses libgaus at all. Call gaus_global_cleanup
 * immediately before the program exits, when the program is again only one thread and after its last use of libgaus.
 *
 * The serverUrl provided will be used for all subsequent gaus calls.
 *
 * Parameters:
 * \param[in] serverUrl: A weak pointer to a null terminated url.  For example "http://example.gaus.com"
 * \param[in] options: A weak pointer to gaus_initialization_options_t may be passed in specifying what options to
 *   initialize the library with. If default options are desired pass in NULL.
 * \return gaus_error_t* A strong pointer to an error describing what went wrong, or `NULL`.  The caller is responsible
 *   for freeing this memory if non null.
 *
 *************************************************************/
gaus_error_t *gaus_global_init(const char *serverUrl, const gaus_initialization_options_t *options);

/*************************************************************//**
 *
 * \brief Register a new device
 *
 * A blocking synchronous call to register a new device.  This is only called once in a devices lifetime to register the
 * device.  On success the function will return `NULL`, and will set the deviceSecret, deviceAccess, and a pollInterval
 * that should be persisted for the lifetime of the device.
 *
 * Out parameters are only valid if return value is `NULL`.  To prevent memory leaks out parameters (and their contents)
 * should always be freed by the caller.
 *
 * \param[in] product_access: A weak pointer to a null terminated product access code, this is generated by the
 *        gaus backend and is specific to the product line being registered.
 * \param[in] product_secret: A weak pointer to a null terminated product secret, this is generated on the gaus
 *        backend and is specific to the product line being registered.
 * \param[in] device_id: A weak pointer to a null terminated deviceId.  This is specific to the device being
 *        registered, for example a MAC address or a unique serial number.
 * \param[out] device_access: A strong pointer to a null terminated deviceAccess code generated by gaus backend. This
 *   should be persisted for use in future.  Free will be called on the current contents of *device_access prior to
 *   allocating the correct amount of memory to store the retrieved device_access code.  Caller is responsible for
 *   freeing this memory.
 * \param[out] device_secret: A strong pointer to a null terminated deviceSecret generated by gaus backend. This
 *   should be persisted for use in future.  Free will be called on the current contents of *device_secret prior to
 *   allocating the correct amount of memory to store the retrieved device_secret code.  Caller is responsible for
 *   freeing this memory.
 * \param[out] pollIntervalSeconds: A pointer to a single unsigned integer, this will be filled by the ::gaus_register
 *   function with a value in seconds with a suggested poll time for how often this device should poll for updates.
 *   This should be persisted for use in future.  Caller is responsible for freeing this memory (if any was allocated).
 *   ::gaus_register does not allocate any memory for this single int so the pointer must point to a valid integer
 *   location.
 * \return gaus_error_t* A strong pointer to an error describing what went wrong, or `NULL`.  The caller is responsible
 *   for freeing this memory if non null.
 *
 *************************************************************/
gaus_error_t *gaus_register(const char *product_access, const char *product_secret, const char *device_id,
                            char **device_access, char **device_secret, unsigned int *poll_interval_seconds);


/*************************************************************//**
 *
 * \brief Authenticate a device
 *
 * Start a new session with Gaus.  This blocking synchronous call gets the \c ::gaus_session_t needed for future
 * communication with the gaus backend.  All future calls will be expected to provide this session, in the event that
 * the session expires this function should be called again.
 *
 * Out parameters are only valid if return value is `NULL`.  To prevent memory leaks out parameters (and their contents)
 * should always be freed by the caller.
 *
 * \param[in] device_access: A weak pointer to a null terminated deviceAccess code, this should be retrieved
 *   from the location it was persisted to after the original call to \c ::gaus_register
 * \param[in] device_secret: A weak pointer to a null terminated deviceSecret, this should be retrieved from the
 *   location it was persisted to after the original call to \c ::gaus_register.
 * \param[out] session: A strong pointer to a gaus_session_t.  This should be saved in session memory for future use.
 *   ::gaus_authenticate will call free on the current contents of session and and allocate new memory as required.  The
 *   caller is responsible for freeing the contents of session.
 *
 * \return gaus_error_t A strong pointer to an error describing what went wrong, or `NULL`.  The caller is responsible
 *   for freeing this memory if non null.
 *************************************************************/
gaus_error_t *gaus_authenticate(const char *device_access, const char *device_secret, gaus_session_t *session);


/*************************************************************//**
 *
 * \brief Check Gaus for updates
 *
 * Check Gaus to find any available updates for this device.  This is a synchronous blocking call. The caller is
 * responsible for processing the updates appropriately after the call returns.
 *
 * Out parameters are only valid if return value is `NULL`.  To prevent memory leaks out parameters (and their contents)
 * should always be freed by the caller.
 *
 * \param[in] session: A weak pointer to a session generated by gaus backend during \c ::gaus_authenticate call.
 * \param[in] filter_count: An integer specifying the number of filters in the filters parameter.
 * \param[in] filters: A weak pointer to an array of filters to be used to build the query string parameters.
 * \param[out] update_count: A strong pointer an int with the number of updates contained in updates.  Caller is
 *   responsible for freeing this memory (if any was allocated). ::gaus_register does not allocate any memory for this
 *   single int so the pointer must point to a valid integer location.
 * \param[out] updates: A strong pointer to an array of gaus_update_t updates.  Each of these updates should be
 *   processed by the caller.  \c::gaus_check_for_updates assumes that no memory is currently allocated and will allocate
 *   memory as required to store the updates.  The caller is responsible for freeing both the array of updates, and its
 *   members.
 *
 * \return gaus_error_t A strong pointer to an error describing what went wrong, or `NULL`.  The caller is responsible
 *   for freeing this memory if non null.
 *
 *************************************************************/
gaus_error_t *
gaus_check_for_updates(const gaus_session_t *session, unsigned int filter_count, const gaus_header_filter_t *filters,
                       unsigned int *update_count, gaus_update_t **updates);


/*************************************************************//**
 *
 * \brief Report an update to gaus.
 *
 * This is a synchronous blocking call.  It is used to make a report to the gaus system.
 *
 * Parameters:
 * \param[in] session: A weak pointer to a \c ::gaus_session_t generated by gaus backend during \c ::gaus_authenticate call.
 * \param[in] filter_count: An integer specifying the number of filters in the filters parameter.
 * \param[in] filters: A weak pointer to an array of filters to be used to build the query string parameters.
 * \param[in] header: A weak pointer to a \c ::gaus_report_header_t containing the header for this data.
 * \param[in] reports: A weak pointer to an array of \c ::gaus_report_ts containing the data that you wish to report.
 *   This is a tagged union and as such both the gaus_report_t::report_type and one of the relevant members.
 *
 * \return gaus_error_t A strong pointer to an error describing what went wrong, or `NULL`.  The caller is responsible
 *   for freeing this memory if non null.
 *************************************************************/
gaus_error_t *gaus_report(const gaus_session_t *session, unsigned int filter_count, const gaus_header_filter_t *filters,
                          const gaus_report_header_t *header, unsigned int report_count, const gaus_report_t *reports);

/*************************************************************//**
 *
 * \brief Cleanup the gaus library
 *
 * This function releases resources acquired by \c ::gaus_global_init.
 *
 * You should call ::gaus_global_cleanup once for each call you make to gaus_global_init, after you are done using
 * libgaus.
 *
 * This function is not thread safe. You must not call it when any other thread in the program (i.e. a thread sharing
 * the same memory) is running. This doesn't just mean no other thread that is using libgaus. Because
 * ::gaus_global_cleanup calls functions of other libraries that are similarly thread unsafe, it could conflict with
 * any other thread that uses these other libraries.
 *
 * Parameters:
 * \return void
 *
 *************************************************************/
void gaus_global_cleanup(void);

#ifdef __cplusplus
}
#endif
#endif //UPDATE_CLIENT_C_GAUS_CLIENT_H
