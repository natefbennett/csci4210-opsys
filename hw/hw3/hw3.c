#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

// get global variables from submitty file
extern int next_thread_id;        
extern int max_squares;           
extern char *** dead_end_boards;

// data structure to manage shared memory
typedef struct {
    // shared globals
    // int *     next_thread_id;  // lock
    // int *     max_squares;     // lock
    // char ***  dead_end_boards; // lock

    // mutex lock for shared globals
    pthread_mutex_t lock;

    // inherited data from parent thread
    char ** board;
    int *   dead_board_count; // track length of dead_end_boards lock
    int *   num_solutions;
    int m, n, // board dimentions: m & n, 
        r, c, // knight position: r & c, 
        x,    // min visited for dead end: x    
        move_num,
        cur_thread_id;  
} SharedData;

// hard-coded knight moves table, organized by [ [row, col], ... ]
// reletive to position S
//         [    c o l    ]
//        [0][1][2][3][4]  
//  ~  [0] .  4  .  3  .   
//  r  [1] 5  .  .  .  2   
//  o  [2] .  . <S> .  .   
//  w  [3] 6  .  .  .  1   
//  ~  [4] .  7  .  8  .  
int moves[8][2] = {  {  1,  2 },    // 1
                     { -1,  2 },    // 2
                     { -2,  1 },    // 3
                     { -2, -1 },    // 4
                     { -1, -2 },    // 5      
                     {  1, -2 },    // 6
                     {  2, -1 },    // 7
                     {  2,  1 }  };  // 8

void PrintBoard( char ** board, int rows, int cols )
{
    for ( int i = 0; i < rows; i++ )
    {
        printf( "MAIN: " );

        // add arrows for first line
        if ( i == 0 )
            printf( ">>" );
        else
            printf( "  " );
        
        printf( "%.*s", cols, board[i] );

        // add arrows for last line
        if ( i == ( rows-1 ) )
            printf( "<<" );

        printf( "\n" );
    }
}

void FreeBoard( char ** board, int rows )
{
    for ( int i = 0; i < rows; i++ )
        free( board[i] );
    free( board );
}

char ** CopyBoard( char ** board, int rows, int cols )
{
    char ** new_board = calloc( rows, sizeof( char * ) );
    // printf("OLD BOARD------------\n");
    // PrintBoard(board, rows, cols);
    // printf( "board :>> %p\n", (void *) board ); // DEBUG
    for ( int i = 0; i < rows; i++ )
    {
        new_board[i] = calloc( cols, sizeof( char ) );
        strncpy( new_board[i], board[i], cols ); 
    }
    // printf("NEW BOARD\n");
    // PrintBoard(new_board, rows, cols);
    // printf( "board :>> %p\n", (void *) new_board ); // DEBUG
    return new_board;
}

void AddBoard( char ** board, int * dest_size ) 
{
    // printf( "*dest_size :>> %d\n", *dest_size ); // DEBUG
    // printf( ">> before dest :>> %p\n", (void *) dead_end_boards ); // DEBUG
    // initialized with size 4
    if ( *dest_size > 3 )
    {
        dead_end_boards = realloc( dead_end_boards, ( ( (*dest_size)+1 ) * sizeof( char ** ) ) );
        // printf( ">> after dest :>> %p\n", (void *) dead_end_boards ); // DEBUG
    }
        
    dead_end_boards[*dest_size] = board;
    ( *dest_size )++;
    // *dest_size = *dest_size + 1;
}

// checks if r and c are within board bounds
int OutOfBounds( int m, int n, int r, int c )
{
    return ( r < 0 || c < 0 || r >= m || c >= n );
}

// checks if r and c are within board bounds and not yet visited
int ValidMove( char ** board, int m, int n, int r, int c )
{
    return !OutOfBounds( m, n, r, c ) && board[r][c] == '.';
}

// returns the number of valid moves and fills the valid moves array with booleans values
int GetValidMoves( char ** board, int * valid_moves, int m, int n, int r, int c )
{
    int valid_count = 0;
    for ( int i = 0; i < 8; i++ )
    {
        if ( ValidMove( board, m, n, moves[i][0]+r, moves[i][1]+c ) )
        {
            valid_moves[i] = 1;
            valid_count++;
        }
    }
    return valid_count;
}

// given a move index it sets the knight to that position on the board
void SetMove( char ** board, int move_index, int * r, int * c )
{
    int new_location[2];
    new_location[0] = moves[move_index][0]+*r;
    new_location[1] = moves[move_index][1]+*c;
    *r = new_location[0];
    *c = new_location[1];
    board[new_location[0]][new_location[1]] = 'S';
}

