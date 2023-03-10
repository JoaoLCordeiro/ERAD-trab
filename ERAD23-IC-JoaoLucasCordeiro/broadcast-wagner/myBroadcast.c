#include <mpi.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "chrono.c"
#include <assert.h>

static int wz_debug = 0;
//#include "WZ_debug_MPI_send.c" 

#define USE_MPI_Bcast 1  // do NOT change
#define USE_my_Bcast 2   // do NOT change
//choose either BCAST_TYPE in the defines bellow
//#define BCAST_TYPE USE_MPI_Bcast
#define BCAST_TYPE USE_my_Bcast

long nmsg;       // o número total de mensagens
long tmsg;       // o tamanho de cada mensagem
int nproc;      // o número de processos MPI
int raiz;       // maquina que ira enviar as mensagens
int processId; 	// rank dos processos
int ni;			// tamanho do vetor contendo as mensagens

chronometer_t myBroadcastChrono;

//#define DEBUG 1
#define DEBUG 0

const int SEED = 100;

// mpirun -np 8 --hostfile hostfile.txt ./PingPongMPI 2 32 8

#define LOGIC_RANK( my_rank, root, comm_size ) \
        (( my_rank + comm_size - root ) % comm_size)
#define PHYSIC_RANK( logic_rank, root, comm_size ) \
        (( logic_rank + root ) % comm_size)        

// int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype, int root, MPI_Comm comm)
void my_Bcast(long int *buf, int count, MPI_Datatype datatype, int root, MPI_Comm comm){

	int np;    // number of participants in each node
    int comm_size;
    int my_rank;   // my_phisical rank
    int my_logic_rank;
    int rc;
        
	MPI_Status stat;
	MPI_Request request;
	    
    int orig, dest;
	static int phase;
		  
    MPI_Comm_size( comm, &comm_size );
    MPI_Comm_rank( comm, &my_rank );
    // my_logic_rank = ( my_rank + comm_size - root ) % comm_size;
    my_logic_rank = LOGIC_RANK( my_rank, root, comm_size );
    
    //fprintf( stderr, "host: %d my logic rank is %d\n", my_rank, my_logic_rank );
	
/*	static int once = 1;
	if( once ) {
	  fprintf( stderr, "host: %d my_Bcast num_fases=%d\n", processId, num_fases );
	}  
*/

    if(my_rank != root)
		rc = MPI_Recv(buf, ni, datatype, MPI_ANY_SOURCE, 0, comm, &stat);

    static int _once_ = 0; //1;
    phase = 1;

	//for(int np=1; 2*np <= comm_size; np *= 2 ){
	for(int np=1; np < comm_size; np *= 2 ){	
		
		//fprintf( stderr, "------------------------------ phase %d np=%d\n", phase, np );	
		if( my_logic_rank < np && my_logic_rank+np < comm_size ) {
			//if( my_logic_rank < np && my_logic_rank+np <= comm_size ) {
		    dest = (my_rank+np)%comm_size; 
		    //dest = PHYSIC_RANK( my_logic_rank+np, root, comm_size );
			//rc = MPI_Isend(buf, ni, datatype, dest, 0, comm, &request );
		
			rc = MPI_Send(buf, ni, datatype, dest, 0, comm );
			if( _once_ ){
		    	fprintf( stderr, "host: %d sending to %d on phase %d\n", my_rank, dest, phase );
		    	_once_ = 0; 
		    }  
		}

		/*
		if( my_logic_rank < np && my_logic_rank <= comm_size ) {
		     dest = (my_rank+np)%comm_size; 
		     //rc = MPI_Isend(buf, ni, datatype, dest, 0, comm, &request );
		rc = MPI_Send(buf, ni, datatype, dest, 0, comm );
		if( _once_ ) {
		       fprintf( stderr, "host: %d sending to %d on phase %d\n", my_rank, dest, phase );
		       _once_ = 0; 
		     }  
		}
		*/

		phase++;
    }
	//once = 0; 
	_once_ = 0;
}

void verifica_my_Bcast( void *buffer, int count, MPI_Datatype datatype,
                        int root, MPI_Comm comm )
{
    int comm_size;
    int my_rank;
    
    MPI_Comm_size( comm, &comm_size );
    MPI_Comm_rank( comm, &my_rank );
    static long *buff = (long *) calloc( count*comm_size, sizeof(long) );
    
    
    // preenche a faixa do raiz com alguma coisa (apenas no raiz)
    if( my_rank == root )
       for( int i=0; i<count; i++ )
          buff[ i ] = i+SEED;
    
    #if BCAST_TYPE == USE_MPI_Bcast
       MPI_Bcast( buff, count, datatype, root, comm );
    #elif BCAST_TYPE == USE_my_Bcast
       my_Bcast( buff, count, datatype, root, comm );
    #else
       assert( BCAST_TYPE == USE_MPI_Bcast || BCAST_TYPE == USE_my_Bcast );
    #endif   
	   
    
    // cada nodo verifica se sua faixa recebeu adequadamente o conteudo
    int ok=1;
    int i;
    for( i=0; i<count; i++ )
       if( buff[ i ] != i+SEED ) {
          ok = 0;
          break;
       }
    // imprime na tela se OK ou nao
    if( ok )
        fprintf( stderr, "MY BCAST VERIF: node %d received ok\n", my_rank );
    else
        fprintf( stderr, "MY BCAST VERIF: node %d NOT ok! local position: %d contains %ld\n",
                           my_rank, i, buff[i] );
      
}


