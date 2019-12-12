#include <iostream>
#include <unistd.h>
#include <signal.h>
#include <cstdlib>
#include <cstdio>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/wait.h>
#include <sys/shm.h>
using namespace std;
//Compile command:g++ -o test -g <path> 
pid_t readbuf, writebuf;	//读进程和写进程

int CreateShmId(int id)//create share buf
    {
        key_t key;
        key = ftok(".", id);//alloc a key
        if(key < 0) 
            {
                cout << "Ftok failed!" << endl;
                exit(1);
            }
        int shmid = shmget(key, 200, IPC_CREAT | 0666);
        /*
        * @description: 得到一个共享内存标识符或创建一个共享内存对象
        * @param key:绑定的键值 
        * @param size:内存的大小
        * @param IPC_CREAT | 0666
        * @return: 返回共享内存的标识符/-1
        */
        if(shmid < 0)
            {
                cout<<"Failed"<<endl;
                exit(1);
            }
	return shmid;
    }

int GetShmId(int id)//获取共享内存的ID
    {
    key_t key;
    key = ftok("/home",id);
    if(key < 0) 
        {
            cout << "Ftok failed!" << endl;
            exit(1);
        }
    int shmid = shmget(key, 0, IPC_CREAT);
    if(shmid < 0)
        {
            printf("Shmget failed!\n");
            exit(1);
        }
    return shmid;
    }

void P(int semid, int index)  //P操作
    {
        struct sembuf sem;
        sem.sem_num = index;
        sem.sem_op = -1;
        sem.sem_flg = 0;
        semop(semid, &sem, 1);
        return;
    }

void V(int semid, int index)//V操作
    {
        struct sembuf sem;
        sem.sem_num = index;
        sem.sem_op = 1;
        sem.sem_flg = 0;
        semop(semid, &sem, 1);
        return;
    }

union semun
    {
    int val;
    struct semid_ds *buf;
    unsigned short  *array;
    };


int main(int argc,char *argv[]) 
{   
    if(argc!=3)
    {
        cout<<"Usage:input_file output_file"<<endl;
        return 0;
    }
    int flag;
    key_t key, key_sem;
    union semun arg;//Build Sign group
    unsigned short array[2]={0,0};
    arg.array = array;
    for(int i = 0; i < 5; i++) 
            int shmid = CreateShmId(i);	//Create share buf
    key_sem = ftok("./test", 7701);
    /*  
    key_t ftok(const char *pathname, int proj_id);
    generate ID
    */
    if(key_sem < 0) 
        {
        cout << "Ftok failed!" << endl;
        exit(1);
        }
    int semid = semget(key_sem, 2, IPC_CREAT | 0666);	//信号灯赋初值
    /**
    * @description: 创建一个新的信号量或获取一个已经存在的信号量的键值。
    * @param {指定键值,初始化信号量的个数,IPC_CREAT | 0666} 
    * @return: 成功返回信号量的标识码ID。失败返回-1；
    */
    if(semid < 0)
        {
    printf("Semget failed!\n");
    exit(1);
    }
    semctl(semid, 1, SETALL, arg);//arg是一个union
    /**
    * @description: 直接控制信号量信息
    * @param {type} 
    * @return: 
    */

    if((readbuf = fork()) < 0) //Crate Read process
        {
            cout << "Create readbuf failed!" << endl;
            exit(1);
        }
    else if(readbuf == 0) //sub read process
        {
            int i = 0, blocknum;
            int shmid;
            key_t key_sem = ftok(".", 2333);
            if(key_sem < 0) 
                {
                    cout << "Ftok failed!" << endl;
                    exit(1);
                }
            int semid = semget(key_sem, 0, IPC_CREAT);
            int fd = open(argv[1],O_RDONLY);	//open file by read only
            //To get number of blocks
            int filelen = lseek(fd, 0, SEEK_END);
            if(filelen % 200 == 0)	
                blocknum = filelen / 200;
            else
                blocknum = filelen / 200 + 1;
            for(int num = 0; num < blocknum; num++)//For each block
            {
                shmid = GetShmId(i);//to get id
                char *data = (char*)shmat(shmid, NULL, 0);
                /**
                * @description: 启动对该共享内存的访问，并把共享内存连接到当前进程的地址空间
                * @param {id,NULL,0} 
                * @return: 一个指向共享内存第一个字节的指针/-1
                */
                lseek(fd, num*200, SEEK_SET);//Re locate pointer
                int len =  read(fd, data, 200);
                i = (i+1) % 5;
                V(semid, 0);
                P(semid, 1);//release share memory
                kill(writebuf,SIGKILL);//killing process
                exit(0);
            }
        }

    if((writebuf = fork()) < 0) 
        {
            cout << "Create writebuf failed!" << endl;
            exit(1);
        }
    else if(writebuf == 0) 
        {
            int i = 0, blocknum = 0;
            int shmid, N = 3;
            key_t key_sem = ftok(".", 2333);
            if(key_sem < 0) 
                {
                    cout << "Ftok failed!" << endl;
                    exit(1);
                }
            int semid = semget(key_sem, 0, IPC_CREAT);
            int fd = open(argv[2], O_WRONLY|O_CREAT|O_TRUNC, 0666);	//write with cover
            while(1) 
                {
                    P(semid, 0);//Protect memory
                    shmid = GetShmId(i);
                    char *data = (char*)shmat(shmid, NULL, 0);
                    lseek(fd, blocknum*200, SEEK_SET);
                    int len = write(fd, data, strlen(data));
                    memset(data, '\0', 200);
                    i = (i+1) % 5;
                    blocknum++;
                    V(semid, 1);
                }
        }
    waitpid(writebuf, NULL, 0);//Wait...
    waitpid(readbuf, NULL, 0);
    for(int i = 0; i < 5; i++) 
        {
    int shmid = GetShmId(i);
    shmctl(shmid, IPC_RMID, 0);//Delete share memory
    }
    return 0;
}
