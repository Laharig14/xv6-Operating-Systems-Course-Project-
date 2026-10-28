  #include "types.h"
 #include "stat.h"
 #include "user.h"
volatile int count=0;
 void 
 handler(void)
 {
    count++;
    printf(1,"Alarm fired!\n");
    sigreturn();
 }
 int 
 main(void)
 {
    sigalarm(5,handler);
    // here interval is 5 as shown above
    while(count<1);
    sigalarm(0,handler);
    int old_count=count;
    for(volatile int i=0;i<100000000;i++);
    if(count==old_count)
    printf(1,"PASS: Alarm disabled\n");
    else
    printf(1,"FAIL: Alarm fired after disabling\n");
    exit();
 }
