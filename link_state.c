#include "link_state.h"

double getcputime(void)        
{ 
	struct timeval tim;        
        struct rusage ru;        
        getrusage(RUSAGE_SELF, &ru);        
        tim=ru.ru_utime;        
        double t=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;
	printf("T: %lf\n",t); 
        tim=ru.ru_stime;        
        t+=(double)tim.tv_sec + (double)tim.tv_usec / 1000000.0;        
	printf("T+: %lf\n",t);
        return t; 
}

int main(int argc, char *argv[]){
	int i;
	clock_t start,end;
	double cpu_time_used;
	struct tms *buf,*buf1;
	total_nodes = -1;
	
	if(argc != 4){
		perror("Incorrect command line arguments\n");
		exit(-1);
	}
	file = fopen(argv[1],"r");
	initialize_topology();
//	print_topology();
	//start = clock();
	//struct tms buf;
	start = times(&buf);
	//s = getcputime();
	printf("Start : %.8lf\n",(double)start);
	link_state(atoi(argv[2]),atoi(argv[3]),0);
	for(i=0;i<total_nodes;i++)
	{
		free(node[i].edge_cost);
	}
	free(node);
	//struct tms buf1;
	end = times(&buf1);
	printf("End : %.8lf\n",(double)end);
	cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
	initialize_topology();
	link_state(atoi(argv[3]),atoi(argv[2]),1);
	printf("Time taken : %lf\n",cpu_time_used);

}	

void initialize_topology(){
	int i,j;
	if(!file){
		perror("Cannot Open File\n");
		exit(-1);
	}
	fseek(file,0,SEEK_SET);	
	fscanf(file,"%d\n",&total_nodes);
//	printf("%d\n",total_nodes);

	node = (Node *)malloc(sizeof(Node)*total_nodes);

	for(i = 0;i< total_nodes;i++){ //initialize node structure
		node[i].edge_cost = (double *)malloc(sizeof(double) * total_nodes);
		memset(node[i].edge_cost,0.0,sizeof(double)*total_nodes);
		node[i].pred = i;
		node[i].flag=0; //not yet permanent
		node[i].cost=INF;//start all nodes with infinite cost
	}
	
	//memset(path,-1,total_nodes*sizeof(int)); //reset path array

	int temp_i = -1,temp_j = -1;
	double temp_cost = 0.0f;
	while(fscanf(file,"%d %d %lf",&temp_i,&temp_j,&temp_cost)!= EOF){
		node[temp_i-1].edge_cost[temp_j-1] = temp_cost;
		node[temp_j-1].edge_cost[temp_i-1] = temp_cost;
	}
}

void print_topology()
{
	int i,j;
	for(i = 0;i < total_nodes; i++){
		printf("%d : ",i+1);
		for(j = 0;j < total_nodes; j++){
			if(node[i].edge_cost[j]!=0)
			printf("\t%d -> %.2lf",j+1, node[i].edge_cost[j]);
		}
		printf("\n");
	}
	
}

void link_state(int src, int dest, char flag){
	
	int i,k;
	double min;

	k=src-1;
        node[src-1].cost=0.0f;
        node[src-1].flag=1;

        do
        {
                for(i=0;i<total_nodes;i++)
                {
                        if(node[k].edge_cost[i]!=0 && node[i].flag == 0)
                        {
                                if(node[k].cost + node[k].edge_cost[i] < node[i].cost)  //update predecessor and cost to src node
                                {
                                        node[i].pred = k;
                                        node[i].cost = node[k].cost+node[k].edge_cost[i];
                                }
                        }

                }
                k=0,min=INF;
                //Find next min
                for(i=0;i<total_nodes;i++)
                {
                        if(node[i].flag ==0 && node[i].cost < min)
                        {
                                min = node[i].cost;
                                k=i;
                        }
                }
                node[k].flag=1;
        }while(k!=(dest-1));
	
	if(!flag)
	printf("\nLeast cost from source to destination is : %.2lf\n",node[dest-1].cost);

	printf("\nRouting table of source node %d -->\n\n",src);
	printf("  i\t  j\tCost\n");
	printf("------\t------\t----\n");
	
        for(k = 0;k < total_nodes; k++){
                       // if(node[i].edge_cost[k]!=0)
			if(node[k].cost!=999999)
                        printf("  %d\t  %d\t%.2lf\n",src,k+1,node[k].cost);
			else
			printf("  %d\t  %d\tINFINITY\n",src,k+1);
        }
	//printf("--------------------\n");
	/*
	printf("\nRouting table of destination node %d -->\n\n",dest);
        printf("Node 1\tNode 2\tCost\n");
        printf("------\t------\t----\n");
        
        for(k = 0;k < total_nodes; k++){
                       // if(node[i].edge_cost[k]!=0)
                        printf("  %d\t  %d\t%.2lf\n",dest,k+1,node[k].cost);
        }
	*/
	printf("\n\n");
}

