#include <stdio.h>
#include <stdlib.h>

struct NodeNetwork
{
    int x, y;

    struct Node *nextNode;
};

typedef struct NodeNetwork *node;

node setupNode()
{
    node tempNode;

    tempNode = (node)malloc(sizeof(struct NodeNetwork)); //allocates enough memory using MALLOC()

    tempNode->nextNode = NULL; //next node is null for new node because new node is last in the network.

    return tempNode;
}

node addNode(node head, int newX, int newY)
{
    node tempNode, p;

    tempNode = setupNode();

    tempNode->x = newX;
    tempNode->y = newY;

    if(head == NULL)
    {
        head = tempNode;
    }
    else
    {
        p = head;
        while(p->nextNode != NULL)
        {
            p = p->nextNode;
        }
        p-> nextNode = tempNode;
    }
    return tempNode;
}

int main()
{
    node head = setupNode();
    node secondNode = addNode(head, 5, 3);
    node thirdNode =  addNode(head, 6, 4);

    printf("2nd node %d", secondNode->x);
}



