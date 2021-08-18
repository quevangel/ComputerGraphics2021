#include <stdio.h>
#include <math.h>
#define auto __auto_type
#define min(a, b) ((a)<(b)?(a):(b))
#define max(a, b) ((a)>(b)?(a):(b))
#define abs(a) ((a)<0?(-(a)):(a))

int main()
{
	int x1, y1, x2, y2;
	scanf(" (%d,%d) , (%d,%d)", &x1, &y1, &x2, &y2);
	auto dx = x2 - x1;
	auto dy = y2 - y1;
	printf("   X   Y\n");
	if (abs(dx) >= abs(dy))
	{
		for(int x = min(x1, x2); x <= max(x1, x2); x++)
		{
			// y = y1 + s * dy / dx;
			auto y = (y1 * dx + (x - x1) * dy)/dx;
			printf("%4d%4d\n", x, y);
		}
	}
	else
	{
		for(int y = min(y1, y2); y <= max(y1, y2); y++)
		{
			auto x = (x1 * dy + (y - y1) * dx)/dy;
			printf("%4d%4d\n", x, y);
		}
	}
}
