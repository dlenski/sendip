/* compact.c - function to convert hex/octal/decimal/raw string to raw
 * ChangeLog since initial release in sendip 2.1.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include "sendip_module.h"

int compact_string(char *data_out) {
	char *data_in = data_out;
	int i=0;
	if(*data_in=='0') {
		data_in++;
		if(*data_in=='x' || *data_in=='X') {
			/* Hex */
			char c='\0';
			data_in++;
			while(*data_in) {
				if(*data_in>='0' && *data_in<='9') {
					c+=*data_in-'0';
				} else if(*data_in>='A' && *data_in<='F') {
					c+=*data_in-'A'+10;
				} else if(*data_in>='a' && *data_in<='f') {
					c+=*data_in-'a'+10;
				} else {
					fprintf(stderr,"Character %c invalid in hex data stream\n",
							  *data_in);
					return 0;
				}
				if( i&1) {
					*(data_out++)=c;  // odd nibble - output it
					c='\0';
				} else {
					c<<=4;   // even nibble - shift to top of byte
				}
				data_in++; i++;
			}
			*data_out=c; // make sure last nibble is added
			i++; i>>=1;  // i was a nibble count...
			return i;
		} else {
         /* Octal */
			char c='\0';
			while(*data_in) {
				if(*data_in>='0' && *data_in<='7') {
					c+=*data_in-'0';
				} else {
					fprintf(stderr,"Character %c invalid in octal data stream\n",
							  *data_in);
					return 0;
				}
				if( (i&3) == 3 ) {
					*(data_out++)=c;  // output every 4th char
					c='\0';
				} else {        // otherwise just shift it up
					c<<=2;
				}
				data_in++; i++;
			}
			*data_out=c;     // add partial last byte
			i+=3; i>>=2;
			return i;
		}
	} else {
		/* String */
		return strlen(data_in);
	}
}

/* @@ Since I'm using the "string and/or rand" business for filling
 * out several header data areas, I decided to extract all of this
 * into routines here.
 *
 * Note the handling of space is slightly screwy - compact_string
 * above overwrites its argument in place, since it knows that
 * no matter what, the string it produces can be no longer than
 * its argument. randombytes, however, uses a static area, since
 * the calling argument there (something like r32) will generally
 * be much shorter than the string produced.
 *
 * In practice, in both cases the string returned will be immediately
 * copied into an allocated area, so the differences in string handling
 * don't matter. But this should be kept in mind if these routines
 * are used elsewhere.
 */
/* @@ Return a pointer to a string of random bytes. Note this is a
 * static area which is overwritten at each call.
 */
u_int8_t *
randombytes(int length)
{
	static u_int8_t answer[MAXRAND];
	int i;

	/* Sanity check */
	if (length > MAXRAND) {
		usage_error("Random data too long to be sane\n");
		return NULL;
	}
	for (i=0; i < length; ++i)
		answer[i] = (u_int8_t)random();
	/* Zero-pad out to 64-bit boundary */
	for (; i&07; ++i)
		answer[i] = 0;
	return answer;
}

/* @@ Yes, well, not the world's most brilliant name, but this
 * does the standard string argument handling. The output
 * may either be the transformed input or a static area.
 * The return value is the length of the output.
 */
int
compact_or_rand(char *input, char **output)
{
	int length=0;

	if (!input || !output) return 0;
	switch (*input) {
	case 'r':
		length = atoi(input+1);
		*output = (char *)randombytes(length);
		if (!*output) return 0;
		return length;
	default:
		length = compact_string(input);
		*output = input;
		return length;
	}
}
