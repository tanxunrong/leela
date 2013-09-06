
#include "myc.h"
#include <glib.h>

//gint *compareFunc(gconstpointer a,gconstpointer b)
//{
//    if (a == NULL || b == NULL)
//        err_quit ("invalid pointer");
//    int com = (int)*a-(int)*b;
//    return (gint *)&com;
//}

int main(int argc,char* argv[])
{
    /*gatomic*/
    volatile tom = 22;
    g_atomic_int_add (&tom,33);
    printf("%d \n",tom);

    /*glist*/
    struct GList *chain = NULL;
    srand(time(NULL));
        int sand = rand();
        printf("%d ",sand);
        g_list_append(chain,(gpointer *)&sand);
//    printf("\n");
//    g_list_sort (chain,compareFunc);
//    for(int j=0;j++;j<g_list_length(chain))
//    {
//        printf("%d ",(int)*g_list_nth_data (chain,j));
//    }
    return 0;
}
