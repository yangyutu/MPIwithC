// gameoflife.c
// Name: yuguang yang
// JHED: yyang60

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "mpi.h"

#define ITERATIONS 64
#define GRID_WIDTH  256
#define DIM  16     // assume a square grid

int getneighborsum(int posx, int posy,int *data_self, int *data_recv_up, int *data_recv_down, int width){
  int sum=0;

  int idx,idy;

  int i, j;

  for (i = -1 ; i < 2; i ++){
    for(j = -1; j<2; j++){
	idx=posx+i;
	idy=(posy+j+DIM)%DIM;
	
//	if(posx== 1 && posy ==2) printf("for %d, %d, %d, %d\n", i,j,sum,data_self[posx*DIM+posy]);

	if(idx == -1) {sum=sum+data_recv_up[idy];}
	else if(idx == width) {sum=sum+data_recv_down[idy];}
	else{sum=sum+data_self[idx*DIM+idy]; }

}
}

	sum=sum-data_self[posx*DIM+posy];

//	printf("%d, %d, %d\n", posx,posy,sum);
	return sum;
}


int main ( int argc, char** argv ) {

  int global_grid[ 256 ] = {0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
    1,1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

  // MPI Standard variable
  int num_procs;
  int ID, j;
  int iters = 1;

  // Messaging variables
  MPI_Status stat;
  // TODO add other variables as necessary

  int i;
  int *data_self,*data_old;
  int *data_recv_up;
  int *data_recv_down;
  int *data_send_up;
  int *data_send_down;
  int *data_buffer;
  int data_size;
  int sum;

  // MPI Setup
  if ( MPI_Init( &argc, &argv ) != MPI_SUCCESS )
  {
    printf ( "MPI_Init error\n" );
  }

  MPI_Comm_size ( MPI_COMM_WORLD, &num_procs ); // Set the num_procs
  MPI_Comm_rank ( MPI_COMM_WORLD, &ID );

  assert ( DIM % num_procs == 0 );

  // TODO Setup your environment as necessary

 data_size = GRID_WIDTH/num_procs;
  data_self=(int *)malloc(data_size*sizeof(int));
 data_old=(int *)malloc(data_size*sizeof(int));
  data_buffer=(int *)malloc(data_size*sizeof(int));

  for (i = 0;i < data_size; i++){
    data_self[i] = global_grid [ data_size*ID + i];
  }
  data_recv_up=(int *)malloc(DIM*sizeof(int));
  data_recv_down=(int *)malloc(DIM*sizeof(int));
//	printf("size %lu\n", sizeof(int *));
  data_send_up=(int *)malloc(DIM*sizeof(int));
  data_send_down=(int *)malloc(DIM*sizeof(int));

//	printf("hello.here! 0\n");
  for ( iters = 0; iters < ITERATIONS; iters++ ) {
    // TODO: Add Code here or a function call to you MPI code
  for (i = 0; i < DIM ; i ++)
 {
  data_send_up[i]=data_self[i];
  data_send_down[i]=data_self[data_size-DIM+i];
  data_recv_up[i]=data_self[data_size-DIM+i];
  data_recv_down[i]=data_self[i];

 }

//	printf("hello.here! 1\n");
  // now every node send its data and receive data from nearby node
  if( num_procs >1) {
  if (ID%2 == 0){
  MPI_Send(data_send_up,DIM,MPI_INT, (ID-1+num_procs)%num_procs,1,MPI_COMM_WORLD);
  MPI_Send(data_send_down,DIM,MPI_INT, (ID+1+num_procs)%num_procs,1,MPI_COMM_WORLD); 
 
  MPI_Recv(data_recv_down,DIM,MPI_INT,(ID+1+num_procs)%num_procs,1,MPI_COMM_WORLD, &stat);
  MPI_Recv(data_recv_up,DIM,MPI_INT,(ID-1+num_procs)%num_procs,1,MPI_COMM_WORLD, &stat);

//	printf("size %lu\n",sizeof(data_recv_down));
 }else{

  MPI_Recv(data_recv_down,DIM,MPI_INT,(ID+1+num_procs)%num_procs,1,MPI_COMM_WORLD, &stat);
  MPI_Recv(data_recv_up,DIM,MPI_INT,(ID-1+num_procs)%num_procs,1,MPI_COMM_WORLD, &stat);


  MPI_Send(data_send_up,DIM,MPI_INT, (ID-1+num_procs)%num_procs,1,MPI_COMM_WORLD);
  MPI_Send(data_send_down,DIM,MPI_INT, (ID+1+num_procs)%num_procs,1,MPI_COMM_WORLD);  
}
 }



  for(i =0; i < data_size;i++) data_old[i]=data_self[i];
  for(i = 0; i< DIM/num_procs; i++){
    for(j = 0; j < DIM; j++){
	 sum= getneighborsum(i,j,data_old,data_recv_up,data_recv_down,DIM/num_procs);
	if(data_self[i*DIM+j] == 1){

	if(sum == 2 || sum==3) 	{data_self[i*DIM+j]=1;}
 	else{	data_self[i*DIM+j]=0;}

	}else
        {
	
	if(sum == 3)data_self[i*DIM+j]=1;	
	}
}}


    
//	printf("hello.here! 2\n");
//after updating, i will collect data from other nodes
    if (ID  != 0){
	MPI_Send(data_self,data_size,MPI_INT,0,0,MPI_COMM_WORLD);
	
    }else{

	for (i=1;i<num_procs;i++){
	MPI_Recv(data_buffer,data_size,MPI_INT,i,0,MPI_COMM_WORLD, &stat);
	
	for (j=0;j<data_size;j++) global_grid[i*data_size+j]=data_buffer[j];	
	}

   	for (j=0;j<data_size;j++) global_grid[j]=data_self[j]; 
    }

 
	
   // Output the updated grid state
    // FIXME: Feel free to print more iterations when you debug but only 
    //  submit with the 64th iteration printing and do not change the 
    //  format of the printed output.
    if ( ID == 0 && iters == ITERATIONS-1) {
      printf ( "\nIteration %d: final grid:\n", iters );
      for ( j = 0; j < GRID_WIDTH; j++ ) {
        if ( j % DIM == 0 ) {
          printf( "\n" );
        }
        printf ( "%d  ", global_grid[j] );
      }
      printf( "\n" );
    }
  }

  // TODO: Clean up memory
  MPI_Finalize(); // finalize so I can exit
}

