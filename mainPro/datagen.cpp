#include<iostream>
#include<stdio.h>

int main()
{
	FILE *fp=fopen("mainfile.txt","w");
	for(int i=1;i<=5000;i++)
		{
			fprintf(fp,"This is a sample file... : %d\n",i);
		}
	fclose(fp);

return 0;
}