int main(int argc, char *argv[]){

	//wz_debug = 1;    // DEBUG_MPI_MESSAGES


	raiz = 0;

	if (argc < 4){
		printf("usage: mpirun -np <np> %s <nmsg> <tmsg> <nproc> (-r <r>)\n",
			   argv[0]);
		return 0;
	}
	else{
        nmsg = atoi(argv[1]);
		tmsg = atoi(argv[2]);
		if (tmsg % 8 != 0){
			printf("usage: mpirun -np <np> %s <nmsg> <tmsg> <nproc> (-r <r>)\n",
			   argv[0]);
			printf("<tmsg> deve ser multiplo de 8\n");
			return 0;
		}
        nproc = atoi(argv[3]);
        if(argc == 6){
            if (strcmp(argv[4], "-r") == 0)
                raiz = atoi(argv[5]);
        }
	}

	ni = tmsg/8;
	MPI_Status Stat;

	long int *inmsg = (long int*)calloc(ni, sizeof(long int));

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nproc);
	MPI_Comm_rank(MPI_COMM_WORLD, &processId);
	
	if(processId == 0){
	   #if BCAST_TYPE == USE_MPI_Bcast
 	       fprintf( stderr, "\n****** USING MPI_Bcast ******\n" );
	   #elif BCAST_TYPE == USE_my_Bcast
	       fprintf( stderr, "\n****** USING my_Bcast ******\n" );
	   #else
	       assert( BCAST_TYPE == USE_MPI_Bcast || BCAST_TYPE == USE_my_Bcast );
	   #endif    
	}
	
	// check my macros
	if(processId == 0){
	   fprintf( stderr, "\n---------ranks-------\n" );
	   for( int phys=0; phys<nproc; phys++ )
	        fprintf( stderr, "%d ", phys );
	   fprintf( stderr, " PHYSIC\n" );
	   for( int phys=0; phys<nproc; phys++ )
	        fprintf( stderr, "%d ", LOGIC_RANK( phys, raiz, nproc ) );
	   fprintf( stderr, " LOGIC from PHYSIC\n" );
	   for( int phys=0; phys<nproc; phys++ ) {
	        int logic_no = LOGIC_RANK( phys, raiz, nproc );
	        fprintf( stderr, "%d ", PHYSIC_RANK( logic_no, raiz, nproc ) );
	   }
	   fprintf( stderr, " PHYSIC from LOGIC\n" );
	   fprintf( stderr, "\n---------------------\n" );
	}
	
        if(processId == raiz){
		for(long int i = 1; i <= ni; i++)
			inmsg[i-1] = i+SEED;
	}

        if( processId == 0 )
           printf("----- root= %d, nmessages=%ld, nlong=%d"
                  " argc:%d argv[4]=%s argv[5]=%s comm_size=%d\n",
                    raiz, nmsg, ni, argc, argv[4], argv[5], nproc );
        
	MPI_Barrier(MPI_COMM_WORLD);

	if(processId == 0){
		chrono_reset(&myBroadcastChrono);
		chrono_start(&myBroadcastChrono);
	}

	for(int m = 0; m < nmsg; m++)
	   #if BCAST_TYPE == USE_MPI_Bcast
 	       MPI_Bcast(inmsg, ni, MPI_LONG, raiz, MPI_COMM_WORLD);
	   #elif BCAST_TYPE == USE_my_Bcast
	       my_Bcast(inmsg, ni, MPI_LONG, raiz, MPI_COMM_WORLD);
	   #else
	       assert( BCAST_TYPE == USE_MPI_Bcast || BCAST_TYPE == USE_my_Bcast );
	   #endif    
		
		   

	MPI_Barrier(MPI_COMM_WORLD);

	if(processId == 0){
		chrono_stop(&myBroadcastChrono);
		chrono_reportTime(&myBroadcastChrono, "myBroadcastChrono");

		// calcular e imprimir a VAZAO (nesse caso: numero de BYTES/s)
		double total_time_in_seconds = (double)chrono_gettotal(&myBroadcastChrono) /
									((double)1000 * 1000 * 1000);
	    double total_time_in_micro = (double)chrono_gettotal(&myBroadcastChrono) /
									((double)1000);
		printf("total_time_in_seconds: %lf s\n", total_time_in_seconds);
		printf("Latencia: %lf us (CADA broadcast)\n", (total_time_in_micro / nmsg));
		double MBPS = (((double)nmsg*tmsg) / ((double)total_time_in_seconds*1000*1000));
		printf("Throughput: %lf MB/s\n", MBPS*(nproc-1));
	}
	
	#if DEBUG == 1
		int rank;
		MPI_Comm_rank( MPI_COMM_WORLD, &rank ); 

		printf("rank %d: ", rank);
		for(long int i = 0; i < 2; i++)
			printf("%ld ", inmsg[i]);
		for(long int i = ni-2; i < ni; i++)
			printf("%ld ", inmsg[i]);
		printf("\n");
	#endif
	
	wz_debug = 1;
	// verifica_my_Bcast a partir de raiz 0  ---------- COM valores da linha de comando
	verifica_my_Bcast( inmsg, ni, MPI_LONG, raiz, MPI_COMM_WORLD );

        // verifica_my_Bcast a partir de raiz 0 ---------- COM OUTROS valores
	//verifica_my_Bcast( inmsg, 7, MPI_LONG, 0, MPI_COMM_WORLD );
        // verifica_my_Bcast a partir de raiz 1 
	//verifica_my_Bcast( inmsg, 7, MPI_LONG, 1, MPI_COMM_WORLD );
        // verifica_my_Bcast a partir de raiz 3 
	//verifica_my_Bcast( inmsg, 7, MPI_LONG, 3, MPI_COMM_WORLD );
	
	free(inmsg);

	MPI_Finalize( );
	return 0;
}
