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
*/

void valid2() {
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

int main(int argc, char**argv) {
	r7 = 0;
	/*while(r7 < 10) {
		r0 = 2;
		r1 = 5;
		valid2();
		printf("%d,%d,%d\n",r0,r1,r7);
		r7 = r7 + 1;
	}*/
	int i,j,k;
	for(i=0; i<5; i++) {
		for(j=0; j<5; j++) {
			for(k=0; k<10; k++) {
				r0 = i;
				r1 = j;
				r7 = k;
				valid2();
				printf("%d, %d, %d : %d, %d, %d\n",i,j,k,r0,r1,r7);
			}
		}
	}
	return 0;
}

//0 : 2,1,0
//1 : 32765,32764,1
//2 : 13234,13233,2
//3 : 21843,21842,3
