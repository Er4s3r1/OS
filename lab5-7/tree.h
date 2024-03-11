#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

typedef struct tree {
    int key;
    struct tree *left;
    struct tree *right;
    struct tree *parent;
    bool exist;
} node;

node *create (node *root, int key) {
    node *tmp = malloc(sizeof(node));
    tmp->key = key;
    tmp->parent = NULL;
    tmp->left = tmp->right = NULL;
    root = tmp;
    root->exist = true;
    return root;
}

node *add(node *root, int key) {
    node *root2 = root, *root3 = NULL;
    node *tmp = malloc(sizeof(node));
    tmp->key = key;
    while (root2 != NULL) {
        root3 = root2;
        if (key < root2->key) {
            root2 = root2->left;
        }
        else {
            root2 = root2->right;
        }
    }
    tmp->parent = root3;
    tmp->left = NULL;
    tmp->right = NULL;
    if (key < root3->key) {
        root3->left = tmp;
    }
    else {
        root3->right = tmp;
    }
    tmp->exist = true;
    return root;
}

node *search(node *root, int key) {
    if (root == NULL || root->key == key) {
        return root;
    }
    if (key < root->key) {
        return search(root->left, key);
    }
    else {
        return search(root->right, key);
    }
}

node *min(node *root) {
    node *l = root;
    while (l->left != NULL) {
        l = l->left;
    }
    return l;
}

node *max(node *root) {
    node *r = root;
    while (r->right != NULL) {
        r = r->right;
    }
    return r;
}

node *next(node *root) {            //поиск следующего элемента относительно текущего
    node *p = root, *l = NULL;
    if (p->right != NULL) {
        return min(p->right);
    }
    l = p->parent;
    while ((l != NULL) && (p == l->right)) {
        p = l;
        l = l->parent;
    }
    return l;
}

node *delete(node *root, int key) {
    if (root == NULL) {
        return root;
    }
    else if (key < root->key) {
        root->left = delete(root->left, key);
    }
    else if (key > root->key) {
        root->right = delete(root->right, key);
    }
    else {
        if (root->left == NULL && root->right == NULL) {
            free(root);
            root = NULL;
        }
        else if (root->left == NULL) {
            node *temp = root;
            root = root->right;
            free(temp);
        }
        else if (root->right == NULL) {
            node *temp = root;
            root = root->left;
            free(temp);
        }
        else {
            node *temp = min(root->right);
            root->key = temp->key;
            root->right = delete(root->right, temp->key);
        }
    }
    return root;
}

void disableNode(node *root) {
    if (root == NULL) {
        return;
    }
    root->exist = false;
    disableNode(root->left);
    disableNode(root->right);
    
}

void disableOneNode(node *root) {
    root->exist = false;
}

void print_tree(node *root, int depth) {
    if (root == NULL) {
        return;
    }
    print_tree(root->right, depth+1);
    for (int i = 0; i < depth; i++) {
        printf("|===");
    }
    
    if (root->exist) {
        printf("%d\n", root->key);
    } else {
        printf("%d (dead)\n", root->key);
    }
    
    print_tree(root->left, depth+1);
}

void preorder(node *root) {     //обход в прямом порядке КЛП
    if (root != NULL) {
        preorder(root->left);
        printf("%d---", root->key);
        preorder(root->right);
    }
}

void postorder(node *root) {    //обход в обратном порядке ЛПК
    if (root == NULL) {
        return;
    }
    postorder(root->left);
    postorder(root->right);
    if (root->key) {
        printf("%d---", root->key);
    } 
}

void inorder(node *root) {    //обход дерева в симметричном порядке ЛКП
    if (root == NULL) {
        return;
    }
    inorder(root->left);
    if (root->key) {
        printf("%d---", root->key);
    }
    inorder(root->right);
}

node *deleteNodeAndChildren(node* root, int key) {
    if (root == NULL) {
        return root;
    }
    if (root->key == key) {
        root = NULL;
        return root;
    }
    root->left = deleteNodeAndChildren(root->left, key);
    root->right = deleteNodeAndChildren(root->right, key);
    return root;
}