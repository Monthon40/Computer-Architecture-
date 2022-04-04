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
long it ,jt , kt, xt, yt;
long i1 ,j_1 , k1, x1, y_1, s1;
long i2 ,j2 , k2, x2, y2, s2;
#define SIZEBUFFER 16
pthread_t thred1,thred2,thred3,thred4,mthred[4];

long fisrt_size,last_size;
long from_t1,from_t2,from_t3,from_t4;
long row_t1 ,row_t2,row_t3,row_t4 ,first_row, first_column , last_row , last_column;
long first_y_sizex , first_x_sizex , last_y_sizex , last_x_sizex;
long *first_value , *lastsss;

pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;

// Any helper functions go here
void load_data();
void before_load();
void* load_matrix_base_pthred(void* arg);
void* multiply_base_pthred(void* arg);
void* load_matrix_pthread(void* arg);
void* multiply_pthred1(void* arg);
void* multiply_pthred2(void* arg);


