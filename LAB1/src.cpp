/*
 * @Description:Lab1
 * @Date: 2019-10-28 21:27:27
 * @Author: I-Hsien
 * @LastEditors: I-Hsien
 * @LastEditTime: 2019-10-28 22:50:17
 */
#include<signal.h>
#include<sys/types.h>
#include<sys/stat.h>
#include <sys/wait.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<assert.h>
#include<string.h>
//See:https://blog.csdn.net/dlutbrucezhang/article/details/8821690
//See Also: https://blog.csdn.net/IT_flying625/article/details/89554946
int filedes[2];//File Description:[0] is read,[1] is write
int pid1=0;
int pid2=0;//Global var pid
void signpass(int sign);
void clean(int sign);
void writepipe(int writehandle);
void readpipe(int readhandle);
int main()
{
    printf("Process on...\n");
    assert (pipe(filedes)>=0);//anonymous pipe; return 0 is succeed and -1 is failed.
    /*
        signal(SIGN,function);
        @Description:Setting signal processing method.
        @Para:SIGN:defined signal value in the header;function:0:Ignore;1:Killing self;Other:Running Pre-Defined function
        @Return:None
        SIGINT's value is 2,trigger when keyboard interrupt.
    */
    signal(SIGINT,signpass);//Setting signal processing method.
    //Fork process
    assert ((pid1=fork())>=0);
    if(pid1==0)//Sub process1
    {
        signal(SIGINT,SIG_IGN);//Ignore Keyboard Interrupt
        signal(SIGUSR1,clean);//SIGUSR is reserved for the user;When receive this signal,run clean
        close(filedes[1]);//stop writing
        readpipe(filedes[0]);//read from process2
    }
    else
    {
        assert((pid2=fork())>=0);
        if (pid2==0)//Sub process2
        {
            signal(SIGINT,SIG_IGN);
            signal(SIGUSR1,clean);//Clean process2
            close(filedes[0]);//Stop reading
            writepipe(filedes[1]);
        }
    }
    waitpid(pid1,NULL,0);
    printf("Process %d stopped\n",pid1);
    waitpid(pid2,NULL,0);//Waiting until all sub processes finish
    printf("Process %d stopped\n",pid2);
    return 0;
}


void signpass(int sign)
//To pass end sign to process 1,2
{
/* kill(pid,SIGN);
kill()可以用来送参数sig 指定的信号给参数pid 指定的进程。参数pid 有几种情况：
1、pid>0 将信号传给进程识别码为pid 的进程.
2、pid=0 将信号传给和目前进程相同进程组的所有进程
3、pid=-1 将信号广播传送给系统内所有的进程
4、pid<0 将信号传给进程组识别码为pid 绝对值的所有进程参数 sig
*/
    kill(pid1,SIGUSR1);
    kill(pid2,SIGUSR1);
}
void clean(int sign)
{
    close(filedes[0]);
    close(filedes[1]);//Close Pipe
    if(pid1==0)
    {
    printf("Sub1 end!\n");
    exit(0);
    }

    if(pid2==0)
    {
    printf("Sub2 end!\n");
    exit(0);
    }
}

void writepipe(int writehandle)
{
	int count =0;
    char buf[200]="kksk";
    while(1)
    {
        printf("%s,I send you %d times\n",buf,count);
        printf("write: %s.\n",buf);
        buf[strlen(buf)] = '\0';//Add end sign
        write(filedes[1],buf,strlen(buf));//Write msg to pipe
	count++;
        sleep(1);
    }
    exit(0);
}
void readpipe(int readhandle)
{
    char buf[50];
	memset(buf,0,sizeof(buf));
	//printf("%d\n",(int)strlen(buf));
    int n=0;
    while(1)
    {
        n=read(filedes[0],buf,sizeof(buf));//Return read size
		printf("buf=%s",buf);
        buf[n]='\0';//add tail
        printf("%d bytes read: %s.\n",n,buf);
        sleep(1);
    }
exit(0);
}

