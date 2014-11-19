#include "../leela.h"

#include <stdio.h>
#include <stdlib.h>

struct logger {
	FILE * handle;
	int close;
};

struct logger *
logger_create(void) {
    struct logger * inst = g_malloc0(sizeof(*inst));
	inst->handle = NULL;
	inst->close = 0;
	return inst;
}

void
logger_release(struct logger * inst) {
	if (inst->close) {
		fclose(inst->handle);
	}
    g_free(inst);
}

static int
_logger(struct leela_context * context, gpointer ud, int type, int session,guint32 source, const gpointer msg, gsize sz) {
	struct logger * inst = ud;
	fprintf(inst->handle, "[:%08x] ",source);
	fwrite(msg, sz , 1, inst->handle);
	fprintf(inst->handle, "\n");
	fflush(inst->handle);

	return 0;
}

int
logger_init(struct logger * inst, struct leela_context *ctx, const char * parm) {
	if (parm) {
		inst->handle = fopen(parm,"w");
		if (inst->handle == NULL) {
			return 1;
		}
		inst->close = 1;
	} else {
		inst->handle = stdout;
	}
	if (inst->handle) {
        leela_context_callback(ctx, inst, _logger);
		leela_command(ctx, "REG", ".logger");
		return 0;
	}
	return 1;
}
