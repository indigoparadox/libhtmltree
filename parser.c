
#include "parser.h"

#include <stdint.h>
#include <stdlib.h>

enum html_tree_state {
   HTML_TREE_IN_DATA,
   HTML_TREE_OPENING_TAG,
   HTML_TREE_IN_START_TAG,
   HTML_TREE_IN_END_TAG,
   HTML_TREE_IN_ATTR_LABEL,
   HTML_TREE_IN_ATTR_VALUE
};

struct html_tree_attr {
   bstring label;
   bstring value;
   struct html_tree_attr* next;
};

struct html_tree_tag {
   bstring tag;
   bstring data;
   struct html_tree_attr* attrs;
   struct html_tree_tag* parent;
   struct html_tree_tag* first_child;
   struct html_tree_tag* next_sibling;
};

struct html_tree {
   enum html_tree_state state;
   char last_char;
   struct html_tree_tag* root;
   struct html_tree_tag* current;
};

void html_tree_new_tag( struct html_tree* tree ) {
   struct html_tree_tag* new_tag = NULL;

   /* Create the new tag. */
   new_tag = calloc( 1, sizeof( struct html_tree_tag ) );
   new_tag->tag = bfromcstr( "" );
   new_tag->data = bfromcstr( "" );

   if( NULL == tree->root ) {
      tree->root = new_tag;
   }

   if( NULL == tree->current ) {
      tree->current = tree->root;
   }

   if(
      new_tag != tree->current &&
      NULL == tree->current->first_child
   ) {
      /* If the new tag is not the root, it must be a child... */
      tree->current->first_child = new_tag;
      new_tag->parent = tree->current;
      tree->current = tree->current->first_child;
   } else if(
      new_tag != tree->current &&
      NULL != tree->current->first_child
   ) {
      /* If it's not the first child, it must be a sibling of that child. */
      new_tag->parent = tree->current;
      tree->current = tree->current->first_child;
      while( NULL != tree->current->next_sibling ) {
         tree->current = tree->current->next_sibling;
      }
      tree->current->next_sibling = new_tag;
      tree->current = tree->current->next_sibling;
   }
}
            
static void html_tree_new_attr( struct html_tree_tag* tag ) {
   struct html_tree_attr* attr_iter = NULL;
   struct html_tree_attr* new_attr = NULL;

   new_attr = calloc( 1, sizeof( struct html_tree_attr ) );
   new_attr->label = bfromcstr( "" );
   new_attr->value = bfromcstr( "" );

   if( NULL == tag ) {
      /* TODO: Error. */
   }

   if( NULL == tag->attrs ) {
      tag->attrs = new_attr;
   } else {
      attr_iter = tag->attrs;
      while( NULL != attr_iter->next ) {
         attr_iter = attr_iter->next;
      }
      attr_iter->next = new_attr;
   }
}

static void html_tree_append_char( char c, struct html_tree* tree ) {
   struct html_tree_attr* attr_current = NULL;

   switch( tree->state ) {
      case HTML_TREE_OPENING_TAG:
         bconchar( tree->current->tag, c );
         break;
   
      case HTML_TREE_IN_DATA:         
         bconchar( tree->current->data, c );
         break;

      case HTML_TREE_IN_ATTR_LABEL:
         attr_current = tree->current->attrs;
         /* The current attr is always last. */
         while( NULL != attr_current->next ) {
            attr_current = attr_current->next;
         }
         bconchar( attr_current->label, c );
         break;

      case HTML_TREE_IN_ATTR_VALUE:
         attr_current = tree->current->attrs;
         /* The current attr is always last. */
         while( NULL != attr_current->next ) {
            attr_current = attr_current->next;
         }
         bconchar( attr_current->value, c );
         break;
   }
}

static void html_tree_parse_char( char c, struct html_tree* tree ) {

   switch( c ) {
      case '<':
         if( 
            HTML_TREE_OPENING_TAG == tree->state ||
            HTML_TREE_IN_START_TAG == tree->state ||
            HTML_TREE_IN_END_TAG == tree->state ||
            HTML_TREE_IN_ATTR_LABEL == tree->state ||
            /* TODO: Should we allow these in attr values? */
            HTML_TREE_IN_ATTR_VALUE == tree->state
         ) {
            /* TODO: Error: Double-open tag. */
         } else {
            html_tree_new_tag( tree );
            tree->state == HTML_TREE_IN_START_TAG;
         }
         break;

      case '>':
         if( HTML_TREE_IN_START_TAG == tree->state ) {
            tree->state = HTML_TREE_IN_DATA;
         } else if( HTML_TREE_IN_END_TAG == tree->state ) {
            if( NULL != tree->current->parent ) {
               tree->current = tree->current->parent;
            }
            tree->state = HTML_TREE_IN_DATA;
         }
         break;

      case '\0':
         break;

      case '"':
         if( HTML_TREE_IN_ATTR_LABEL == tree->state ) {
            tree->state = HTML_TREE_IN_ATTR_VALUE;
         } else if( HTML_TREE_IN_ATTR_VALUE == tree->state ) {
            /* Only the start tag has attrs, anyway. */
            tree->state = HTML_TREE_IN_START_TAG;
         }
         break;
  
      case '=':
         /* Don't append the '=' to the attr label. */
         if( HTML_TREE_IN_ATTR_LABEL != tree->state ) {
            html_tree_append_char( c, tree );
         }
         break;

      case '/':
         if( HTML_TREE_OPENING_TAG == tree->state ) {
            tree->state = HTML_TREE_IN_END_TAG;
         }
         break;

      case ' ':
         if( HTML_TREE_IN_START_TAG == tree->state ) {
            html_tree_new_attr( tree->current );
            tree->state = HTML_TREE_IN_ATTR_LABEL;
         }
         break;

      default:
         html_tree_append_char( c, tree );
         break;
   }

   tree->last_char = c;
}

int html_tree_parse_string( bstring html_string, struct html_tree* out ) {
   int i;

   if( NULL == out ) {
      /* TODO: Error. */
   }

   if( NULL != out->root ) {
      /* TODO: Error. */
   }

   out->root = calloc( 1, sizeof( struct html_tree_tag ) );
   out->current = out->root;

   for( i = 0 ; blength( html_string ) > i ; i++ ) {
      html_tree_parse_char( bchar( html_string, i ), out );
   }

}

