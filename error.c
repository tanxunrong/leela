
#include "leela.h"
#include "context.h"

void
leela_error(struct leela_context * context, const char *msg, ...) {
    static guint logger = 0;
    logger = leela_handle_findname("logger");
    if (logger == 0) {
        return;
    }
#define LEELA_LOG_SIZE 256
    char tmp[LEELA_LOG_SIZE];
    char *data = NULL;

    va_list ap;

    va_start(ap,msg);
    int len = vsnprintf(tmp, LEELA_LOG_SIZE, msg, ap);
    va_end(ap);
    if (len < LEELA_LOG_SIZE) {
        data = g_strdup(tmp);
    } else {
        int max_size = LEELA_LOG_SIZE;
        for (;;) {
            max_size *= 2;
            data = g_malloc0(max_size);
            va_start(ap,msg);
            len = vsnprintf(data, max_size, msg, ap);
            va_end(ap);
            if (len < max_size) {
                break;
            }
            g_free(data);
        }
    }


    struct leela_msg *smsg = g_malloc0(sizeof(*smsg));
    if (context == NULL) {
        smsg->source = 0;
    } else {
        smsg->source = leela_context_handle(context);
    }
    smsg->session = 0;
    smsg->data = data;
    smsg->sz = len | (PTYPE_TEXT << HANDLE_REMOTE_SHIFT);
    leela_context_push(logger, smsg);
}
