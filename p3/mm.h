#define SIZEX 2500
#define SIZEY 2500

long * huge_matrixA;
long * huge_matrixB;
long * huge_matrixC;

FILE *fin1, *fin2, *fout, *ftest;

void flush_all_caches();
void load_matrix_base();
void free_all();
void multiply_base();
void compare_results();

// Your job is to finish these three functions in the way that it is fast
void write_results();
void load_matrix();
void multiply();

// Any additional variables needed go here
long row = (long) SIZEX;
long column = (long) SIZEY;
long size = (long) SIZEX * (long) SIZEY;
long i , j , k , x , y, s;
#define SIZEBUFFER 16

// Any helper functions go here
void load_data();
