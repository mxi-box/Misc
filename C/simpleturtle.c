/*===========================================*\
|| simpleturtle.c: a simple graphing program ||
\*===========================================*/

#include <stdio.h>
#include <stdlib.h>

#define TRUE	-1
#define FALSE	0

#define PLOT_SIZE	40
#define ACTUAL_SIZE	(PLOT_SIZE*2+1)
#define OFFSET		(PLOT_SIZE)

typedef struct xy
{
	int x;
	int y;
} xy_t;

int main(void)
{
	int i, j;
	char in;
	char out;
	int distance;
	int ret;
	int drawing = FALSE;
	int facing = 0; // 0 = left, 90 = up, 180 = right, 270 = down
	int debug = FALSE;
	char plot[ACTUAL_SIZE][ACTUAL_SIZE];
	xy_t pos = {0, 0};
	xy_t dir = {1, 0};

	for(i = 0; i < ACTUAL_SIZE; i++)
		for(j = 0; j < ACTUAL_SIZE; j++)
			plot[i][j] = '-';

	while((ret = scanf("%c", &in)) != EOF)
	{
		switch(in)
		{
			case 'U':
				drawing = FALSE;
				break;
			case 'D':
				drawing = TRUE;
				break;
			case 'R':
				facing -= 90;
				break;
			case 'L':
				facing += 90;
				break;
			case 'M':
				scanf("%d", &distance);
				if(drawing)
				{
					for(i = 0; i <= distance; i++)
						plot[OFFSET + pos.y][OFFSET + pos.x + dir.x * i] = '#';
					for(i = 0; i <= distance; i++)
						plot[OFFSET + pos.y + dir.y * i][OFFSET + pos.x] = '#';
				}

				pos.x += dir.x * distance;
				pos.y += dir.y * distance;
				break;
			case 'P':		// Print whole plot
				for(i = ACTUAL_SIZE - 1; i >= 0; i--)
				{
					for(j = 0; j < ACTUAL_SIZE; j++)
					{
						putc(out = plot[i][j], stdout);
						putc(out, stdout);
					}
					putc('\n', stdout);
				}
				break;
			case '?':		// Print current position and facing
				printf("@(%d,%d)\n", pos.x, pos.y);
				printf("-> %d\n", facing);
				break;
			case 'X':
				exit(0);
		}
		if(facing < 0)
			facing += 360;
		else if(facing >= 360)
			facing -= 360;
		switch(facing)
		{
			case 0:
				dir.x = 1;
				dir.y = 0;
				break;
			case 90:
				dir.x = 0;
				dir.y = 1;
				break;
			case 180:
				dir.x = -1;
				dir.y = 0;
				break;
			case 270:
				dir.x = 0;
				dir.y = -1;
				break;
		}
	}
}
