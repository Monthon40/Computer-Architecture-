#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <pthread.h>
#include "mm-mt.h"
#include <unistd.h>


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

void before_load(){
	huge_matrixA = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixB = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
	huge_matrixC = malloc(sizeof(long)*(long)SIZEX*(long)SIZEY);
}
void load_matrix_base()
{
	// Load the input
	// Note: This is suboptimal because each of these loads can be done in parallel.
	for(i = 0 ;i < size ; i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i));
		huge_matrixC[i] = 0;
	}
}

void* load_matrix_base_pthred(void* arg){
	for(j=0 ; j < size ;j++)
	{
		fscanf(fin2,"%ld", (huge_matrixB+i));
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
	first_row = row / 2;
	first_column = column /2;

	for (i = 0; i < first_row; i++)
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

void* multiply_base_pthred(void* arg){
	pthread_mutex_lock( &mutex1 );
	for (it = first_row; it < row; it++)
	{
		for (jt = first_column; jt < column; jt++)
		{
			for (kt = first_column; kt < column; kt++)
			{
				huge_matrixC[jt+(row*it)] += huge_matrixA[kt+(row*it)] * huge_matrixB[it+(row*kt)];
			}
		}
	}
	pthread_mutex_unlock( &mutex1 );
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
	fisrt_size = size / 2;
	for(i = 0 ; i < size ; i++)
	{
		fscanf(fin1,"%ld", (huge_matrixA+i));
		huge_matrixC[i] = 0;
	}
}
void* load_matrix_pthread(void* arg)
{
	// Your code here
	for(j = 0 ; j < size ; j++)
	{
		fscanf(fin2,"%ld", (huge_matrixB+j));
	}
}



void* multiply_pthred1(void* arg)
{
	// Your code here
	pthread_mutex_lock( &mutex1 );
	from_t1 = 0;
	row_t1 = (row*1) / 2;
	for (x1 = 0; x1 < row; x1 += SIZEX)
	{
		for (y_1 = 0; y_1 < column; y_1 += SIZEX)
		{
			for (i1 = from_t1; i1 < row_t1; i1++)
			{
				for (j_1 = x1; j_1 < x1 + SIZEX; j_1++)
				{
					for (s1 = y_1; s1 < y_1 + SIZEX; s1++)
					{
						huge_matrixC[(row*i1) + (j_1+x1)] += huge_matrixA[(row*i1) + (y_1+s1)] * huge_matrixB[(row*s1) + (x1+j_1)];
					}
				}
			}
		}
	}
	pthread_mutex_unlock( &mutex1 );
}

void* multiply_pthred2(void* arg)
{
	// Your code here
	pthread_mutex_lock( &mutex1 );
	from_t2 = row_t1;
	row_t2 = (row*2) / 2;

	for (x2 = 0; x2 < row; x2 += SIZEX)
	{
		for (y2 = 0; y2 < column; y2 += SIZEX)
		{
			for (i2 = from_t2; i2 < row_t2; i2++)
			{
				for (j2 = x2; j2 < x2 + SIZEX; j2++)
				{
					for (s2 = y2; s2 < y2 + SIZEX;s2++)
					{
						huge_matrixC[(row*i2) + (j2+x2)] += huge_matrixA[(row*i2) + (y2+s2)] * huge_matrixB[(row*s2) + (x2+j2)];
					}
				}
			}
		}
	}
	pthread_mutex_unlock( &mutex1 );
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
	before_load();
	pthread_create(&thred1, NULL , load_matrix_base_pthred ,NULL);
	load_matrix_base();
	pthread_join(thred1,NULL);
	t = clock();
	total_in_base += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("[Baseline] Total time taken during the load = %f seconds\n", total_in_base);
	

	s = clock();
	pthread_create(&thred2, NULL , multiply_base_pthred ,NULL);
	multiply_base();
	pthread_join(thred2,NULL);
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
	before_load();
	pthread_create(&thred3, NULL , load_matrix_pthread ,NULL);
	load_matrix();
	pthread_join(thred3,NULL);
	t = clock();
	total_in_your += ((double)t-(double)s) / CLOCKS_PER_SEC;
	printf("Total time taken during the load = %f seconds\n", total_in_your);

	s = clock();
	pthread_create(&mthred[1], NULL , multiply_pthred1 ,NULL);
	pthread_create(&mthred[2], NULL , multiply_pthred2 ,NULL);
	pthread_join(mthred[1],NULL);
	pthread_join(mthred[2],NULL);
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

// Tested with 2500 arrays.
//The result of implementing the multi-thread multiply only reduced the "[Baseline] Total time taken during the multiply"
//but not the Total time taken during the multiply