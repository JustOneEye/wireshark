/* capture_sync.h
 * Synchronisation between Wireshark capture parent and child instances
 *
 * Wireshark - Network traffic analyzer
 * By Gerald Combs <gerald@wireshark.org>
 * Copyright 1998 Gerald Combs
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 */


/** @file
 *
 *  Sync mode capture (internal interface).
 *
 *  Will start a new Wireshark child instance which will do the actual capture
 *  work.
 */

#ifndef __CAPTURE_SYNC_H__
#define __CAPTURE_SYNC_H__

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

struct _info_data;

/**
 * Start a new capture session.
 *  Create a capture child which is doing the real capture work.
 *  The various capture_input_... functions will be called, if something had
 *  happened.
 *
 *  Most of the parameters are passed through the global capture_opts.
 *
 *  @param capture_opts the options
 *  @param cap_session a handle for the capture session
 *  @param cap_data a struct with capture info data
 *  @param update_cb update screen
 *  @return             TRUE if a capture could be started, FALSE if not
 */
extern gboolean
sync_pipe_start(capture_options *capture_opts, capture_session *cap_session, struct _info_data* cap_data, void(*update_cb)(void));

/** User wants to stop capturing, gracefully close the capture child */
extern void
sync_pipe_stop(capture_session *cap_session);

/** User wants to stop the program, just kill the child as soon as possible */
extern void
sync_pipe_kill(ws_process_id fork_child);

/**
 * Set wireless channel using dumpcap
 *  On success, *data points to a buffer containing the dumpcap output,
 *  *primary_msg and *secondary_msg are NULL, and 0 is returned.  *data
 *  must be freed with g_free().
 *
 *  On failure, *data is NULL, *primary_msg points to an error message,
 *  *secondary_msg either points to an additional error message or is
 *  NULL, and -1 or errno value is returned; *primary_msg, and
 *  *secondary_msg if not NULL must be freed with g_free().
 *
 *  @param iface (monitor) network interface name
 *  @param freq channel control frequency string (in MHz)
 *  @param type channel type string (or NULL if not used)
 *  @param center_freq1 VHT channel center frequency (or NULL if not used)
 *  @param center_freq2 VHT channel center frequency 2 (or NULL if not used)
 *  @return 0 on success
 */
extern int
sync_interface_set_80211_chan(const gchar *iface, const char *freq, const gchar *type,
                              const gchar *center_freq1, const gchar *center_freq2,
                              gchar **data, gchar **primary_msg,
                              gchar **secondary_msg, void (*update_cb)(void));

/** Get an interface list using dumpcap */
extern int
sync_interface_list_open(gchar **data, gchar **primary_msg,
                         gchar **secondary_msg, void (*update_cb)(void));

/** Get interface capabilities using dumpcap */
extern int
sync_if_capabilities_open(const gchar *ifname, gboolean monitor_mode, const gchar* auth,
                          gchar **data, gchar **primary_msg,
                          gchar **secondary_msg, void (*update_cb)(void));

/** Start getting interface statistics using dumpcap. */
extern int
sync_interface_stats_open(int *read_fd, ws_process_id *fork_child, gchar **msg, void (*update_cb)(void));

/** Stop gathering statistics. */
extern int
sync_interface_stats_close(int *read_fd, ws_process_id *fork_child, gchar **msg);

/** Read a line from a pipe, similar to fgets.  Non-blocking. */
extern int
sync_pipe_gets_nonblock(int pipe_fd, char *bytes, int max);

/*
 * Routines supplied by our caller; we call them back to notify them
 * of various events.
 *
 * XXX - this is *really* ugly.  We should do this better.
 */

/**
 * Capture child told us we have a new (or the first) capture file.
 */
extern gboolean
capture_input_new_file(capture_session *cap_session, gchar *new_file);

/**
 * Capture child told us we have new packets to read.
 */
extern void
capture_input_new_packets(capture_session *cap_session, int to_read);

/**
 * Capture child told us how many dropped packets it counted.
 */
extern void
capture_input_drops(capture_session *cap_session, guint32 dropped, char* interface_name);

/**
 * Capture child told us that an error has occurred while starting the capture.
 */
extern void
capture_input_error_message(capture_session *cap_session, char *error_msg,
                            char *secondary_error_msg);

/**
 * Capture child told us that an error has occurred while parsing a
 * capture filter when starting/running the capture.
 */
extern void
capture_input_cfilter_error_message(capture_session *cap_session, guint i,
                                    char *error_message);

/**
 * Capture child closed its side of the pipe, report any error and
 * do the required cleanup.
 */
extern void
capture_input_closed(capture_session *cap_session, gchar *msg);

/* set a callback to be called after fork with the pid of the forked child */
extern void capture_sync_set_fetch_dumpcap_pid_cb(void(*cb)(ws_process_id pid));

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* __CAPTURE_SYNC_H__ */
