#include <assert.h>

#define USE_MPI_Bcast 1  // do NOT change
#define USE_my_Bcast 2   // do NOT change
//choose either BCAST_TYPE in the defines bellow
//#define BCAST_TYPE USE_MPI_Bcast
#define BCAST_TYPE USE_my_Bcast

const int SEED = 100;

void my_Bcast(TYPEMSG *buffMsg, int ni, MPI_Datatype mpi_data, int raiz, MPI_Comm comm);

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

   free(buff);      
}



