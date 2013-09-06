
#include "myc.h"
#include <glib.h>

int main(int argc,char* argv[])
{
    /*gatomic*/
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

    return 0;
}
