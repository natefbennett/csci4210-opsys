#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>


#define BUFF_CHAR_MAX 1024
#define ARG_CHAR_MAX 64

int TokensInBuffer( char *buffer, char delim )
{
    int count = 0;
    char * itr = buffer;
    while ( *itr != '\0' && *itr != '\n' )
    {
        if ( *itr == delim ) 
            count++;

        itr++;
    }

    // buffer not empty 
    // NOTE: last word does not have a space after
    if ( itr > buffer )
        count++;

    fflush( stdout );
    return count; 
}

// must free pointer returned after calling
char ** Split( char * buffer, char delim )
{
    // build deliminator string
    char * delim_str = calloc( 3, sizeof( char ) );
    *delim_str = delim;
    *(delim_str + 1) = '\n';
    *(delim_str + 2) = '\0';

    // count number of tokens and allocate memory
    int num_tokens = TokensInBuffer( buffer, delim );
    char ** token_arr = calloc( num_tokens+1, sizeof( char * ) );
    *(token_arr + num_tokens) = NULL; 

    // parse input buffer
    char * save_ptr = NULL;
    char * token = strtok_r( buffer, delim_str, &save_ptr );

    char ** index_ptr = token_arr;
    while ( token != NULL )
    {
        // allocate memory and add token to array
        *index_ptr = calloc( strlen( token ) + 1, sizeof( char ) );
        strcpy( *index_ptr, token );

        // get next token and move to next slot in array
        token = strtok_r( save_ptr, delim_str, &save_ptr );
        index_ptr++;
    }

    free( delim_str );
    
    return token_arr;
}

int ArraySize ( char ** array )
{
    char ** itr = array;
    while ( *itr != NULL )
        itr++;

    return itr-array;
}

void FreeStringArray( char ** array )
{
    char ** itr = array;
    while ( *itr != NULL )
    {
        free( *itr );
        *itr = NULL;
        itr++;
    }
    free( array );
}

int main()
{
    // NOTE: for Submitty (auto-grader) use only
    setvbuf( stdout, NULL, _IONBF, 0 ); 

    // initialize process id for forking and wait status variable
    pid_t process_id = 0;
    int wstatus;

    char * buffer = calloc( BUFF_CHAR_MAX, sizeof( char ) );
    struct stat * statbuf = calloc( 1, sizeof( struct stat ) );

    char * path_env = getenv( "MYPATH" );
    
    // check if set
    if ( path_env == NULL )
        strcpy( buffer, "/bin" );
    else 
        strcpy( buffer, getenv("MYPATH") );

    char ** mypath = Split( buffer, ':' );

    while ( 1 )
    {
        // // check parent for background processes
		pid_t res = waitpid( 0, &wstatus, WNOHANG );
		if ( res > 0 )
        {
            if ( wstatus == 0 )
			    printf( "<background process terminated with exit status %d>\n", wstatus );
            else       
                printf( "<background process terminated abnormally>\n" );
        }

        // bring up prompt 
        printf( "$ " );
        
        // get a line of user input
        fgets( buffer, BUFF_CHAR_MAX, stdin );

        // convert line into array of strings
        char ** input_arr = Split( buffer, ' ' ); 
        int input_arr_size = ArraySize( input_arr );
        char * command = *input_arr;

        // check if no command supplied
        if ( input_arr_size == 0 )
        {
            FreeStringArray( input_arr );
            continue;
        }

        // check if command "exit"
        if ( strcmp( command, "exit" ) == 0 )
        {
            printf( "bye\n" );
            FreeStringArray( input_arr );
            break;
        }
        // check if command "cd"
        else if ( strcmp( command, "cd" ) == 0 ) 
        {
            char * dir;
            // check if a directory is supplied
            if ( input_arr_size == 1 )
                dir = getenv( "HOME" );
            else 
                dir = *(input_arr+1);

            int res = chdir( dir );
            
            if ( res == -1 )
                perror( "ERROR: chdir() failed!" );

            FreeStringArray( input_arr );
            continue;
        }

        // check if background ('&' at EOL) or forground process
        int bg_process = 0;
        char ** itr = input_arr;
        while ( *itr != NULL ) 
        {
            if ( **itr == '&' )
            {
                // remove '&' from input array
                free( *itr );
                *itr = NULL;
                bg_process = 1;
            }
            itr++;
        }
        
        // check if command in path
        int command_found = 0;
        char * absolute_path;
        itr = mypath;
        while ( *itr != NULL ) 
        {            
            sprintf( buffer, "%s/%s", *itr, command );
            // if executable exists
            if ( lstat( buffer, statbuf ) == 0 )
            {
                absolute_path = buffer;
                command_found = 1;
                break;
            }
            itr++;
        }

        if ( !command_found )
        {
            fprintf( stderr, "%s: command not found\n", command );
            FreeStringArray( input_arr );
            continue;
        }

        // fork to run command
        process_id = fork();

        // fork failed
        if ( process_id == -1 )
        {
            perror( "ERROR: fork() failed!" );
            FreeStringArray( input_arr );
            continue;
        }
        // is child
        else if ( process_id == 0 )
        {
            // run command
            execv( absolute_path, input_arr );
        }
        // is parent
        else
        {
            // wait on child if a forground process
            if ( bg_process )
            {
                printf("<running background process \"%s\">\n", command);
            }
            else
            {
                waitpid( process_id, &wstatus, 0 );
            }
        }
        FreeStringArray(input_arr);
    }

    free( buffer );
    free( statbuf );
    FreeStringArray( mypath );

    return EXIT_SUCCESS;
}