#include<stdio.h>
#include<stdlib.h>
#include"assignment_4.h"
#define ins(val) insert_key(val, fp)


//Using only the left offset we create a linked list of deleted/invalid nodes. This function adds the deleted node to the list.
void add_deleted_node(node_t *currNode,int currOff, tree_t *h, FILE *fp)
{
    currNode->left_offset = currNode->right_offset = -1;
    if(h->free_head==-1)//If no invalid node exists
    {
        h->free_head = currOff;
        fseek(fp,0,SEEK_SET);
        fwrite(h,sizeof(tree_t),1,fp);
    }
    else //If list exists already, add the node to the end of the list.
    {
        int prev;
        int temp = h->free_head;
        node_t *tempNode = (node_t*)malloc(sizeof(node_t));
        while(temp != -1)
        {
            fseek(fp,temp,SEEK_SET);
            fread(tempNode,sizeof(node_t),1,fp);
            prev = temp;
            temp = tempNode->left_offset;
        }
        tempNode->left_offset = currOff;
        fseek(fp,prev,SEEK_SET);
        fwrite(tempNode,sizeof(node_t),1,fp);
    }
    fseek(fp,currOff,SEEK_SET);
    fwrite(currNode,sizeof(node_t),1,fp);
}

//Function create a file with the given filename and initialize the header with tree_t structure. If the file already exists, just return the file pointer fp.
FILE* init_tree(const char* filename)
{
    tree_t *h = (tree_t*)malloc(sizeof(tree_t));
    FILE* fp = fopen(filename, "r+");
	if(fp == NULL)
	{
		fp = fopen(filename, "w"); //If file doesnt exist,create one
        h->root = -1;
        h->free_head = -1;
		fwrite(h,sizeof(tree_t),1,fp);
		fclose(fp);
		fp = fopen(filename, "r+");
	}
	fread(h,sizeof(tree_t),1,fp);
	return fp;
}

//Function to close the file pointed to by fp
void close_tree(FILE *fp)
{
    fclose(fp);
}

//Recursive function to print inorder traversal of the tree
void rec_inorder(tree_t *h,int off,FILE *fp)
{
    if(off==-1)
        return;
    else
    {
        node_t *temp = (node_t*)malloc(sizeof(node_t));
        fseek(fp,off,SEEK_SET);
        fread(temp,sizeof(node_t),1,fp);
        rec_inorder(h,temp->left_offset,fp);  //print left sub tree
        printf("%d ",temp->key);
        rec_inorder(h,temp->right_offset,fp);  //print right sub tree
    }
}

//Function to call rec_inorder  
void display_inorder(FILE *fp)
{
    tree_t *h = (tree_t*)malloc(sizeof(tree_t));
    fseek(fp,0,SEEK_SET);
    fread(h,sizeof(tree_t),1,fp);
    int off = h->root;
    rec_inorder(h,off,fp);  //calls rec_inorder which is a recursive function
    printf("\n");
}

//Recursive function to print preorder traversal of the tree
void rec_preorder(tree_t *h,int off,FILE *fp)
{
    if(off==-1)
        return;
    else
    {
        node_t *temp = (node_t*)malloc(sizeof(node_t));
        fseek(fp,off,SEEK_SET);
        fread(temp,sizeof(node_t),1,fp);
        printf("%d ",temp->key);
        rec_preorder(h,temp->left_offset,fp);  //print left sub tree
        rec_preorder(h,temp->right_offset,fp);  //print right sub tree
    }
}
//Function to call rec_preorder  
void display_preorder(FILE *fp)
{
    tree_t *h = (tree_t*)malloc(sizeof(tree_t));
    fseek(fp,0,SEEK_SET);
    fread(h,sizeof(tree_t),1,fp);
    int off = h->root;
    rec_preorder(h,off,fp);  //calls rec_preorder which is a recursive function
    printf("\n");
}

