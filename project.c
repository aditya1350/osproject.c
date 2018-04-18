#include<stdio.h>
#include<stdlib.h>


int *burst;								//temp burst time storage
int *q1;
int *q2;

int pc;							//total process count
int queue1_size=-1;
int queue2_size=-1;
int queue2_pc=0;


struct q1_process{
int pid;
int a_time;
int b_time;
int priority;
int wait_time;
int turnaround_time;
}*p1;


void insert_first(int pro_id)
{
	q1[++queue1_size]=pro_id;
}

void insert_sec(int pro_id)
{
	q2[++queue2_size]=pro_id;
}

void delete1(int pro_id)
{
	int i;
	for(i=0;i<=queue1_size;i++)
	{
		if(q1[i]==pro_id)
		break;
	}
	for(int j=i;j<=queue1_size-1;j++)
	{
		q1[j]=q1[j+1];
	}
	queue1_size--;
}

void delete2()
{
	int j;
	for(int j=0;j<=queue2_size-1;j++)
	{
		q2[j]=q2[j+1];
	}
	queue2_size--;
}

void wait(int pro_id)
{
	for(int i=0;i<=queue1_size;i++)
	{
		if(pro_id!=q1[i])
		{
			p1[q1[i]-1].wait_time++;			//incrementing wait time of processes in the queue.
		}
	}
	for(int j=0;j<=queue2_size;j++)
	{
		p1[q2[j]-1].wait_time++;
	}	
}

void wait2()
{
	for(int i=0;i<=queue2_size;i++)
	{	
		p1[q2[i]-1].wait_time+=1;			//incrementing wait time of processes in the queue.
	}
}

int process_arrival(int time)
{
	int *id=(int*)malloc(sizeof(int)*pc);	//array to store processes arriving at same time
	int pos=-1;
	for(int i=0;i<pc;i++)
	{
		if(p1[i].a_time==time)
		{
			id[++pos]=p1[i].pid;
			insert_first(id[pos]);
		}
	}
	if(pos==-1)
	{						//no process
		return 0;
	}				
	else if(pos==0)						//only one process
	{	
		return id[pos];
	}
	else							//more than one process at given time
	{
		int max_p=id[0];
		for(int i=1;i<=pos;i++)
		{
			if(p1[id[i]-1].priority<p1[max_p-1].priority)		//checking priority
			{
				max_p=id[i];
			}
		}
		return max_p;
	}
}

int process_arrival2(int time)
{
	int i=0;
	int id=0;
	for(;i<pc;i++)
	{
		if(time==p1[i].a_time)
		{
			id=p1[i].pid;
		}
	}
	return id;
}

int allocate_p()
{
	if(queue1_size==0)			//if only one process in queue
	return q1[0];
	else
	{
		int max_p=q1[0];
		for(int i=1;i<=queue1_size;i++)
		{
			if(p1[q1[i]-1].priority<p1[max_p-1].priority)
			{
				max_p=q1[i];
			}
			else if(p1[q1[i]-1].priority==p1[max_p-1].priority)		//checking arrival time of equal priority processes
			{
				if(p1[q1[i]-1].a_time<p1[max_p-1].a_time)
				{
					max_p=q1[i];
				}
			}	
		}
		return max_p;
	}
}

int srtf_algo(int time,int flag)
{
	int time_q=2;
	int counter=0;
	int run_id=0;
	static int finish_process=0;
	while(finish_process!=queue2_pc)
	{
		if(flag==1)
		{
			int new_id=process_arrival2(time);
			if(new_id!=0)
			{
				if(run_id!=0)
				{
					insert_sec(run_id);
				}
				return time;
			}
		}
		if(run_id==0)
		{
			run_id=q2[0];
			delete2();
		}
		time+=1;
		burst[run_id-1]-=1;
		wait2();
		counter+=1;
		if(burst[run_id-1]==0)
		{
			finish_process++;
			run_id=0;
			counter=0;
		}
		else if(counter==time_q)
		{
			insert_first(run_id);
			counter=0;
			run_id=0;
		}	
	}
	return time;
}

