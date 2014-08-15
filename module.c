
#include "module.h"

#define LEELA_MAX_MODULE 32
struct leela_module_all {
    GMutex mtx;
    const gchar *path;
    guint num;
    struct leela_module modules[LEELA_MAX_MODULE];
};

static struct leela_module_all *GM;

static GModule *
leela_open_so(const char *name)
{
    struct leela_module_all *m = GM;
    gchar *path = g_strdup(m->path);
    gchar **div = g_strsplit(path,";",0);
    guint sz = strlen(name);
    g_assert(sz != 0);
    g_free(path);

    g_assert(g_module_supported());
    GModule *module = NULL;

    if (div)
    {
        gchar **interator = div;
        gchar *realPath = NULL;

        ///@brief search for the module
        while(*interator != NULL && module == NULL)
        {
            path = *interator;
            int i = 0;
            while(*(path+i) != '\0')
            {
                if (*(path+i) == '?')
                {
                    realPath = g_malloc0(strlen(*interator) + sz + 1);
                    if (i > 0)
                    {
                        memcpy(realPath,path,i);
                    }
                    memcpy(realPath+i,name,sz);
                    if (strlen(path) - i - 1 > 0)
                    {
                        memcpy(realPath+i+sz,path+i+1,strlen(path) - i - 1);
                    }

                    module = g_module_open(realPath,G_MODULE_BIND_MASK);
                    if (module != NULL)
                    {
                        break;
                    }
                }
                i++;
            }
            interator++;
        }

        ///@brief free div
        interator = div;
        while(*interator != NULL)
        {
            g_free(*interator);
            interator++;
        }
        interator = NULL;
        g_free(div);

        return module;
    }

    return module;
}

void leela_module_insert(struct leela_module *mod);
struct leela_module * leela_module_query(const char * name)
{
    struct leela_module_all *m = GM;
    struct leela_module *module = NULL;
    gchar *module_name = g_strdup(name);
    g_strchomp(module_name);

    for(int i=0;i<m->num;i++)
    {
        if (g_str_equal(m->modules[i].name,module_name))
        {
            return &(m->modules[i]);
        }
    }
    g_assert(m->num < LEELA_MAX_MODULE);
    g_mutex_lock(&m->mtx);
    GModule *so = leela_open_so(name);
    if (so)
    {
        module = &(m->modules[m->num]);
        module->module = so;
        module->name = module_name;

        gchar *createSymbol = g_strjoin(NULL,name,"_create");
//        gchar *initSymbol = g_strjoin(NULL,g_strdup(name),"_init");
//        gchar *releaseSymbol = g_strjoin(NULL,g_strdup(name),"_release");
        gchar *initSymbol = "toy_init";
        gchar *releaseSymbol = "toy_release";
        if (!g_module_symbol(so,createSymbol,(gpointer *)&module->create))
        {
            g_error("create symbol empty for module %s",module_name);
        }
        if (!g_module_symbol(so,initSymbol,(gpointer *)&module->init))
        {
            g_error("init symbol empty for module %s",module_name);
        }
        if (!g_module_symbol(so,releaseSymbol,(gpointer *)&module->release))
        {
            g_error("release symbol empty for module %s",module_name);
        }
        g_free(createSymbol);
//        g_free(initSymbol);
//        g_free(releaseSymbol);

        m->num++;
        module = module;
    }
    g_mutex_unlock(&m->mtx);
    return module;
}

void * leela_module_instance_create(struct leela_module *);
int leela_module_instance_init(struct leela_module *, void * inst, struct leela_context *ctx, const char * parm);
void leela_module_instance_release(struct leela_module *, void *inst);

void leela_module_init(const char *path)
{
    g_assert(GM == NULL);
    struct leela_module_all *m = g_malloc0(sizeof(*m));
//    g_assert(g_ascii_isprint(path));

    m->path = g_strdup(path);
    g_assert(strlen(m->path) != 0);

    g_mutex_init(&m->mtx);
    m->num = 0;
    GM = m;
}
