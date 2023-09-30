/* Reimplementation of pattern matching */
/* This file is part of the CivetWeb web server.
 * See https://github.com/civetweb/civetweb/
 */


/* Initialize structure with 0 matches */
static void
match_context_reset(struct mg_match_context *mcx)
{
	mcx->num_matches = 0;
	memset(mcx->match, 0, sizeof(mcx->match));
}


/* Add a new match to the list of matches */
static void
match_context_push(const char *str, size_t len, struct mg_match_context *mcx)
{
	if (mcx->num_matches < MG_MATCH_CONTEXT_MAX_MATCHES) {
		mcx->match[mcx->num_matches].str = str;
		mcx->match[mcx->num_matches].len = len;
		mcx->num_matches++;
	}
}


static ptrdiff_t
mg_match_impl(const char *pat,
              size_t pat_len,
              const char *str,
              struct mg_match_context *mcx)
{
	/* Parse string */
	size_t i_pat = 0; /* Pattern index */
	size_t i_str = 0; /* Pattern index */

	int case_sensitive = ((mcx != NULL) ? mcx->case_sensitive : 0); /* 0 or 1 */

	while (i_pat < pat_len) {

		/* Pattern ? matches one character, except / and NULL character */
		if ((pat[i_pat] == '?') && (str[i_str] != '\0')
		    && (str[i_str] != '/')) {
			size_t i_str_start = i_str;
			do {
				/* Advance as long as there are ? */
				i_pat++;
				i_str++;
			} while ((i_pat < pat_len) && (pat[i_pat] == '?')
			         && (str[i_str] != '\0') && (str[i_str] != '/'));

			/* If we have a match context, add the substring we just found */
			if (mcx) {
				match_context_push(str + i_str_start, i_str - i_str_start, mcx);
			}

			/* Reached end of pattern ? */
			if (i_pat == pat_len) {
				return (ptrdiff_t)i_str;
			}
		}

		/* Pattern $ matches end of string */
		if (pat[i_pat] == '$') {
			return (str[i_str] == '\0') ? (ptrdiff_t)i_str : -1;
		}

		/* Pattern * or ** matches multiple characters */
		if (pat[i_pat] == '*') {
			size_t len; /* length matched by "*" or "**" */
			ptrdiff_t ret;

			i_pat++;
			if ((i_pat < pat_len) && (pat[i_pat] == '*')) {
				/* Pattern ** matches all */
				i_pat++;
				len = strlen(str + i_str);
			} else {
				/* Pattern * matches all except / character */
				len = strcspn(str + i_str, "/");
			}

			if (i_pat == pat_len) {
				/* End of pattern reached. Add all to match context. */
				if (mcx) {
					match_context_push(str + i_str, len, mcx);
				}
				return ((ptrdiff_t)(i_str + len));
			}

			/* This loop searches for the longest possible match */
			do {
				ret = mg_match_impl(pat + i_pat,
				                    (pat_len - (size_t)i_pat),
				                    str + i_str + len,
				                    mcx);
			} while ((ret == -1) && (len-- > 0));

			/* If we have a match context, add the substring we just found */
			if (ret >= 0) {
				if (mcx) {
					match_context_push(str + i_str, len, mcx);
				}
				return ((ptrdiff_t)i_str + ret + (ptrdiff_t)len);
			}

			return -1;
		}


		/* Single character compare */
		if (case_sensitive) {
			if (pat[i_pat] != str[i_str]) {
				/* case sensitive compare: mismatch */
				return -1;
			}
		} else if (lowercase(&pat[i_pat]) != lowercase(&str[i_str])) {
			/* case insensitive compare: mismatch */
			return -1;
		}

		i_pat++;
		i_str++;
	}
	return (ptrdiff_t)i_str;
}


static ptrdiff_t
mg_match_alternatives(const char *pat,
                      size_t pat_len,
                      const char *str,
                      struct mg_match_context *mcx)
{
	const char *match_alternative = (const char *)memchr(pat, '|', pat_len);

	if (mcx != NULL) {
		match_context_reset(mcx);
	}

	while (match_alternative != NULL) {
		/* Split at | for alternative match */
		size_t left_size = (size_t)(match_alternative - pat);

		/* Try left string first */
		ptrdiff_t ret = mg_match_impl(pat, left_size, str, mcx);
		if (ret >= 0) {
			/* A 0-byte match is also valid */
			return ret;
		}

		/* Reset possible incomplete match data */
		if (mcx != NULL) {
			match_context_reset(mcx);
		}

		/* If no match: try right side */
		pat += left_size + 1;
		pat_len -= left_size + 1;
		match_alternative = (const char *)memchr(pat, '|', pat_len);
	}

	/* Handled all | operators. This is the final string. */
	return mg_match_impl(pat, pat_len, str, mcx);
}


static int
match_compare(const void *p1, const void *p2, void *user)
{
	const struct mg_match_element *e1 = (const struct mg_match_element *)p1;
	const struct mg_match_element *e2 = (const struct mg_match_element *)p2;

	/* unused */
	(void)user;

	if (e1->str > e2->str) {
		return +1;
	}
	if (e1->str < e2->str) {
		return -1;
	}
	return 0;
}


#if defined(MG_EXPERIMENTAL_INTERFACES)
CIVETWEB_API
#else
static
#endif
ptrdiff_t
mg_match(const char *pat, const char *str, struct mg_match_context *mcx)
{
	size_t pat_len = strlen(pat);
	ptrdiff_t ret = mg_match_alternatives(pat, pat_len, str, mcx);
	if (mcx != NULL) {
		if (ret < 0) {
			/* Remove possible incomplete data */
			match_context_reset(mcx);
		} else {
			/* Join "?*" to one pattern. */
			size_t i, j;

			/* Use difference of two array elements instead of sizeof, since
			 * there may be some additional padding bytes. */
			size_t elmsize =
			    (size_t)(&mcx->match[1]) - (size_t)(&mcx->match[0]);

			/* First sort the matches by address ("str" begin to end) */
			mg_sort(mcx->match, mcx->num_matches, elmsize, match_compare, NULL);

			/* Join consecutive matches */
			i = 1;
			while (i < mcx->num_matches) {
				if ((mcx->match[i - 1].str + mcx->match[i - 1].len)
				    == mcx->match[i].str) {
					/* Two matches are consecutive. Join length. */
					mcx->match[i - 1].len += mcx->match[i].len;

					/* Shift all list elements. */
					for (j = i + 1; j < mcx->num_matches; j++) {
						mcx->match[j - 1].len = mcx->match[j].len;
						mcx->match[j - 1].str = mcx->match[j].str;
					}

					/* Remove/blank last list element. */
					mcx->num_matches--;
					mcx->match[mcx->num_matches].str = NULL;
					mcx->match[mcx->num_matches].len = 0;

				} else {
					i++;
				}
			}
		}
	}
	return ret;
}


static ptrdiff_t
match_prefix(const char *pattern, size_t pattern_len, const char *str)
{
	if (pattern == NULL) {
		return -1;
	}
	return mg_match_alternatives(pattern, pattern_len, str, NULL);
}


static ptrdiff_t
match_prefix_strlen(const char *pattern, const char *str)
{
	if (pattern == NULL) {
		return -1;
	}
	return mg_match_alternatives(pattern, strlen(pattern), str, NULL);
}

/* End of match.inl */
