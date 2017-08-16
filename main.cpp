#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#define DEBUG 0

float maxerr = 0.0;

/*
   10-bit minifloat
   6-bit mantissa (unsigned)
	-1  -2   -3    -4     -5      -6        -7         -8
	0.5 0.25 0.125 0.0625 0.03125 0.0078125 0.00390625 0.001953125
   4-bit exponent
*/

float mini2float(u_int16_t mini) {
	union {
		float f;
		unsigned df;
	} f;

	if( mini == 0 )
		return 0.0;

	int miniexp = (mini >> 7) & 0x000F;
	int mant    = (mini)      & 0x003F;

	int exp = miniexp + 116;

	f.df = (exp << 23) | (mant << 17);

	if( DEBUG )
	printf("mini  %03X is u %08X miniexp %4d exp %4d mant %8d float %7.5f\n",
		mini, f.df, miniexp, exp, mant, f.f);
 
	return f.f;
}

u_int16_t float2mini(float x) {
	union {
		float f;
		unsigned df;
	} f;

	f.f = x;

	int exp = (f.df & 0x7F800000 ) >> 23;
	int mant= (f.df)       & 0x07FFFFF;

	// add the missing 24th bit
	float test = (mant + 0x0800000) * pow(2, -23 + exp - 127);

	// we'll call this 0
	if( -11 > (exp - 127) ) {
		return 0;
	}

	// valid range for our exponent.  Range of 15
	assert( -11 <= (exp - 127) && (exp - 127) <= 4 );

	int miniexp = (exp - 116) & 0x0F;

	u_int16_t mini = (miniexp << 7) | ((mant >> 17) & 0x003F);

	if( DEBUG )
	printf("float %7.5f is u %08X miniexp %4d exp %4d mant %8d test %7.5f mini %03X\n", 
		x, f.df, miniexp, exp, mant, test, mini);	

	return mini;
}

void runtest(float t) {
	u_int16_t mini;
	float     f;

	mini = float2mini(t);
	f    = mini2float(mini);

	float delta = t - f;

	float err = delta / t;

	if( err > maxerr )
		maxerr = err;

	printf("test %7.5f mini %03X float %7.5f delta %9g err %0.4f\n\n",
		t, mini, f, delta, err);
}

int main(int argc, char **argv) {
	
	float     test;

	maxerr = 0.0;

	runtest(0.0);
	for( test = 0.0005; test < 9.0; test += 0.0001 )
		runtest(test);

	printf("maxerr %0.6f\n", maxerr);
}

