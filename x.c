#include <stdio.h>

int strxcmp(const char *s1, const char *s2, unsigned int n)
{
	while (n--) {
		if (!*s1 && !*s2)
			return 0;

		if (!*s1 && *s2)
			return -1;
		else if (*s1 && !*s2)	
			return  1;

		if ( *s1 == *s2) {
			s1++;
			s2++;
			continue;
		}

		return *s1 - *s2;
	}

	return 0;

}

int main(void)
{
	printf("%d \n", strxcmp("firo", "fijue", 3));

	return 0;
}