//Function to insert the key into the tree and write to the file
void insert_key(int key, FILE *fp)
{
    tree_t *h = (tree_t*)malloc(sizeof(tree_t));
    fseek(fp,0,SEEK_SET);
    fread(h,sizeof(tree_t),1,fp);

    int off;
    node_t *newNode = (node_t*)malloc(sizeof(node_t));
    int newOff;
    //Check if any ivalid nodes exist. If yes reuse them.
    if(h->free_head == -1) 
    {
        fseek(fp,0,SEEK_END);
        newOff=ftell(fp); 
    }
    else
    {
        newOff = h->free_head;
        node_t *temp = (node_t*)malloc(sizeof(node_t));
        fseek(fp,newOff,SEEK_SET);
        fread(temp,sizeof(node_t),1,fp);
        h->free_head = temp->left_offset;
        fseek(fp,0,SEEK_SET);
        fwrite(h,sizeof(tree_t),1,fp);
    }
    
    newNode->key=key;
    newNode->left_offset = -1;
    newNode->right_offset = -1;

    if(h->root == -1) //check if root exists. If root doesnt exist create one.
    {                 
        h->root=newOff;
        fseek(fp,0,SEEK_SET);
        fwrite(h,sizeof(tree_t),1,fp);
        fseek(fp,newOff,SEEK_SET);
        fwrite(newNode,sizeof(node_t),1,fp);
    }
    else //if root exists, just add the node
    {
        int currOff = h->root;
        int prevOff = -1;
        node_t *currNode = (node_t*)malloc(sizeof(node_t));
        fseek(fp,currOff,SEEK_SET);
        fread(currNode,sizeof(node_t),1,fp);

        while(currOff!=-1)  //Traverse the tree till we find the correct position
        {
            while(currOff!=-1 && currNode->key>key)  //move to the left sub tree
            {
                prevOff = currOff;
                currOff = currNode->left_offset;
                if(currOff!=-1)
                {
                    fseek(fp,currOff,SEEK_SET);
                    fread(currNode,sizeof(node_t),1,fp);
                }
            }

            while(currOff!=-1 && currNode->key<key) //move to the right sub tree
            {
                prevOff = currOff;
                currOff = currNode->right_offset;
                if(currOff!=-1)
                {
                    fseek(fp,currOff,SEEK_SET);
                    fread(currNode,sizeof(node_t),1,fp);
                }
            }
            if(currNode->key==key)  //If the key already exists just return.
            {
                return;
            }
        }

        if(newNode->key<currNode->key) //if the new node is less than the leaf node, we add it as the left child
        {
            currNode->left_offset=newOff;
            fseek(fp,prevOff,SEEK_SET); 
            fwrite(currNode,sizeof(node_t),1,fp);
        }
        else
        {
            currNode->right_offset=newOff; //if the new node is greater than the leaf node, then we add it as the right child
            fseek(fp,prevOff,SEEK_SET); 
            fwrite(currNode,sizeof(node_t),1,fp);
        }
        
        fseek(fp,0,SEEK_SET);
        fwrite(h,sizeof(tree_t),1,fp); //save updated tree_t structure into the file
        fseek(fp,newOff,SEEK_SET);
        fwrite(newNode,sizeof(node_t),1,fp); //write new node into the file
    }
}

