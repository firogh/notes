#include <stdio.h>
#include <string.h>

// Runtime: 856 ms, faster than 5.03% of C online submissions for Longest Substring Without Repeating Characters.
int longest_substring(int start, char *s,int n)
{
    int i,j,l;
    l = n-start;
    int k=n;
    //printf("=>%d %d %d\n", start,k,n);
    for (i=start; i <= n; i++) {
      //  printf("%d %d %d\n", i,j,l);
        for (j=i+1; j<=k; j++) {
            if (s[i] ==s[j]) {
                break;
            }    
        }
        if (j<=k) {
                k = j-1;
                l = j - start;
        }
    }   
    
    return l;
}

int lengthOfLongestSubstring(char * s){   
    int i, j, k,m,l=0,n=0;
    n = strlen(s);
    if (n<=1)
        return n;
    
    for (i = 0; i<=n; i++) {
        m = longest_substring(i,s,n);
        //printf("%d %d %d\n", i, m, n); 
        if (l<m)
            l=m;
        if (l>=  n-i)
            break;    
    }   
    return l;
}
int main()
{

 int i = lengthOfLongestSubstring("bbbbb");

	printf("%d\n", i);
	return 0;
}
