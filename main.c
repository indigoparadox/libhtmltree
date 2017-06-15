
#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

void walk_tree( struct html_tree_tag* tag, int depth ) {
   struct html_tree_tag* tag_iter = NULL;
   struct html_tree_attr* attr_iter = NULL;
   int i;

   tag_iter = tag;
   while( NULL != tag_iter ) {
      for( i = 0 ; depth > i ; i++ ) {
         printf( "   " );
      }
      if( 0 == blength( tag_iter->tag ) ) {
         printf( "data ( %s )\n", bdata( tag_iter->data ) );
      } else {
         printf( "%s (", bdata( tag_iter->tag ) );
         attr_iter = tag_iter->attrs;
         while( NULL != attr_iter ) {
            printf( " %s=%s", bdata( attr_iter->label ), bdata( attr_iter->value ) );
            attr_iter = attr_iter->next;
         }
         printf( " )\n" );
      }
      if( NULL != tag_iter->first_child ) {
         walk_tree( tag_iter->first_child, depth + 1 );
      }
      tag_iter = tag_iter->next_sibling;
   }
}

int main( int argc, char** argv ) {
   bstring btest = NULL;
   struct html_tree* ttest = NULL;
   FILE* ftest = NULL;
   char readc = '\0';

   if( 2 > argc ) {
      printf( "Usage: %s FILE\n", argv[0] );
      return 1;
   }

   ftest = fopen( argv[1], "r" );
   if( NULL == ftest ) {
      printf( "Unable to open: %s\n", argv[1] );
      return 2;
   }

   btest = bfromcstr( "" );

   while( 1 == fread( &readc, 1, 1, ftest ) ) {
      bconchar( btest, readc );
   }

   fclose( ftest );

   ttest = calloc( 1, sizeof( struct html_tree ) );

   html_tree_parse_string( btest, ttest );

   walk_tree( ttest->root, 0 );

   bdestroy( btest );

   return 0;
}

