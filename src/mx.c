#include <errno.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>

#include "mx.h"
#include "log.h"

struct MXRecord *
pick_mx(struct MXRecord *records, size_t nrecords)
{
	struct MXRecord *rec;
	uint16_t cur_min_pri = (1<<16) - 1;
	size_t cur_min_count = 0,
		   cur_min_pos = 0,
		   min_pos;
	register size_t i;

	for (rec = records, i = 0; i < nrecords; rec++, i++) {
		if (rec->priority < cur_min_pri) {
			cur_min_count = 1;
			cur_min_pos = i;
			cur_min_pri = rec->priority;
		} else if (rec->priority == cur_min_pri) {
			cur_min_count++;
		}
	}

	/* Now in cur_min_count we have amount of records with minimal
	 * value of priority (== highest priority)
	 */
	if (cur_min_count == 1) {
		return records + cur_min_pos;
	} else if (cur_min_count == 0) {
		return NULL;
	}

	min_pos = rand() % cur_min_count;
	cur_min_pos = 0;
	for (rec = records, i = 0; i < nrecords; rec++, i++) {
		if (rec->priority == cur_min_pri) {
			if (cur_min_pos == min_pos) {
				return rec;
			}
			cur_min_pos++;
		}
	}
	return NULL;
}

/** Resolving MX records for domain name
 *
 * Function allocates memory to keep records. It is callers responsibility
 * to free *records. nrecords will contain amount of resolved records.
 */
int resolve_mxs(const char *dname,
				struct MXRecord **records,
				size_t *nrecords)
{
	u_char *rdata;
	union {
		HEADER hdr;              /* defined in resolv.h */
		u_char buf[NS_PACKETSZ]; /* defined in arpa/nameser.h */
	} response;                  /* response buffers */

	int rlen,             /* buffer length */
		rrnum,
		rdlen,
		rrcount,
		r = 0;

	struct MXRecord mxbuf;

	ns_msg handle;  /* handle for response message */
	ns_rr rr;

	log_debug("Resolving MX records for %s", dname);

	rlen = res_query(
			   dname, ns_c_any, ns_t_mx,
			   (u_char *)&response, sizeof (response)
		   );

	/* Failed to do DNS-lookup. h_errno contains an error code */
	if(rlen < 0) {
		log_herror(dname);
		return -1;
	}

	/* Failed to initialize response parsing. Standard error (in errno) */
	if (ns_initparse(response.buf, rlen, &handle) < 0) {
		log_perror("Failed to parse DNS response");
		return -1;
	}

	rrcount = ns_msg_count(handle, ns_s_an);

	if (rrcount == 0) {
		log_error("Empty MX response");
		return -1;
	}

	/* Allocate memory for resolved MX-records */
	*records = (struct MXRecord *)malloc(rrcount*sizeof(struct MXRecord));
	if (*records == NULL) {
		return -ENOSPC;
	}
	*nrecords = rrcount;

	/* Starting at this point we must remember to free records before return.
	 * (this is why we doing `goto clear` instear `return`.
	 */

	for (rrnum = 0; rrnum < rrcount; rrnum++) {
		ns_parserr(&handle, ns_s_an, rrnum, &rr);
		rdata = (u_char *)ns_rr_rdata(rr);
		rdlen = ns_rr_rdlen(rr);
		if (rdlen < (size_t)NS_INT16SZ) {
			log_error("RR record format error");
			r = -1;
			goto clear;
		}

		/* Priority. */
		mxbuf.priority = ns_get16(rdata);
		rdata += NS_INT16SZ;

		/* Target. */
		if (dn_expand(
					ns_msg_base(handle), ns_msg_end(handle),
					rdata, (char *)mxbuf.name, MX_DOMAIN_LEN) < 0) {
			r = -ENOSPC;
			goto clear;
		}

		(*records)[rrnum] = mxbuf;
	}

	// Everything went fine -- don't need to free records
	return 0;
clear:
	if (records != NULL) {
		free(records);
	}
exit:
	return r;
}

int resolve_mx(const char *dname, struct MXRecord *record)
{
	int r = 0;
	struct MXRecord *records;
	struct MXRecord *picked_record;
	size_t nrecords;

	if ((r = resolve_mxs(dname, &records, &nrecords)) < 0) {
		return r;
	}

	picked_record = pick_mx(records, nrecords);

	if (picked_record == NULL)	{
		free(records);
		return -1;
	}

	*record = *picked_record;
	free(records);
	return 0;
}
