#include <time.h>
#include <memory>
#include <iostream>
#include <chrono>
#include <algorithm>
#include <cstdint>
extern "C"
{
#include <immintrin.h>
}

using namespace std;

// COUNTER TYPE //
typedef unsigned long long bench_t;


//static bench_t before;
//static bench_t after;

static inline bench_t cycles_start(void) {
    unsigned int hi, lo;
    __asm__ __volatile__("lfence\n\trdtsc" : "=a"(lo), "=d"(hi) :: "memory");
    return ((bench_t)lo) | (((bench_t)hi) << 32);
}
static inline bench_t cycles_end(void) {
    unsigned int hi, lo;
    __asm__ __volatile__("rdtsc\n\tlfence" : "=a"(lo), "=d"(hi) :: "memory");
    return ((bench_t)lo) | (((bench_t)hi) << 32);
}


float horner(float X,float *coef,long size){
	float ACC=0.0;
	int i;
	for(i=0;i<size;i++){
		ACC=(ACC+coef[i])*X;
	}
	return ACC;	
}
//Tarea
float horner_intrinsic(float X,float *coef,long size){
	float P;
	int i;
	__m256 X256,Y;	
	
	float x2 = X * X;
    float x4 = x2 * x2;
    float x8 = x4 * x4;
	
	X256 = _mm256_set1_ps(x8);
    Y = _mm256_set1_ps(0.0f);

	for(i=0;i<size/8-1;i++){;
		__m256 coef_block = _mm256_load_ps(&coef[i * 8]);
        Y = _mm256_add_ps(Y, coef_block);
        Y = _mm256_mul_ps(Y, X256);
	}
	
	__m256 coef_block_final = _mm256_load_ps(&coef[i * 8]);
    Y = _mm256_add_ps(Y, coef_block_final);
	alignas(32) float R[8];
    _mm256_store_ps(R, Y);
	
	P  = R[7]*X;
	P += R[6]*X*X;
	P += R[5]*X*X*X;
	P += R[4]*X*X*X*X;
	P += R[3]*X*X*X*X*X;
	P += R[2]*X*X*X*X*X*X;
	P += R[1]*X*X*X*X*X*X*X;
	P += R[0]*X*X*X*X*X*X*X*X;
	return P;
}
//Encontre esta forma de medir los tiempos de ejecucion y la adapte al codigo
static float g_sink = 0.0f;   // evita que el compilador borre los bucles

template <class F>
double bench(const char *name, F fn, float X, float *coef, long size,
             int num_iters, int reps = 7, int warmup = 20) {
    using clk = std::chrono::steady_clock;

    float acc = 0.0f;

    // warmup
    for (int w = 0; w < warmup; ++w)
        acc += fn(X, coef, size);

    double best_ms = 1e30;
    for (int r = 0; r < reps; ++r) {
        auto t1 = clk::now();
        for (int j = 0; j < num_iters; ++j)
            acc += fn(X, coef, size);
        auto t2 = clk::now();

        double ms = std::chrono::duration<double, std::milli>(t2 - t1).count();
        best_ms = std::min(best_ms, ms);
    }

    // barrera contra dead-code elimination
    asm volatile("" : : "r"(acc) : "memory");
    g_sink += acc;

    cout << name << ": " << best_ms << " ms  (best of " << reps
         << ", " << num_iters << " iters)\n";
    return best_ms;
}

int main(){
	alignas(32) float coef[4000];
    const float X = 1.1f;
    const int num_iters = 100000;
	float R;
	int i,num_trails = 100000;
	
	//clock_t t1, t2;
	
	srand(time(NULL));	

	float *coeficientes;
	int j;
	coeficientes = (float *)_mm_malloc(10000*sizeof(float), 32);

	for(j=0;j<1;j++){
		
		for(i=0;i<10000;i++){
			coeficientes[i]=(float)(rand()%1000)/1000;
			if(i<10)
				cout<< coeficientes[i] <<endl;
		}
		R=horner(X,coeficientes,10000);
		cout<< R <<endl;
		R=horner_intrinsic(X,coeficientes,10000);
		cout<< R <<endl;		
	}
	
	//t1 = clock();
	//for(j=0;j<num_trails;j++)	
		//R=horner(X,coeficientes,10000);
	//t2 = clock();
  	//float diff = (((float)t2 - (float)t1) / CLOCKS_PER_SEC );
  	//cout<<"Time taken: "<<diff<<endl;
	

	//t1 = clock();
	//for(j=0;j<num_trails;j++)	
		//R=horner_intrinsic(X,coeficientes,10000);
	//t2 = clock();
  	//diff = (((float)t2 - (float)t1) / CLOCKS_PER_SEC );
  	//cout<<"Time taken: "<<diff<<endl;

	double ts = bench("horner          ", horner,           X, coeficientes, 10000, num_trails);
    double tv = bench("horner_intrinsic", horner_intrinsic, X, coeficientes, 10000, num_trails);

    cout << "\nspeedup (scalar/vector): " << (ts / tv) << "x\n";


	
		
	_mm_free(coeficientes);

}