void * PThreadWorker( void * d )
{
    // has to start as void be definition of pthread_create()
    SharedData *data = ( SharedData* )d;

    // loop until knights tour complete
    while ( 1 )
    {
        // get valid moves
        int valid_moves[] = { 0, 0, 0, 0, 0, 0, 0, 0 };
        int num_moves = GetValidMoves( data->board, valid_moves, 
                                       data->m,     data->n,      
                                       data->r,     data->c );
        // printf( ">> [%d,%d,%d,%d,%d,%d,%d,%d]\n", valid_moves[0], valid_moves[1], valid_moves[2], valid_moves[3],valid_moves[4], valid_moves[5], valid_moves[6], valid_moves[7] ); // DEBUG
        // only one move possible, do not create another thread
        if ( num_moves == 1 )
        {
            // find the valid move and make it
            for ( int i = 0; i < 8; i++ )
            {
                if ( valid_moves[i] )
                {
                    SetMove( data->board, i, &data->r, &data->c );
                    data->move_num++;
                    break;
                }
            }
        }
        // no more possible moves, exit thread
        else if ( num_moves == 0 )
        {
            
    
            // check for full knights tour
            if ( data->move_num == data->n*data->m )
            {
                printf("THREAD %d: Sonny found a full knight's tour!\n", data->cur_thread_id );
                ( *data->num_solutions )++;
            }
            else 
            {
                // if enough moves were made (x) add to dead board array
                if ( data->move_num >= data->x )
                {
                    pthread_mutex_lock( &data->lock );
                    char ** board_cpy = CopyBoard( data->board, data->m, data->n );
                    // lock mutex to edit shared memory
                    
                    
                    AddBoard( board_cpy, data->dead_board_count );
                    // printf( "dead_end_boards :>> %p\n", (void *) dead_end_boards ); // DEBUG

                    // unlock mutex, editing done
                    pthread_mutex_unlock( &data->lock );
                }                 
                if ( data->cur_thread_id == 0 )
                    printf( "MAIN: " );
                else
                    printf( "THREAD %d: ", data->cur_thread_id );
                printf("Dead end at move #%d\n", data->move_num );
            }

            // set max squares if surpassed
            if ( max_squares < data->move_num )
                max_squares = data->move_num;

            // free board in current shared data struct    
            // FreeBoard( data->board, data->m );
        
            
            
            
            // exit thread
            int * ret_val = calloc( 1, sizeof( int ) );
            *ret_val = data->move_num;

            pthread_exit( ret_val );
        }
        // create thread for each possible move
        else if ( num_moves > 1 )
        {
            if ( data->cur_thread_id == 0 )
                printf( "MAIN: " );
            else
                printf( "THREAD %d: ", data->cur_thread_id );
            printf("%d possible moves after move #%d; creating %d child threads...\n", num_moves, data->move_num, num_moves );

            pthread_t child_threads[num_moves];
            int max_score = 0;
            int cur_child = 0;

            // create a new thread for every possible move
            for ( int i = 0; i < 8; i++ )
            {
                if ( valid_moves[i] )
                {
                    // create new shared data struct for child thread
                    SharedData c_data;
                    
                    

                    // c_data.next_thread_id = data->next_thread_id;
                    // c_data.max_squares = data->max_squares;
                    // c_data.dead_end_boards = data->dead_end_boards;                    
                    // c_data.board = CopyBoardStack( data->board, data->m, data->n );
                    c_data.dead_board_count = data->dead_board_count;
                    c_data.num_solutions = data->num_solutions;
                    c_data.move_num = data->move_num;
                    c_data.lock = data->lock;
                    c_data.n = data->n;
                    c_data.m = data->m;
                    c_data.r = data->r;
                    c_data.c = data->c;
                    c_data.x = data->x;

                    pthread_mutex_lock( &data->lock );
                    c_data.board = CopyBoard( data->board, data->m, data->n );
                    c_data.cur_thread_id = next_thread_id;
                    next_thread_id++;
                    pthread_mutex_unlock( &data->lock );
                    

                    
                    
                    // make move
                    SetMove( c_data.board, i, &c_data.r, &c_data.c );
                    c_data.move_num++;
                    
                    pthread_create( &child_threads[cur_child], NULL, PThreadWorker, &c_data );
                    
                    // NO_PARALLEL flag on
                    // join then move to next
                    #ifdef NO_PARALLEL
                        int * ret_val;
                        pthread_join( child_threads[cur_child], ( void ** )&ret_val );
                        if ( data->cur_thread_id == 0 )
                            printf( "MAIN: " );
                        else
                            printf( "THREAD %d: ", data->cur_thread_id );
                                printf("Thread %d joined (returned %d)\n", c_data.cur_thread_id, *ret_val);
                        
                        // set max score if higher
                        if ( max_score < *ret_val )
                            max_score = *ret_val;
                        
                        free( ret_val );
                    #endif

                    cur_child++;
                }

            }

            // PARALLEL flag on
            // join back all at once
            #ifdef PARALLEL
                for ( int i = 0; i < num_moves; i++ )
                {
                    int * ret_val;
                    pthread_join( child_threads[i], (void**)&ret_val );
                    if ( data->cur_thread_id == 0 )
                        printf( "MAIN: " );
                    else
                        printf( "THREAD %d: ", data->cur_thread_id );
                    printf("Thread %d joined (returned %d)\n", (data->cur_thread_id)+(i+1), *ret_val);
                    
                    // set max score if higher
                    if ( max_score < *ret_val )
                        max_score = *ret_val;
                    
                    free( ret_val );                    
                }
            #endif

            // pthread_mutex_lock( &data->lock );
            // free board in current shared data struct
            // FreeBoard( data->board, data->m );
            // pthread_mutex_unlock( &data->lock );

            // exit thread
            int * ret_val = calloc( 1, sizeof( int ) );
            *ret_val = max_score;
            // pthread_exit( ret_val );
            return ret_val;

        }
    }
}

