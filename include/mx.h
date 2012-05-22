#ifndef _H_MX
#define _H_MX

#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/nameser.h>
#include <resolv.h>
#include <netdb.h>

#define MX_DOMAIN_LEN MAXDNAME

typedef struct MXRecord {
	uint16_t priority;
	u_char name[MX_DOMAIN_LEN];
} mxrecord_t;

int resolve_mx(const char *dname, mxrecord_t *record);

int resolve_mxs(const char *dname, mxrecord_t **records, size_t *nrecords);
mxrecord_t *pick_mx(mxrecord_t *records, size_t nrecords);
#endif
