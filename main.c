/* -----------------------------------------------------------------------
*  To Do: find a way through a maze using the depth-first search algorithm
*  -----------------------------------------------------------------------
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int debugMode = 0;

typedef struct mazeStruct
{
	//char arr[32][32];		// allows for a maze of size 30*30 plus outer walls
	char **arr;				// allows for a dynamic 2-D maze of any size
	int xsize, ysize;
	int xstart, ystart;
	int xend, yend;
} maze;

typedef struct stackStruct
{
	int x;	//x coordinate
	int y;	//y coordinate
	int v;	//visited
	struct stackStruct* next;
} Node;

void pop(Node **top)
{
	Node *temp = *top;
	
	if(*top != NULL)
	{
		*top = (*top)->next;
		free(temp);
	}
	else
	{
		printf("Stack is empty!\n");
	}
}

void push(Node **top, int x, int y, int v)
{
	Node *temp = (Node *)malloc( sizeof(Node) );
	temp->x = x;
	temp->y = y;
	temp->v = v;
	temp->next = NULL;

	if(*top != NULL)
	{
		temp->next = *top;
	}

	*top = temp;
}

void printStack(Node *top)
{
	if(top == NULL)
	{	
		return;
	}
	printStack(top->next);
	printf("X: %d, Y: %d\n", top->x, top->y);
}

int main(int argc, char **argv)
{
	maze m1;

	int xpos, ypos;
	int i, j;

	Node *top = NULL;

	FILE *src;

	// DebugMode Check
	if(argc > 0)
	{
		for(i = 1; i < argc; i++)
		{
			if(strcmp(argv[i], "-d") == 0)
			{
				debugMode = 1;
				
				if(i = 1)
					argv[1] = argv[2];
			}
		}
	}

	
	/*if(argc != 2)
	{
		printf("Usage: %s <input file name>\n", argv[0]);
		exit(-1);
	}*/

	// Verify the proper number of commmand line arguments were given
	if(argc == 1)
	{
		printf("No file given!\n");
		return 1;
	}

	// Try to open the input file
	if((src = fopen(argv[1], "r")) == NULL)
	{
		printf("Can't open the input file: %s\n", argv[1]);
		exit(-1);
	}

	// Read in the size, strting and ending positions in the maze
	fscanf(src, "%d %d", &m1.xsize, &m1.ysize);
	while(m1.xsize <= 0 || m1.ysize <= 0)
	{
		fprintf(stderr, "Error: The size must be greater than 0!\n");
		fscanf(src, "%d %d", &m1.xsize, &m1.ysize);
	}
	fscanf(src, "%d %d", &m1.xstart, &m1.ystart);

	while(m1.xstart > m1.xsize || m1.ystart > m1.ysize)
	{
		fprintf(stderr, "Error: The start value is bigger than the size of the board!\n");
		fscanf(src, "%d %d", &m1.xstart, &m1.ystart);
	}
	fscanf(src, "%d %d", &m1.xend, &m1.yend);
	
	while(m1.xend > m1.xsize || m1.yend > m1.ysize)
	{
		fprintf(stderr, "Error: The end value is bigger than the size of the board!\n");
		fscanf(src, "%d %d", &m1.xend, &m1.yend);
	}

	// Print them out to verify the input
	printf("size: %d, %d\n", m1.xsize, m1.ysize);
	printf("start: %d, %d\n", m1.xstart, m1.ystart);
	printf("end: %d, %d\n", m1.xend, m1.yend);

	// Allocate the maze
	m1.arr = malloc( (m1.xsize + 2) * sizeof(char *) );
	for(i = 0; i < (m1.xsize + 2); i++)
	{
		m1.arr[i] = malloc( (m1.ysize + 2) * sizeof(char) );
	}

	// Initialize the maze to empty
	for(i = 0; i < m1.xsize + 2; i++)
		for(j = 0; j < m1.ysize + 2; j++)
			m1.arr[i][j] = '.';

	// Mark the borders of the maze with *'s
	for(i = 0; i < m1.xsize + 2; i++)
	{
		m1.arr[i][0] = '*';
		m1.arr[i][m1.ysize + 1] = '*';
	}
	for(i = 0; i < m1.ysize + 2; i++)
	{
		m1.arr[0][i] = '*';
		m1.arr[m1.xsize + 1][i] = '*';
	}

	// Mark the starting and ending positions in the maze
	m1.arr[m1.xstart][m1.ystart] = 's';
	m1.arr[m1.xend][m1.yend] = 'e';

	// Mark the blocked positions in the maze with *'s
	while(fscanf(src, "%d %d", &xpos, &ypos) != EOF)
	{
		m1.arr[xpos][ypos] = '*';

		if(xpos == m1.xstart && ypos == m1.ystart)
		{
			fprintf(stderr, "Error: a * is located in the start position x = %d, y = %d!\n", xpos, ypos);
			m1.arr[xpos][ypos] = 's';
			fscanf(src, "%d %d", &xpos, &ypos);
		}
		if(xpos == m1.xend && ypos == m1.yend)
		{
			fprintf(stderr, "\n Error: a * is located in the end position x = %d, y = %d!\n", xpos, ypos);
			m1.arr[xpos][ypos] = 'e';
			fscanf(src, "%d %d", &xpos, &ypos);
		}
		while(xpos > m1.xsize || ypos > m1.ysize)
		{
			fprintf(stderr, "Error: The location x = %d, y = %d is not on the board!\n", xpos, ypos);
			fscanf(src, "%d %d", &xpos, &ypos);
		}
	}

	// Print out the initial maze
	for(i = 0; i < m1.xsize + 2; i++)
	{
		for(j = 0; j < m1.ysize + 2; j++)
			printf("%c", m1.arr[i][j]);
		printf("\n");
	}
	printf("\n");

	int win = 0;

	push(&top, m1.xstart, m1.ystart, 1);
	while(top != NULL && win != 1)
	{
		if(top->x == m1.xend && top->y == m1.yend)
		{
			printf("Solution found!\n\n");
			win = 1;
			break;
		}
		else if(m1.arr[top->x + 1][top->y] != '*')
		{
			m1.arr[top->x + 1][top->y] = '*';
			push(&top, top->x + 1, top->y, 1);
		}
		if(m1.arr[top->x - 1][top->y] != '*')
		{
			m1.arr[top->x - 1][top->y] = '*';
			push(&top, top->x - 1, top->y, 1);
		}
		else if(m1.arr[top->x][top->y + 1] != '*')
		{
			m1.arr[top->x][top->y + 1] = '*';
			push(&top, top->x, top->y + 1, 1);
		}
		else if(m1.arr[top->x][top->y - 1] != '*')
		{
			m1.arr[top->x][top->y - 1] = '*';
			push(&top, top->x, top->y - 1, 1);
		}
		else
			pop(&top);
	}

	if(top == NULL)
		printf("There is no solution!\n");
	else
	{
		printf("Solution path: \n");
		printStack(top);
	}

	for(i = 0; i < m1.xsize; i++)
	{
		free(m1.arr[i]);
	}

	free(m1.arr);
}