int preemptive()
{
	int temp_pc=pc;
	int finished_process=0;
	int time=0;
	int run_id=0;
	int new_id;
	while(finished_process!=temp_pc)
	{
		new_id=process_arrival(time);					//process arriving at current time
		if(new_id!=0)
		{
			if(run_id!=0)
			{
				if(p1[new_id-1].priority<p1[run_id-1].priority)		//checking priority
				{
					delete1(run_id);
					insert_sec(run_id);			//swap into queue2
					queue2_pc++;
					run_id=new_id;
					temp_pc--;
				}
			}
			else
			{
				run_id=new_id;					//first running process or no process was running before
			}
		}
		else
		{
			if(queue2_size>-1 &&queue1_size==-1)
			{
				time=srtf_algo(time,1);
				continue;
			}
		}
		if(queue1_size>-1)						//if queue1 not empty
		{
			wait(run_id);
			burst[run_id-1]=burst[run_id-1]-1;
			if(burst[run_id-1]==0)					//if a process is finished
			{
				delete1(run_id);
				finished_process++;
				run_id=0;					//resetting running process id to NULL
				if(queue1_size!=-1)
				{
					run_id=allocate_p();				//select new process from queue1 if not empty
				}
			}
			
		}
		time++;		
	}
	return time;
}

void display()
{
	int time=preemptive();
	float avg_wait_time,avg_turnaround_time;
	int wait_s=0,turn_s=0;
	if(queue2_size!=-1)
		time=srtf_algo(time,0);
	printf("\nPID\tPriority\tArrival Time\tBurst Time\tWaiting Time\tTurnaround Time");
	for(int i=0;i<pc;i++)
	{
		p1[i].turnaround_time=p1[i].b_time+p1[i].wait_time;	
		printf("\n%-8d%-16d%-16d%-16d%-16d%d",p1[i].pid,p1[i].priority,p1[i].a_time,p1[i].b_time,p1[i].wait_time,p1[i].turnaround_time);
	}
	printf("\n\nTotal CPU time used(including idle time):%d",time);
	for(int i=0;i<pc;i++)
	{
		wait_s+=p1[i].wait_time;
		turn_s+=p1[i].turnaround_time;
	}
	avg_wait_time=wait_s*1.0/pc;
	avg_turnaround_time=turn_s*1.0/pc;	
	printf("\nAverage Waiting time:%.3f",avg_wait_time);
	printf("\nAverage Turnaround time:%.3f",avg_turnaround_time);
	printf("\n");
}
int main()
{
	int temp;
	while(1)
	{
		printf("Enter total Processes:");
		scanf("%d",&temp);
		if(temp>0)
		{
			pc=temp;
			break;
		}
		else
		{
			printf("Number should be greater than 0\n\n");
		}
	}
	p1=(struct q1_process*)malloc(sizeof(struct q1_process)*pc);
	q1=(int*)malloc(sizeof(int)*pc);
	q2=(int*)malloc(sizeof(int)*(pc-1));
	burst=(int*)malloc(sizeof(int)*pc);	
	printf("\n\t\tProcessor's Detail'\n");
	for(int i=0;i<pc;i++)
	{
		p1[i].pid=i+1;
		while(1)
		{
			printf("\nArrival time of Process %d:",i+1);
			scanf("%d",&temp);
			if(temp>=0)
			{
				p1[i].a_time=temp;
				break;
			}
			else
			{
				printf("Number should be greator than 0\n");
			}
		}
		while(1)
		{
			printf("Burst Time%d:",i+1);
			scanf("%d",&temp);
			if(temp>0 && temp%2==0)
			{
				p1[i].b_time=temp;
				break;
			}
			else
			{
				printf("Error:Enter positive number only\n\n");
			}
		}
		burst[i]=p1[i].b_time;
		while(1)
		{
			printf("Priority of Process",i+1);
			scanf("%d",&temp);
			if(temp>=0)
			{
				p1[i].priority=temp;
				break;
			}
			else
			{
				printf("Positive only\n\n");
			}
		}
		p1[i].wait_time=0;
	}
	display();
	free(p1);
	free(q1);
	free(burst);
	free(q2);	
}
