
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <pthread.h>
 //Note: Compile args:$gcc -g '/root/LAB/lab2.cpp'  -fpermissive -pthread -o ./ker
//ref: https://www.cnblogs.com/lixiaofei1987/p/3208414.html
union semun {
    int val; /* value for SETVAL */
    struct semid_ds *buf; /* buffer for IPC_STAT, IPC_SET */
    unsigned short *array; /* array for GETALL, SETALL */
    struct seminfo *__buf; /* buffer for IPC_INFO */
};

int i=0,sum=0;
int semid;//信号量集合首地址
pthread_t p1,p2;
union semun arg;
void P(int semid, int index);
void V(int semid, int index);

void P(int semid, int index){
	struct sembuf sem={index,-1,0};//index=array index 0=IPC_NOWAIT
/*
int  semop（int  semid，struct sembuf  *sops，size_t nsops)
@des:operate sign
@para:semid,opvalue,1: IPC_NOWAIT but not SEM_UNDO
*/
    semop(semid,&sem,1);
}

void V(int semid, int index){
    struct sembuf sem={index,1,0};
    semop(semid,&sem,1);
}

/*线程一*/      
void thread_1(void){  
    for(;i<=100;i++){ 
    P(semid,0);   
    printf("This is a pthread_1.\ni=%d\n",i); 
	//printf("Now Semi:%d\n",semid); 
    sum+=i;
    V(semid,1); 
    }
	return;  
}  

/*线程二*/  
void thread_2(void){    
    while(1){
    P(semid,1); 
    printf("This is a pthread_2.\nsum=%d\n",sum); 
//printf("Now Semi:%d\n",semid);
    if(i==101)return;
    V(semid,0);
    }
}  

int main(){
	int key ;//系统建立IPC通讯(消息队列、信号量和共享内存)时指定一个ID值
    int ret;
	key=ftok("/tmp", 0x66 ) ;
    if(key<0){
        perror("ftok key error") ;
        return -1 ;
    }
    
    semid=semget(key,2,IPC_CREAT|0666);//create sign;return intenger sign index
    if(semid==-1){
        perror("create error\n");
        return 0;
    }
    /***对0号信号量设置初始值***/
	arg.val=1;
    ret=semctl(semid,0,SETVAL,arg);
//set sign[0]=0
	arg.val=0;
	ret =semctl(semid,1,SETVAL,arg);//set sign[1]=1
    if (ret < 0 ){
        perror("ctl sem error");
        semctl(semid,0,IPC_RMID,arg);
        return -1 ;
    }
    
    ret=pthread_create(&p1,NULL, &thread_1,NULL);  
	/*
	@des:create a thread for a function
	@para:pointer of thread,NULL,&function,NULL
	@ret:status var
	*/ 
    if(ret!=0)  {  
        printf("Create pthread error!\n");  
        return -1;  
    }  
    
    ret=pthread_create(&p2,NULL,& thread_2,NULL);  
    if(ret!=0)  {  
        printf("Create pthread error!\n");  
        return -1;  
    }  

//WAIT....
    pthread_join(p1,NULL);  
    pthread_join(p2,NULL);  
//Del sign
    semctl(semid,0,IPC_RMID,arg);
    semctl(semid,1,IPC_RMID,arg);
    return 0;  
}
