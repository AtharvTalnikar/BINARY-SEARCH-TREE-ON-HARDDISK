# BINARY-SEARCH-TREE-ON-HARDDISK
Implement a binary search tree on a harddisk
Program to construct and destroy a BST directly on the harddisk. All operations on the tree ara updated to the file directly.
When inserting a node to the tree, the very first task is to find free space(offset) on the file using the free_head variable to write the new node. If the value doesn't 
exist already, we add the node at that offset. If the value of variable root is -1, that means tree doesnt exist. Save the root offset.

We read the root from the file and hence begin to traverse the tree. the left_offset and right_offset know
the offset position of the two children of that particular node. Hence we traverse the tree till we
find the right position as to where it has to be inserted. At every instance we keep accessing and 
reading the data from the file. Once we add the node to the tree, we write the node onto the file and 
update the file.
When it comes to deleting the node from tree, we again check if root exists, if it doesnt then there 
is no tree. Else if root is present, we again traverse the tree till we find the node. Once the node is found, we delete it.
There are 5 cases which can occur while deleting a node:
1. Delete root node
2. Delete leaf node
3. Delete node with only left sub-tree
4. Delete node with only right sub-tree
5. Delete node with both sub-trees(by replacing the node to be deleted with the inorder successor)
All changes are updated into the file every time.

display_preorder and display_inorder functions are used to display the respective traversals by just reading nodes through the file in an order an printing them. 
