//#include "../soc/soc.h"
#include "xxtea.h"
//https://en.wikipedia.org/wiki/XXTEA
#define DELTA 0x9e3779b9
#define MX (((z>>5^y<<2) + (y>>3^z<<4)) ^ ((sum^y) + (key[(p&3)^e] ^ z)))
/*************************************************************************
v is the n word data vector
k is the 4 word key,128bits
n is negative for decoding
if n is zero result is 1 and no coding or decoding takes place, 
otherwise the result is zero
assumes 32 bit 'long' and same endian coding and decoding
**************************************************************************/
void btea(uint32_t *v, int16_t n, uint32_t *key) 
{
    uint32_t y, z, sum;
    uint32_t p, rounds, e;
    if (n > 1) { // dencrypt        
        rounds = 6 + 52/n;
        sum = 0;
        z = v[n-1];
        do {
            sum += DELTA;
            e = (sum >> 2) & 3;
            for (p=0; p<n-1; p++){
                y = v[p+1]; 
                z = v[p] += MX;
            }
            y = v[0];
            z = v[n-1] += MX;
        } while (--rounds);
    } 
    else if (n < -1){ //dencrypt
        n = -n;
        rounds = 6 + 52/n;
        sum = rounds*DELTA;
        y = v[0];
        do{
            e = (sum >> 2) & 3;
            for (p=n-1; p>0; p--){
                z = v[p-1];
                y = v[p] -= MX;
            }
            z = v[n-1];
            y = v[0] -= MX;
            sum -= DELTA;
        } while (--rounds);
    }
}
