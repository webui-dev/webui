/* response.inl
 *
 * Bufferring for HTTP headers for HTTP response.
 * This function are only intended to be used at the server side.
 * Optional for HTTP/1.0 and HTTP/1.1, mandatory for HTTP/2.
 *
 * This file is part of the CivetWeb project.
 */

#if defined(NO_RESPONSE_BUFFERING) && defined(USE_HTTP2)
#error "HTTP2 works only if NO_RESPONSE_BUFFERING is not set"
#endif


/* Internal function to free header list */
static void
free_buffered_response_header_list(struct mg_connection *conn)
{
#if !defined(NO_RESPONSE_BUFFERING)
	while (conn->response_info.num_headers > 0) {
		conn->response_info.num_headers--;
		mg_free((void *)conn->response_info
		            .http_headers[conn->response_info.num_headers]
		            .name);
		conn->response_info.http_headers[conn->response_info.num_headers].name =
		    0;
		mg_free((void *)conn->response_info
		            .http_headers[conn->response_info.num_headers]
		            .value);
		conn->response_info.http_headers[conn->response_info.num_headers]
		    .value = 0;
	}
#else
	(void)conn; /* Nothing to do */
#endif
}


/* Send first line of HTTP/1.x response */
static int
send_http1_response_status_line(struct mg_connection *conn)
{
	const char *status_txt;
	const char *http_version = conn->request_info.http_version;
	int status_code = conn->status_code;

	if ((status_code < 100) || (status_code > 999)) {
		/* Set invalid status code to "500 Internal Server Error" */
		status_code = 500;
	}
	if (!http_version) {
		http_version = "1.0";
	}

	/* mg_get_response_code_text will never return NULL */
	status_txt = mg_get_response_code_text(conn, conn->status_code);

	if (mg_printf(
	        conn, "HTTP/%s %i %s\r\n", http_version, status_code, status_txt)
	    < 10) {
		/* Network sending failed */
		return 0;
	}
	return 1;
}


/* Initialize a new HTTP response
 * Parameters:
 *   conn: Current connection handle.
 *   status: HTTP status code (e.g., 200 for "OK").
 * Return:
 *   0:    ok
 *  -1:    parameter error
 *  -2:    invalid connection type
 *  -3:    invalid connection status
 *  -4:    network error (only if built with NO_RESPONSE_BUFFERING)
 */
int
mg_response_header_start(struct mg_connection *conn, int status)
{
	int ret = 0;
	if ((conn == NULL) || (status < 100) || (status > 999)) {
		/* Parameter error */
		return -1;
	}
	if ((conn->connection_type != CONNECTION_TYPE_REQUEST)
	    || (conn->protocol_type == PROTOCOL_TYPE_WEBSOCKET)) {
		/* Only allowed in server context */
		return -2;
	}
	if (conn->request_state != 0) {
		/* only allowed if nothing was sent up to now */
		return -3;
	}
	conn->status_code = status;
	conn->request_state = 1;

	/* Buffered response is stored, unbuffered response will be sent directly,
	 * but we can only send HTTP/1.x response here */
#if !defined(NO_RESPONSE_BUFFERING)
	free_buffered_response_header_list(conn);
#else
	if (!send_http1_response_status_line(conn)) {
		ret = -4;
	};
	conn->request_state = 1; /* Reset from 10 to 1 */
#endif

	return ret;
}


/* Add a new HTTP response header line
 * Parameters:
 *   conn: Current connection handle.
 *   header: Header name.
 *   value: Header value.
 *   value_len: Length of header value, excluding the terminating zero.
 *              Use -1 for "strlen(value)".
 * Return:
 *    0:    ok
 *   -1:    parameter error
 *   -2:    invalid connection type
 *   -3:    invalid connection status
 *   -4:    too many headers
 *   -5:    out of memory
 */
int
mg_response_header_add(struct mg_connection *conn,
                       const char *header,
                       const char *value,
                       int value_len)
{
#if !defined(NO_RESPONSE_BUFFERING)
	int hidx;
#endif

	if ((conn == NULL) || (header == NULL) || (value == NULL)) {
		/* Parameter error */
		return -1;
	}
	if ((conn->connection_type != CONNECTION_TYPE_REQUEST)
	    || (conn->protocol_type == PROTOCOL_TYPE_WEBSOCKET)) {
		/* Only allowed in server context */
		return -2;
	}
	if (conn->request_state != 1) {
		/* only allowed if mg_response_header_start has been called before */
		return -3;
	}

#if !defined(NO_RESPONSE_BUFFERING)
	hidx = conn->response_info.num_headers;
	if (hidx >= MG_MAX_HEADERS) {
		/* Too many headers */
		return -4;
	}

	/* Alloc new element */
	conn->response_info.http_headers[hidx].name =
	    mg_strdup_ctx(header, conn->phys_ctx);
	if (value_len >= 0) {
		char *hbuf =
		    (char *)mg_malloc_ctx((unsigned)value_len + 1, conn->phys_ctx);
		if (hbuf) {
			memcpy(hbuf, value, (unsigned)value_len);
			hbuf[value_len] = 0;
		}
		conn->response_info.http_headers[hidx].value = hbuf;
	} else {
		conn->response_info.http_headers[hidx].value =
		    mg_strdup_ctx(value, conn->phys_ctx);
	}

	if ((conn->response_info.http_headers[hidx].name == 0)
	    || (conn->response_info.http_headers[hidx].value == 0)) {
		/* Out of memory */
		mg_free((void *)conn->response_info.http_headers[hidx].name);
		conn->response_info.http_headers[hidx].name = 0;
		mg_free((void *)conn->response_info.http_headers[hidx].value);
		conn->response_info.http_headers[hidx].value = 0;
		return -5;
	}

	/* OK, header stored */
	conn->response_info.num_headers++;

#else
	if (value_len >= 0) {
		mg_printf(conn, "%s: %.*s\r\n", header, (int)value_len, value);
	} else {
		mg_printf(conn, "%s: %s\r\n", header, value);
	}
	conn->request_state = 1; /* Reset from 10 to 1 */
#endif

	return 0;
}


