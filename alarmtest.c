 #include "types.h"
 #include "stat.h"
 #include "user.h"
volatile int count=0;
 void 
 handler(void)
 {
    count++;
    printf(1,"Alarm fired! count=%d\n",count);
    sigreturn();
 }
 int 
 main(void)
 {
    sigalarm(10,handler);
    // here interval is 10 as shown above
    while(count<3){
        //keep running 
    }
    sigalarm(0,handler);
    printf(1,"Alarm test finished!\n");
    exit();
 }
