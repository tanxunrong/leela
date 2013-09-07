
#include "myc.h"
#include <glib.h>

int main(int argc,char* argv[])
{

    return 0;
}
/*
//gatomic
volatile int tom = 22;
g_atomic_int_add (&tom,33);
g_atomic_int_set(&tom,56);
int tryTimes = 0;
CompareAndSet:
tryTimes++;
if ( g_atomic_int_compare_and_exchange(&tom,56,79) == FALSE)
{
    if(tryTimes < 5)
        goto CompareAndSet;
    else
        fputs("compare and exchange fail",stderr);
}

printf("%d \n",tom);

//glist
gint comp(gconstpointer a,gconstpointer b)
{
    if(a == NULL || b == NULL)
        err_quit("invalid ptr");
//    gint *aa = g_slice_dup(gint,a);
//    gint *bb = g_slice_dup(gint,b);
    return *((gint *)a) - *((gint *)b);
}
void listall(gpointer data,gpointer *userdata)
{
    printf("%d ",*((gint *)data));
    fflush(stdout);
}

    GList *cha = NULL;
        srand(time(NULL));

        for(gint i=0;i<10;i++)
        {
            gint *sand=g_slice_new0(gint);
            *sand = rand() % 1000;
            printf("%d ",*sand);
            cha = g_list_prepend(cha,(gpointer)sand);
        }
        NEWLINE;
        printf("list length %d\n",g_list_length(cha));
        g_list_foreach(cha,(GFunc)listall,NULL);
        NEWLINE;
        printf("%d \n",*((gint *)g_list_nth_data(cha,3)));
        cha = g_list_sort(cha,(GCompareFunc)comp);
        g_list_foreach(cha,(GFunc)listall,NULL);
        NEWLINE;
*/
