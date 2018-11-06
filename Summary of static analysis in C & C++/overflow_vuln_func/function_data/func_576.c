int main( int argc, char *argv[] )
{
    FILE *f;
    int i, j;
    char output[41];
    sha1_context ctx;
    unsigned char buf[1000];
    unsigned char sha1sum[20];

    if( argc < 2 )
    {
        printf( "\n SHA-1 Validation Tests:\n\n" );

        for( i = 0; i < 3; i++ )
        {
            printf( " Test %d ", i + 1 );

            sha1_starts( &ctx );

            if( i < 2 )
            {
                sha1_update( &ctx, (guint8 *) msg[i],
                             strlen( msg[i] ) );
            }
            else
            {
                memset( buf, 'a', 1000 );

                for( j = 0; j < 1000; j++ )
                {
                    sha1_update( &ctx, (guint8 *) buf, 1000 );
                }
            }

            sha1_finish( &ctx, sha1sum );

            for( j = 0; j < 20; j++ )
            {
                g_snprintf( output + j * 2, 41-j*2, "%02x", sha1sum[j] );
            }

            if( memcmp( output, val[i], 40 ) )
            {
                printf( "failed!\n" );
                return( 1 );
            }

            printf( "passed.\n" );
        }

        printf( "\n" );
    }
    else
    {
        if( ! ( f = ws_fopen( argv[1], "rb" ) ) )
        {
            printf("fopen: %s", g_strerror(errno));
            return( 1 );
        }

        sha1_starts( &ctx );

        while( ( i = fread( buf, 1, sizeof( buf ), f ) ) > 0 )
        {
            sha1_update( &ctx, buf, i );
        }

        sha1_finish( &ctx, sha1sum );

        for( j = 0; j < 20; j++ )
        {
            printf( "%02x", sha1sum[j] );
        }

        printf( "  %s\n", argv[1] );
    }

    return( 0 );
}
