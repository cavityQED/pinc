#ifndef BST_H
#define BST_H

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>


typedef struct node_t Node;
struct node_t
{
	uint32_t	key;
	Node*		parent;
	Node*		left;
	Node*		right;
	void*		data;

	uint32_t	height;
	uint32_t	size;
};

int		skew				(Node* node);
void	node_augment		(Node* node);
void	print_node			(Node* node);
void	print_tree			(Node* root);
void	node_swap_data		(Node* high, Node* low);

Node* 	node_prev			(Node* node);
Node* 	node_next			(Node* node);
Node* 	node_free			(Node* node);
Node* 	node_remove			(Node* node);
Node* 	node_find			(Node* root, uint32_t key);

Node* 	tree_first			(Node* root);
Node* 	tree_last			(Node* root);
Node* 	tree_rotate_right	(Node* root);
Node* 	tree_rotate_left	(Node* root);
Node* 	tree_balance		(Node* root);

void	tree_delete			(Node* root);
Node* 	tree_insert_after	(Node* root, Node* node);
Node* 	tree_insert_before	(Node* root, Node* node);
Node* 	tree_insert_first	(Node* root, Node* node);
Node* 	tree_insert_last	(Node* root, Node* node);
Node* 	tree_insert_node	(Node* root, Node* node);
Node* 	tree_insert_pair	(Node* root, uint32_t key, void* data);



#endif