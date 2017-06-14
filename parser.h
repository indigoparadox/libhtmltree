
#ifndef PARSER_H
#define PARSER_H

#include "bstrlib.h"

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

#endif /* PARSER_H */

