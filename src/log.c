#include <malloc.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <netdb.h>

#include "log.h"

#define FILE_PATH_LEN 256

struct LogMessage {
	level_t level;
	char file[FILE_PATH_LEN];
	size_t linenum;
	pthread_t thread_id;
	int is_perror;
	int is_herror;
	int errno_val;
	char msg[MAX_MSG_LEN];
};

// Global configuration
static int			_level;
static FILE *		_stream;
static int			_is_configured;
static int			_need_fclose;

static const char *strlevel(level_t level);
static void _log_message(struct LogMessage *msg);

/** Main internal logging procedure.
 *
 * Used by macro `log`.
 *
 *  \param level -- logging level;
 *  \param format -- `printf`-like format template.
 */
void
log_msg(
	level_t level,
	const char *file,
	size_t linenum,
	int is_perror,
	int is_herror,
	const char *format, ...)
{
	struct LogMessage msg;
	va_list ap;

	if (!_is_configured || (level < _level)) {
		return;
	}

	va_start(ap, format);
	vsnprintf(msg.msg, MAX_MSG_LEN, format, ap);
	va_end(ap);

	msg.level = level;
	strcpy(msg.file, file);
	msg.linenum = linenum;
	msg.thread_id = pthread_self();
	msg.is_perror = is_perror;
	msg.is_herror = is_herror;

	if (msg.is_perror) {
		msg.errno_val = errno;
	} else if (msg.is_herror) {
		msg.errno_val = h_errno;
	}

	_log_message(&msg);
}

/* Before calling `log_configure` all calls to `log()` are ignored. */
void
log_configure(level_t level, FILE *stream)
{
	_level = level;

	if (stream == NULL) {
		_stream = stderr;
	} else {
		_stream = stream;
	}

	_is_configured = 1;
}

static void
_log_message(struct LogMessage *msg)
{
	flockfile(_stream);

	fprintf(_stream,
			"[%05lu] [%s]\t%s:%d\t%s",
			msg->thread_id,
			strlevel(msg->level),
			msg->file,
			msg->linenum,
			msg->msg);

	if (msg->is_perror) {
		fprintf(_stream, " [ERRNO: %s]\n", strerror(msg->errno_val));
	} else if (msg->is_herror) {
		fprintf(_stream, " [H_ERRNO: %d]\n", msg->errno_val);
	} else {
		fputc('\n', _stream);
	}

	funlockfile(_stream);
}

static const char *
strlevel(level_t level)
{
	switch (level) {
	case LOG_ERROR:
		return "ERROR";
	case LOG_WARNING:
		return "WARN";
	case LOG_INFO:
		return "INFO";
	case LOG_DEBUG:
		return "DEBUG";
	default:
		return "CUSTOM";
	}
}
