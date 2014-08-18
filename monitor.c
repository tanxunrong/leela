
#include "monitor.h"
#include "context.h"

/**
 * @brief leela_monitor_new
 * @return
 */
struct leela_monitor *leela_monitor_new()
{
    struct leela_monitor *ret = g_malloc0(sizeof(*ret));
    return ret;
}

/**
 * @brief leela_monitor_delete
 * @param m
 */
void leela_monitor_delete(struct leela_monitor *m)
{
    g_free(m);
}

/**
 * @brief leela_monitor_trigger
 * @param moniter
 * @param src
 * @param dest
 */
void leela_monitor_trigger(struct leela_monitor *monitor,guint src,guint dest)
{
    monitor->dest = dest;
    monitor->src = src;
    g_atomic_int_inc(&monitor->version);
}

/**
 * @brief leela_monitor_check
 * @param monitor
 */
void leela_monitor_check(struct leela_monitor *monitor)
{
    if (monitor->version == monitor->check_version)
    {
        if (monitor->dest)
        {
            leela_context_endless(monitor->dest);
            g_printerr("msg from %08x to %08x may be in endless loop",monitor->src,monitor->dest);
        }
    }
    else
    {
        monitor->check_version = monitor->version;
    }
}