//Function to delete the given key from the tree in the file
void delete_key(int key, FILE *fp)
{
    tree_t *h = (tree_t*)malloc(sizeof(tree_t));
    fseek(fp,0,SEEK_SET);
    fread(h,sizeof(tree_t),1,fp);

    if(h->root==-1) //if root doesnt exist then tree doesnt exist.
    {
        return;
    }
    else
    {
        int prevOff = -1; //to keep track of previous visited node offset
        int currOff = h->root; //to keep track of current node offset to be deleted
        node_t *prevNode = (node_t*)malloc(sizeof(node_t));  //to hold the previous visited node
        node_t *currNode = (node_t*)malloc(sizeof(node_t));  //to hold the current node which is to be deleted
        fseek(fp,currOff,SEEK_SET);
        fread(currNode,sizeof(node_t),1,fp);
        if(currNode->key==key && currNode->left_offset==-1 && currNode->right_offset==-1) //if root is the only node that exists then delete it and re-initialise root
        {
            h->root=-1;
            fseek(fp,0,SEEK_SET);
            fwrite(h,sizeof(tree_t),1,fp);
            return;
        }

        while(currOff!=-1 && currNode->key!=key)  //if given key is not the root node
        {
            while(currOff!=-1 && key<currNode->key)  //if the node to be found is lesser than current node then branch left
            {
                prevOff = currOff;
                currOff = currNode->left_offset;
                if(currOff!=-1)
                {
                    fseek(fp,currOff,SEEK_SET);
                    fread(currNode,sizeof(node_t),1,fp);
                }
            }

            while(currOff!=-1 && key>currNode->key)  //if the node to be found is greater than current node then branch right
            {
                prevOff = currOff;
                currOff = currNode->right_offset;
                if(currOff!=-1)
                {
                    fseek(fp,currOff,SEEK_SET);
                    fread(currNode,sizeof(node_t),1,fp);
                }
            }

            if(currOff!=-1 && currNode->key==key)  //if node is found then break
            {
                break;
            }
        }
        fseek(fp,currOff,SEEK_SET);
        fread(currNode,sizeof(node_t),1,fp);
        fseek(fp,prevOff,SEEK_SET);
        fread(prevNode,sizeof(node_t),1,fp);

        if(currOff==-1) //if node doesnt exist then return
        {
            return;
        }
        else if(currNode->left_offset==-1 || currNode->right_offset==-1)
        {
            if(currNode->left_offset==-1 && currNode->right_offset==-1) //if it is a leaf node which is to be deleted 
            {
                if(currNode->key<prevNode->key)  //if it was a left leaf node
                {
                    prevNode->left_offset = -1;
                    fseek(fp,prevOff,SEEK_SET);
                    fwrite(prevNode,sizeof(node_t),1,fp);
                }
                else //if it was a right leaf node 
                {              
                    prevNode->right_offset = -1;
                    fseek(fp,prevOff,SEEK_SET);
                    fwrite(prevNode,sizeof(node_t),1,fp);
                }

            }
            else if(currNode->left_offset==-1) //only right sub tree exists
            {
                if(currNode->key<prevNode->key)  //check if it was a left node
                {
                    prevNode->left_offset = currNode->right_offset;
                    fseek(fp,prevOff,SEEK_SET);
                    fwrite(prevNode,sizeof(node_t),1,fp);
                }
                else//if it was a right node
                {                                                   
                    prevNode->right_offset = currNode->right_offset;
                    fseek(fp,prevOff,SEEK_SET);
                    fwrite(prevNode,sizeof(node_t),1,fp);
                }
            }
            else if(currNode->right_offset==-1) //only left subtree exists
            {
                if(currNode->key<prevNode->key)  //check if it was a left node
                {
                    prevNode->left_offset = currNode->left_offset;
                    fseek(fp,prevOff,SEEK_SET);
                    fwrite(prevNode,sizeof(node_t),1,fp);
                }
                else//if it was a right node
                {                                                   
                    prevNode->right_offset = currNode->left_offset;
                    fseek(fp,prevOff,SEEK_SET);
                    fwrite(prevNode,sizeof(node_t),1,fp);
                }
            }
            
            add_deleted_node(currNode,currOff,h,fp);
        }
        else  //if the node to be deleted has both sub trees, then find the inorder successor suc_node of the node to be deleted and replace it with that.
        {                                                               
            int sucPar_off = currOff;
            node_t *sucPar_node = (node_t*)malloc(sizeof(node_t)); //to hold the parent of inorder suc_node
            int suc_off = currNode->right_offset;
            node_t *suc_node = (node_t*)malloc(sizeof(node_t));   //to hold the inorder suc_node
            fseek(fp,suc_off,SEEK_SET);
            fread(suc_node,sizeof(node_t),1,fp);
            if(suc_node->left_offset==-1)  //If the inorder successor is the right node of the node to be deleted
            {
                sucPar_node->key = suc_node->key;
                sucPar_node->right_offset = suc_node->right_offset;
                fseek(fp,sucPar_off,SEEK_SET);
                fwrite(sucPar_node,sizeof(node_t),1,fp);
                add_deleted_node(suc_node,suc_off,h,fp);
                return;
            }
            
            while(suc_node->left_offset!=-1) //If the right node of the node to be deleted has a left sub tree, then we keep branching left till we reach the last node
            {
                sucPar_off = suc_off;
                suc_off=suc_node->left_offset;
                if(suc_node->left_offset!=-1)
                {
                    fseek(fp,suc_off,SEEK_SET);
                    fread(suc_node,sizeof(node_t),1,fp);
                }
            }
            
            //Replace the node to be deleted with the inorder successor and save all the updated data into the file
            fseek(fp,sucPar_off,SEEK_SET);
            fread(sucPar_node,sizeof(node_t),1,fp);
            currNode->key = suc_node->key;
            sucPar_node->left_offset = -1;
            fseek(fp,sucPar_off,SEEK_SET);
            fwrite(sucPar_node,sizeof(node_t),1,fp); 
            add_deleted_node(suc_node,suc_off,h,fp);   
            fseek(fp,currOff,SEEK_SET);
            fwrite(currNode,sizeof(node_t),1,fp);
        }
    }
}

/*int main(int argc, char const *argv[])
{
    FILE *fp;
	fp = init_tree(argv[1]);

    ins(65);
    ins(25);
    ins(95);
    ins(30);
    ins(80);
    ins(75);
    ins(15);
    ins(40);
    ins(55);
    ins(60);
    ins(85);
    ins(90);
    ins(120);
    delete_key(90, fp);
    delete_key(60, fp);
    ins(90);

    display_inorder(fp);
    display_preorder(fp);

    delete_key(80, fp); // middle node

    display_inorder(fp);
    display_preorder(fp);

    insert_key(80, fp);

    display_inorder(fp);
    display_preorder(fp);

    delete_key(95, fp); // middle node

    display_inorder(fp);
    display_preorder(fp);

    delete_key(111, fp); // not in tree check

    display_inorder(fp);
    display_preorder(fp);

    close_tree(fp);

    fp = init_tree(argv[1]);

    display_inorder(fp);
    display_preorder(fp);

    close_tree(fp);
    
}*/

int main(int argc, char const *argv[])
{
    FILE *fp;
    fp = init_tree(argv[1]);

    ins(65);
    ins(25);
    ins(95);
    ins(30);
    ins(80);
    ins(75);
    ins(15);
    ins(40);
    ins(55);
    ins(60);
    ins(85);
    ins(90);
    ins(120);

    delete_key(90, fp);
    delete_key(60, fp);
    ins(90);

    delete_key(80, fp);
    insert_key(80, fp);
    delete_key(95, fp);

    display_inorder(fp);
    display_preorder(fp);

    delete_key(65, fp);

    display_inorder(fp);
    display_preorder(fp);

    close_tree(fp);

    fp = init_tree(argv[1]);

    display_inorder(fp);
    display_preorder(fp);

    close_tree(fp);
}