
#include "parser.h"

#include <stdlib.h>
#include <stdio.h>

void walk_tree( struct html_tree_tag* tag, int depth ) {
   struct html_tree_tag* tag_iter = NULL;
   struct html_tree_attr* attr_iter = NULL;
   
   tag_iter = tag;
   while( NULL != tag_iter ) {
      printf( "%d: %s\n", depth, bdata( tag_iter->tag ) );
      if( NULL != tag_iter->first_child ) {
         walk_tree( tag_iter->first_child, depth + 1 );
      }
      tag_iter = tag_iter->next_sibling;
   }
}

int main( void ) {
   bstring btest = NULL;
   struct html_tree* ttest = NULL;

   ttest = calloc( 1, sizeof( struct html_tree ) );
   btest = bfromcstr( "<html><head><title>Test</title></head><body>Test</body></html>" );

   html_tree_parse_string( btest, ttest );

   walk_tree( ttest->root, 0 );

   return 0;
}

