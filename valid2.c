#include <stdio.h>
#include <stdlib.h>

int r0, r1, r7;

/*
0x178b JT reg0 0x1793
0x178e ADD reg0 reg1 1
0x1792 RET
0x1793 JT reg1 0x17a0
0x1796 ADD reg0 reg0 32767
0x179a SET reg1 reg7
0x179d CALL 0x178b
0x179f RET
0x17a0 PUSH reg0
0x17a2 ADD reg1 reg1 32767
0x17a6 CALL 0x178b
0x17a8 SET reg1 reg0
0x17ab POP reg0
0x17ad ADD reg0 reg0 32767
0x17b1 CALL 0x178b
0x17b3 RET
...
0x156b SET reg0 4
0x156e SET reg1 1
0x1571 CALL 0x178b
...
0x1573 EQ reg1 reg0 6 ; reg1 needs to equal '5' when call returns
*/

void valid2() {
	//printf("R0: %d, R1: %d\n", r0, r1);
	int t;
	if (r0 != 0) {
		if (r1 != 0) {
			t = r0;
			r1 = (r1 + 32767) % 32768;
			valid2();
			r1 = r0;
			r0 = t;
			r0 = (r0 + 32767) % 32768;
			valid2();
			return;
		}
		r0 = (r0 + 32767) % 32768;
		r1 = r7;
		valid2();
		return;
	}
	r0 = (r1 + 1) % 32768;
}

void valid4() {
	//printf("R0: %d, R1: %d\n", r0, r1);
	int t;
	if (r0 != 0) {
		if (r1 != 0) {
			t = r0;
			r1 = (r1 + 32767) % 32768;
			valid4();
			r1 = r0;
			r0 = t;
			r0 = (r0 + 32767) % 32768;
			valid4();
			return;
		}
		r0 = (r0 + 32767) % 32768;
		r1 = (r1 + r7 + 1) % 32768;
		valid4();
		return;
	}
	r0 = (r1 + 1) % 32768;
}

int valid5(int r0, int r1) {
	if (r0 != 0) {
		if (r1 != 0) {
			int t = r0;
			r1 = valid5(r0, (r1 + 32767) % 32768);
			r0 = (r1 + 1) % 32768;
			return valid5((t + 32767) % 32768, r0);
		}
		return valid5((r0 + 32767) % 32768, r7);
		}
	return r1;
}

int valid6(int r0, int r1) {
	if (r0 != 0) {
		if (r1 != 0) {
			return valid6(r0-1, valid6(r0,r1-1)+1);
		}
		return valid6(r0-1, r7);
		}
	return r1 % 32768;
}

/*
//Tested for R0 == 3; R1 == 1
int n_gen(int step) {
	if (step == 2) {
		return 25;
	}
	else {
		return step*6 + 2 + n_gen(step-1);
	}
}

int valid3(int n) {
	if (n == 2) {
		return 38;
	}
	else {
		return (valid3(n-1) + n_gen(n)) % 32768;
	}
}
*/

int valid7(int r0, int r1) {
	if (r0 == 1 && r1 == 0)
		return r7;
	if (r0 == 1 && r1 > r0)
		return (r1 + r7) % 32768;
	while (r0 != 0){
		if (r1 != 0)
			return valid7(r0-1, valid7(r0,r1-1)+1);
		return valid7(r0-1, r7);
	}
	return r1 % 32768;
}


int main(int argc, char**argv) {
	int k = 3;
	while(k < 32768) {
		r0 = 4;
		r1 = 1;
		r7 = k;
		printf("%d, %d\n", k, valid7(r0, r1));
		if(r1 == 5) {
			printf("Solution K is: %d", k);
			return 0;
		}
		//printf("%d, %d\n",r1,r7);
		k = k + 1;
	}
	/*
	int i,j,k;
	for(i=0; i<5; i++) {
		for(j=0; j<5; j++) {
			for(k=0; k<10; k++) {
				r0 = i;
				r1 = j;
				r7 = k;
				valid2();
				printf("%d, %d, %d : %d, %d, %d\n",i,j,k,r0,r1,r7);
	q		}
		}
	}
	*/
	return 0;
}

//0 : 2,1,0
//1 : 32765,32764,1
//2 : 13234,13233,2
//3 : 21843,21842,3
