#include <mruby.h>
#include <mruby/variable.h>
#include <mruby/error.h>
#include "leela.h"

/*
static int
_cb(struct leela_context * context, void * ud, int type, int session, uint32_t source, const void * msg, size_t sz) {

	return 0;
}
*/
//Leela.error
/*
static mrb_value
mrb_leela_error(mrb_state *mrb, mrb_value self)
{
	mrb_sym ctxSym = mrb_intern_cstr(mrb,"@@CTX");
	mrb_value ctxV = mrb_const_get(mrb,self,ctxSym);
	struct leela_context *ctx = mrb_cptr(ctxV);
	if (ctx == NULL)
	{
		mrb_sys_fail(mrb,"fail to get ctx");
	}
	const char *err = NULL;
	mrb_get_args(mrb,"z",&err);
	leela_error(ctx,"%s",err);
	return mrb_nil_value();
}

//Leela.harbor
static mrb_value
mrb_leela_harbor(mrb_state *mrb, mrb_value self)
{
	mrb_sym ctxSym = mrb_intern_cstr(mrb,"@@CTX");
	mrb_value ctxV = mrb_const_get(mrb,self,ctxSym);
	struct leela_context *ctx = mrb_cptr(ctxV);
	if (ctx == NULL)
	{
		mrb_sys_fail(mrb,"fail to get ctx");
	}

}

//Leela.genid
static mrb_value
mrb_leela_genid(mrb_state *mrb, mrb_value self)
{
	mrb_sym ctxSym = mrb_intern_cstr(mrb,"@@CTX");
	mrb_value ctxV = mrb_const_get(mrb,self,ctxSym);
	struct leela_context *ctx = mrb_cptr(ctxV);
	if (ctx == NULL)
	{
		mrb_sys_fail(mrb,"fail to get ctx");
	}

	mrb_int session = leela_send(ctx,0,0,PTYPE_TAG_ALLOCSESSION,0,NULL,0);
	return mrb_fixnum_value(session);
}
*/

//Leela.callback
static mrb_value
mrb_leela_callback(mrb_state *mrb, mrb_value self)
{
	mrb_sym ctxSym = mrb_intern_cstr(mrb,"@@CTX");
	mrb_value ctxV = mrb_const_get(mrb,self,ctxSym);
	struct leela_context *ctx = mrb_cptr(ctxV);
	if (ctx == NULL)
	{
		mrb_sys_fail(mrb,"fail to get ctx");
	}

	//leela_callback(ctx,_cb);
	return self;
}

//Leela.command
static mrb_value
mrb_leela_command(mrb_state *mrb,mrb_value self)
{
	mrb_sym ctxSym = mrb_intern_cstr(mrb,"@@CTX");
	mrb_value ctxV = mrb_const_get(mrb,self,ctxSym);
	struct leela_context *ctx = mrb_cptr(ctxV);
	if (ctx == NULL)
	{
		mrb_sys_fail(mrb,"fail to get ctx");
	}

	const char *ret = NULL;
	const char *cmd = NULL,*param = NULL;
	mrb_get_args(mrb,"z|z",&cmd,&param);
	ret = leela_command(ctx,cmd,param);
	if (ret)
	{
		return mrb_str_new_cstr(mrb,ret);
	}
	return mrb_nil_value();
}


void
mrb_mruby_leela_gem_init(mrb_state* mrb) {
  struct RClass *leela;
  leela = mrb_define_class(mrb,"Leela",mrb->object_class);
  mrb_define_class_method(mrb, leela, "callback", mrb_leela_callback, MRB_ARGS_NONE());
  mrb_define_class_method(mrb, leela, "command", mrb_leela_command, MRB_ARGS_ARG(1,1));
 // mrb_define_class_method(mrb, leela, "genid", mrb_leela_genid, MRB_ARGS_NONE());
 // mrb_define_class_method(mrb, leela, "error", mrb_leela_error, MRB_ARGS_ARG(1,0));
}

void
mrb_mruby_leela_gem_final(mrb_state* mrb) {
  /* finalizer */
}
