/* Sort function. */
/* from https://github.com/bel2125/sort_r */

static void
mg_sort(void *data,
        size_t elemcount,
        size_t elemsize,
        int (*compfunc)(const void *data1, const void *data2, void *userarg),
        void *userarg)
{
	/* We cannot use qsort_r here. For a detailed reason, see
	 * https://github.com/civetweb/civetweb/issues/1048#issuecomment-1047093014
	 * https://stackoverflow.com/questions/39560773/different-declarations-of-qsort-r-on-mac-and-linux
	 */

	/* We use ShellSort here with this gap sequence: https://oeis.org/A102549 */
	size_t A102549[9] = {1, 4, 10, 23, 57, 132, 301, 701, 1750};
	size_t gap, i, j, k;
	int Aidx;
	void *tmp = alloca(elemsize);

	for (Aidx = 8; Aidx >= 0; Aidx--) {
		gap = A102549[Aidx];
		if (gap > (elemcount / 2)) {
			continue;
		}
		for (i = 0; i < gap; i++) {
			for (j = i; j < elemcount; j += gap) {
				memcpy(tmp, (void *)((size_t)data + elemsize * j), elemsize);

				for (k = j; k >= gap; k -= gap) {
					void *cmp = (void *)((size_t)data + elemsize * (k - gap));
					int cmpres = compfunc(cmp, tmp, userarg);
					if (cmpres > 0) {
						memcpy((void *)((size_t)data + elemsize * k),
						       cmp,
						       elemsize);
					} else {
						break;
					}
				}
				memcpy((void *)((size_t)data + elemsize * k), tmp, elemsize);
			}
		}
	}
}

/* end if sort.inl */