/* forward */
static int parse_http_headers(char **buf, struct mg_header hdr[MG_MAX_HEADERS]);


/* Add a complete header string (key + value).
 * Parameters:
 *   conn: Current connection handle.
 *   http1_headers: Header line(s) in the form "name: value".
 * Return:
 *  >=0:   no error, number of header lines added
 *  -1:    parameter error
 *  -2:    invalid connection type
 *  -3:    invalid connection status
 *  -4:    too many headers
 *  -5:    out of memory
 */
int
mg_response_header_add_lines(struct mg_connection *conn,
                             const char *http1_headers)
{
	struct mg_header add_hdr[MG_MAX_HEADERS];
	int num_hdr, i, ret;
	char *workbuffer, *parse;

	/* We need to work on a copy of the work buffer, sice parse_http_headers
	 * will modify */
	workbuffer = mg_strdup_ctx(http1_headers, conn->phys_ctx);
	if (!workbuffer) {
		/* Out of memory */
		return -5;
	}

	/* Call existing method to split header buffer */
	parse = workbuffer;
	num_hdr = parse_http_headers(&parse, add_hdr);
	ret = num_hdr;

	for (i = 0; i < num_hdr; i++) {
		int lret =
		    mg_response_header_add(conn, add_hdr[i].name, add_hdr[i].value, -1);
		if ((ret > 0) && (lret < 0)) {
			/* Store error return value */
			ret = lret;
		}
	}

	/* mg_response_header_add created a copy, so we can free the original */
	mg_free(workbuffer);
	return ret;
}


#if defined(USE_HTTP2)
static int http2_send_response_headers(struct mg_connection *conn);
#endif


/* Send http response
 * Parameters:
 *   conn: Current connection handle.
 * Return:
 *   0:    ok
 *  -1:    parameter error
 *  -2:    invalid connection type
 *  -3:    invalid connection status
 *  -4:    network send failed
 */
int
mg_response_header_send(struct mg_connection *conn)
{
#if !defined(NO_RESPONSE_BUFFERING)
	int i;
	int has_date = 0;
	int has_connection = 0;
#endif

	if (conn == NULL) {
		/* Parameter error */
		return -1;
	}
	if ((conn->connection_type != CONNECTION_TYPE_REQUEST)
	    || (conn->protocol_type == PROTOCOL_TYPE_WEBSOCKET)) {
		/* Only allowed in server context */
		return -2;
	}
	if (conn->request_state != 1) {
		/* only allowed if mg_response_header_start has been called before */
		return -3;
	}

	/* State: 2 */
	conn->request_state = 2;

#if !defined(NO_RESPONSE_BUFFERING)
#if defined(USE_HTTP2)
	if (conn->protocol_type == PROTOCOL_TYPE_HTTP2) {
		int ret = http2_send_response_headers(conn);
		free_buffered_response_header_list(conn);
		return (ret ? 0 : -4);
	}
#endif

	/* Send */
	if (!send_http1_response_status_line(conn)) {
		free_buffered_response_header_list(conn);
		return -4;
	};
	for (i = 0; i < conn->response_info.num_headers; i++) {
		mg_printf(conn,
		          "%s: %s\r\n",
		          conn->response_info.http_headers[i].name,
		          conn->response_info.http_headers[i].value);

		/* Check for some special headers */
		if (!mg_strcasecmp("Date", conn->response_info.http_headers[i].name)) {
			has_date = 1;
		}
		if (!mg_strcasecmp("Connection",
		                   conn->response_info.http_headers[i].name)) {
			has_connection = 1;
		}
	}

	if (!has_date) {
		time_t curtime = time(NULL);
		char date[64];
		gmt_time_string(date, sizeof(date), &curtime);
		mg_printf(conn, "Date: %s\r\n", date);
	}
	if (!has_connection) {
		mg_printf(conn, "Connection: %s\r\n", suggest_connection_header(conn));
	}
#endif

	mg_write(conn, "\r\n", 2);
	conn->request_state = 3;

	/* ok */
	free_buffered_response_header_list(conn);
	return 0;
}
