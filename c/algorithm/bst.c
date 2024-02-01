#include "bst.h"




int skew(Node* node)
{
	if(!node)
		return 0;

	uint32_t lh = (node->left)?		node->left->height	: 0;
	uint32_t rh = (node->right)?	node->right->height	: 0;

	return rh - lh;
}

void node_augment(Node* node)
{
	uint32_t lh		= 0;
	uint32_t rh		= 0;
	uint32_t size	= 0;
	
	Node* left	= node->left;
	Node* right	= node->right;

	if(left)
	{
		lh		= left->height;
		size 	+= left->size;
	}

	if(right)
	{
		rh		= right->height;
		size	+= right->size;
	}

	node->height	= (lh > rh)? ++lh : ++rh;
	node->size		= ++size;
}

void node_swap_data(Node* high, Node* low)
{
	Node tmp;
	tmp.key		= high->key;
	tmp.data	= high->data;

	high->key	= low->key;
	high->data	= low->data;

	low->key	= tmp.key;
	low->data	= tmp.data;
}

void print_node(Node* node)
{
	printf("Node (%d, 0x%X):\t", node->key, ((uint32_t)node) & 0xFFFF);
	printf("(L: %d,   P: %d,   R: %d,   H: %d,   S: %d)  \n", 
					(node->left)?	node->left->key		: -1,
					(node->parent)?	node->parent->key	: -1,
					(node->right)?	node->right->key	: -1,
					node->height,
					node->size);
}

void print_tree(Node* root)
{
	if(!root)
		return;

	print_tree(root->left);
	print_node(root);
	print_tree(root->right);
}






















Node* node_prev(Node* root)
{
	Node* left = root->left;
	if(left)
		return tree_last(left);
	while(root == root->parent->left)
		root = root->parent;
	return root->parent;
}


Node* node_next(Node* root)
{
	Node* right = root->right;
	if(right)
		return tree_first(right);
	while(root == root->parent->right)
		root = root->parent;
	return root->parent;
}


Node* node_find(Node* root, uint32_t key)
{
	printf("Node Find Key:\t0x%8X\n", key);
	if(!root)
		return NULL;

	uint32_t rkey = root->key;

	if(rkey == key)
		return root;

	if(rkey > key)
		return node_find(root->left, key);
	if(rkey < key)
		return node_find(root->right, key);
}


Node* node_free(Node* root)
{
	uint32_t key = root->key;
	Node* parent = root->parent;

	if(parent)
	{
		if(root == parent->left)
			parent->left = 0;
		else
			parent->right = 0;
	}

	memset(root, 0, sizeof(Node));
	free(root->data);
	free(root);
	return tree_balance(parent);
}


Node* node_remove(Node* root)
{
	Node* lower = root;

	if(root->left)
		lower = node_prev(root);
	else if(root->right)
		lower = node_next(root);
	else
		return node_free(root);

	if(lower != root)
		node_swap_data(root, lower);

	node_remove(lower);
}



























Node* tree_first(Node* root)
{
	if(!root->left)
		return root;
	return tree_first(root);
}


Node* tree_last(Node* root)
{
	if(!root->right)
		return root;
	return tree_last(root);
}


Node* tree_rotate_right(Node* root)
{
	// printf("tree_rotate_right(%d)\n", root->key);
	// print_tree(root);
	Node* old_root	= root;
	Node* parent	= root->parent;
	Node* new_root	= root->left;
	Node* pivot		= new_root->right;

	if(pivot)
		pivot->parent = old_root;
	if(parent)
	{
		if(root == parent->left)
			parent->left = new_root;
		else
			parent->right = new_root;
	}

	old_root->parent	= new_root;
	old_root->left		= pivot;
	new_root->parent	= parent;
	new_root->right		= old_root;

	node_augment(old_root);
	node_augment(new_root);
	// print_tree(new_root);

	return new_root;
}


Node* tree_rotate_left(Node* root)
{
	// printf("tree_rotate_left(%d)\n", root->key);
	// print_tree(root);
	Node* old_root	= root;
	Node* parent	= root->parent;
	Node* new_root	= root->right;
	Node* pivot		= new_root->left;

	if(pivot)
		pivot->parent = old_root;
	if(parent)
	{
		if(root == parent->left)
			parent->left = new_root;
		else
			parent->right = new_root;
	}

	old_root->parent	= new_root;
	old_root->right		= pivot;
	new_root->parent	= parent;
	new_root->left		= old_root;

	node_augment(old_root);
	node_augment(new_root);
	// print_tree(new_root);

	return new_root;
}


Node* tree_balance(Node* root)
{
	if(!root)
		return root;

	node_augment(root);

	if(skew(root) > 1)
	{
		if(skew(root->right) == -1)
			tree_rotate_right(root->right);
		root = tree_rotate_left(root);
	}

	else if(skew(root) < -1)
	{
		if(skew(root->left) == -1)
			tree_rotate_left(root->left);
		root = tree_rotate_right(root);
	}

	return (root->parent)? tree_balance(root->parent) : root;
}



































Node* tree_insert_after(Node* root, Node* node)
{
	// printf("tree_insert_after(%d, %d)\n", root->key, node->key);
	if(!root->right)
	{
		root->right = node;
		node->parent = root;
	}

	else
	{
		Node* parent = node_next(root);
		parent->left = node;
		node->parent = parent;
	}

	// printf("tree_insert_after()\n");
	return tree_balance(node);
}


Node* tree_insert_before(Node* root, Node* node)
{
	// printf("tree_insert_before(%d, %d)\n", root->key, node->key);
	if(!root->left)
	{
		root->left = node;
		node->parent = root;
	}

	else
	{
		Node* parent = node_prev(root);
		parent->right = node;
		node->parent = parent;
	}

	return tree_balance(node);
}


Node* tree_insert_node(Node* root, Node* node)
{
	if(!root)
		return tree_balance(node);

//	printf("tree_insert_node(%d, %d)\n", root->key, node->key);
	uint32_t rkey = root->key;
	uint32_t nkey = node->key;

	if(rkey == nkey)
	{
		root->data = node->data;
		return tree_balance(root);
	}
	else if(rkey > nkey)
	{
		if(!root->left)
			return tree_insert_before(root, node);
		else
			return tree_insert_node(root->left, node);
	}
	else if(rkey < nkey)
	{
		if(!root->right)
			return tree_insert_after(root, node);
		else
			return tree_insert_node(root->right, node);
	}

	// printf("tree_insert_node()\n");
}

Node* tree_insert_pair(Node* root, uint32_t key, void* data)
{
	Node* new_node = malloc(sizeof(Node));
	memset(new_node, 0, sizeof(Node));
	new_node->key = key;
	new_node->data = data;
	return tree_insert_node(root, new_node);
}












void tree_delete(Node* root)
{
	if(!root)
		return;
	else if(root->left)
		tree_delete(root->left);
	else if(root->right)
		tree_delete(root->right);
	else
	{
		root = node_remove(root);
		tree_delete(root);
	}
}