int simulate ( int argc, char * argv[] )
{
    // NOTE: for Submitty (auto-grader) use only
    setvbuf( stdout, NULL, _IONBF, 0 ); 

    // board dimentions
    // m = rows; n = cols
    int m = atoi( argv[1] );
    int n = atoi( argv[2] );

    // starting position
    // r = rows; c = cols
    int r = atoi( argv[3] );
    int c = atoi( argv[4] );

    // minimum squares covered in a dead-end board for it to be stored
    int x = atoi( argv[5] );

    int input_valid = 1;

    // validate number of comand line args
    if ( argc != 6 )
        input_valid = 0;

    // validate correctness of board dimentions
    if ( m < 3 || n < 3 )
        input_valid = 0;

    // validate starting position, within bounds of board
    if ( OutOfBounds( m, n, r, c ) )
        input_valid = 0;

    // validate x, positive integer no greater than m * n
    if ( x < 1 || x > ( m*n ) )
        input_valid = 0;

    if ( !input_valid )
    {
        fprintf( stderr, "ERROR: Invalid argument(s)\nUSAGE: a.out <m> <n> <r> <c> <x>\n" );
        return EXIT_FAILURE;
    }

    // make starting board
    // initialze & allocate
    char ** board = calloc( m, sizeof( char * ) ); 
    for ( int i = 0; i < m; i++ )
        board[i] = calloc( n, sizeof( char ) );

    // fill board
    for ( int i = 0; i < m; i++ )
        for ( int j = 0; j < n; j++ )
            board[i][j] = '.';

    board[r][c] = 'S';

    // allocate space to track number of dead end boards
    int * dead_board_count = calloc( 1, sizeof( int ) );
    *dead_board_count = 0;
    int * num_solutions = calloc( 1, sizeof( int ) );
    *num_solutions = 0;
    // int * move_num = calloc( 1, sizeof( int ) );
    // move_num = 1;

    printf( "MAIN: Solving Sonny's knight's tour problem for a %dx%d board\n", m, n );
    printf( "MAIN: Sonny starts at row %d and column %d (move #1)\n", r, c );

    // allocate and initialize shared memory data structure
    SharedData * data = calloc( 1, sizeof( SharedData ) );
    // data->next_thread_id = &next_thread_id;
    // data->max_squares = &max_squares;
    // data->dead_end_boards = dead_end_boards;
    data->board = board;
    data->dead_board_count = dead_board_count;
    data->num_solutions = num_solutions;
    data->move_num = 1;
    data->cur_thread_id = 0;
    data->n = n;
    data->m = m;
    data->r = r;
    data->c = c;
    data->x = x;

    // set mutex to protect shared memory segment
    // only one thread should be able to edit shared segment at a time
    pthread_mutexattr_t mutex_attr;
    pthread_mutexattr_init( &mutex_attr );
    pthread_mutexattr_setpshared( &mutex_attr, PTHREAD_PROCESS_SHARED );
    pthread_mutex_init( &data->lock, &mutex_attr );

    // create initial new thread worker to kick off recursion
    int * ret_val;
    pthread_t thread;
    pthread_create( &thread, NULL, PThreadWorker, data );
    pthread_join( thread, (void **)&ret_val) ;
    free( ret_val );

    // print ending output stats
    if ( max_squares == ( m*n ) )
    {
        if ( *data->num_solutions == 1 )
            printf( "MAIN: All threads joined; found 1 possible way to achieve a full knight's tour\n" );
        else
            printf( "MAIN: All threads joined; found %d possible ways to achieve a full knight's tour\n", *data->num_solutions );

    }
    else
    {
        if ( max_squares == 1 )
            printf( "MAIN: All threads joined; best solution(s) visited 1 square out of %d\n", ( m*n ) );
        else
            printf( "MAIN: All threads joined; best solution(s) visited %d squares out of %d\n", max_squares, ( m*n ) );
    
        if ( *dead_board_count == 1 )    
        printf( "MAIN: Dead end board covering at least %d square:\n", x );
        else
            printf( "MAIN: Dead end boards covering at least %d squares:\n", x );

        for ( int i = 0; i < *dead_board_count; i++ )
            PrintBoard( dead_end_boards[i], m, n );
    }
    

    // free memory
    // free individual boards
    // for ( int i = 0; i < *dead_board_count; i++ )
    //     FreeBoard( dead_end_boards[i], m );

    free( dead_board_count );
    free( num_solutions );
    free( data );

    return EXIT_SUCCESS;
}