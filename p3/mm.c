#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "mm.h"



// Task 1: Flush the cache so that we can do our measurement :)
void flush_all_caches()
{
	// Your code goes here
	if (huge_matrixA == NULL || huge_matrixB == NULL || huge_matrixC == NULL || size <= 0)
	{
		return;
	}
	for (i = 0; i < size; i++)
	{
		asm volatile("clflush (%0)\n\t":: "r"(huge_matrixA + i): "memory");
		asm volatile("clflush (%0)\n\t":: "r"(huge_matrixB + i): "memory");
		asm volatile("clflush (%0)\n\t":: "r"(huge_matrixC + i): "memory");
	}
	asm volatile("sfence\n\t"::: "memory");
}

void load_matrix_base()
{
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	// Load the input
	// Note: This is suboptimal because each of these loads can be done in parallel.
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i));
		fscanf(fin2,"%ld", (huge_matrixB+i));
		huge_matrixC[i] = 0;
	}
}

void free_all()
{
	free(huge_matrixA);
	free(huge_matrixB);
	free(huge_matrixC);
}

void multiply_base()
{
	// Your code here
	//
	// Implement your baseline matrix multiply here.
	for (i = 0; i < row; i++)
	{
		for (j = 0; j < column; j++)
		{
			for (s = 0; s < column; s++)
			{
				huge_matrixC[j+(row*i)] += huge_matrixA[s+(row*i)] * huge_matrixB[i+(row*s)];
			}
		}
	}
	
}

void compare_results()
{
	fout = fopen("./out.in","r");

	long temp1, temp2;

	ftest = fopen("./reference.in","r");
	
	for(i=0;i<((long)SIZEX*(long)SIZEY);i++)
	{
		fscanf(fout, "%ld", &temp1);
		fscanf(ftest, "%ld", &temp2);
		if(temp1!=temp2)
		{
			printf("Wrong solution!");
			exit(1);
		}
	}
	fclose(fout);
	fclose(ftest);
}

void write_results()
{
	// Your code here
	//
	// Basically, make sure the result is written on fout
	// Each line represent value in the X-dimension of your matrix

	fout = fopen("./out.in","w");
	char temp_buff[SIZEBUFFER];
	long buff_length;
	for ( i = 0; i < row; i++){
		for ( j = 0; j < column; j++){
			sprintf(temp_buff, "%ld ", huge_matrixC[j+(i * row)]);
			buff_length = strlen(temp_buff);
			fwrite(temp_buff, sizeof(char), buff_length, fout);
		}
		fwrite("\n", sizeof(char), strlen("\n"), fout);
	}
	fclose(fout);
}

void load_matrix()
{
	// Your code here
	huge_matrixA = malloc(size*sizeof(long));
	huge_matrixB = malloc(size*sizeof(long));
	huge_matrixC = malloc(size*sizeof(long));

	for(i = 0 ; i < size ; i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i));
		fscanf(fin2,"%ld", (huge_matrixB+i));
		huge_matrixC[i] = 0;
	}
}



void multiply()
{
	// Your code here
	for (x = 0; x < row; x += SIZEX)
	{
		for (y = 0; y < column; y += SIZEX)
		{
			for (i = 0; i < row; i++)
			{
				for (j = x; j < x + SIZEX; j++)
				{
					for (s = y; s < y + SIZEX; s++)
					{
						huge_matrixC[(row*i) + (j+x)] += huge_matrixA[(row*i) + (y+s)] * huge_matrixB[(row*s) + (x+j)];
					}
				}
			}
		}
	}
}

void load_data()
{
	fin1 = fopen("./input1.in","r");
	fin2 = fopen("./input2.in","r");
	fout = fopen("./out.in","w");
	ftest = fopen("./reference.in","r");
}

int main()
{

	clock_t s,t;
	double total_in_base = 0.0;
	double total_in_your = 0.0;
	double total_mul_base = 0.0;
	double total_mul_your = 0.0;
	load_data();

	flush_all_caches();

	s = clock();
	load_matrix_base();
	t = clock();
	total_in_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the load = %f seconds\n", total_in_base);

	s = clock();
	multiply_base();
	t = clock();
	total_mul_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the multiply = %f seconds\n", total_mul_base);
	fclose(fin1);
	fclose(fin2);
	fclose(fout);

	flush_all_caches();
	free_all();

	load_data();

	s = clock();
	load_matrix();
	t = clock();
	total_in_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the load = %f seconds\n", total_in_your);

	s = clock();
	multiply();
	t = clock();
	total_mul_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the multiply = %f seconds\n", total_mul_your);
	write_results();
	fclose(fin1);
	fclose(fin2);

	free_all();
	compare_results();

	return 0;
}
