
#include "module.h"

#define LEELA_MAX_MODULE 32
struct leela_module_all {
    GMutex mtx;
    const gchar *path;
    guint num;
    struct leela_module modules[LEELA_MAX_MODULE];
};

static struct leela_module_all *GM = NULL;

/**
 * @brief leela_open_so
 * @param name
 * @return
 */
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
    }
    g_strfreev(div);

    return module;
}

/**
 * @brief leela_module_insert
 * @param module
 */
void leela_module_insert(struct leela_module *module)
{
    struct leela_module_all *m = GM;
    g_mutex_lock(&GM->mtx);

    for(int i=0;i<m->num;i++)
    {
        if (g_str_equal(m->modules[i].name,module->name))
        {
             g_error("module %s exist",module->name);
        }
    }
    g_assert(m->num < LEELA_MAX_MODULE);

    m->modules[m->num] = *module;
    m->num++;
    g_mutex_unlock(&GM->mtx);
}

/**
 * @brief leela_module_query
 * @param name
 * @return
 */
struct leela_module * leela_module_query(const char * name)
{
    struct leela_module_all *m = GM;

    gchar *module_name = g_strdup(name);
    g_strchomp(module_name);

    //find if existd
    for(int i=0;i<m->num;i++)
    {
        if (g_str_equal(m->modules[i].name,module_name))
        {
            return &(m->modules[i]);
        }
    }

    //add new module
    g_assert(m->num < LEELA_MAX_MODULE);
    g_mutex_lock(&m->mtx);
    GModule *so = leela_open_so(module_name);

    struct leela_module *module = NULL;
    if (so)
    {
        module = &(m->modules[m->num]);
        module->module = so;
        module->name = module_name;

        //need null terminate ... va_args
        gchar *createSymbol = g_strjoin(NULL,name,"_create",NULL);
        gchar *initSymbol = g_strjoin(NULL,name,"_init",NULL);
        gchar *releaseSymbol = g_strjoin(NULL,name,"_release",NULL);
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
        g_free(initSymbol);
        g_free(releaseSymbol);

        m->num++;
        module = module;
    }
    g_mutex_unlock(&m->mtx);
    return module;
}

/**
 * @brief leela_module_instance_create
 * @param module
 * @return
 */
gpointer
leela_module_instance_create(struct leela_module *module)
{
    if (module->create)
    {
        return module->create();
    }
    else
    {
        return (gpointer)(gintptr)(~0);
    }
}

/**
 * @brief leela_module_instance_release
 * @param module
 * @param inst
 */
void leela_module_instance_release(struct leela_module *module, void *inst)
{
    if (module->release)
    {
        module->release(inst);
    }
}

/**
 * @brief leela_module_instance_init
 * @param module
 * @param inst
 * @param ctx
 * @param parm
 * @return
 */
int
leela_module_instance_init(struct leela_module *module, gpointer inst, struct leela_context *ctx, const gchar * param)
{
    return module->init(inst,ctx,param);
}

/**
 * @brief leela_module_init
 * @param path
 */
void leela_module_init(const char *path)
{
    g_assert(GM == NULL);
    struct leela_module_all *m = g_malloc0(sizeof(*m));

    m->path = g_strdup(path);
    g_assert(strlen(m->path) != 0);

    g_mutex_init(&m->mtx);
    m->num = 0;
    GM = m;
}
