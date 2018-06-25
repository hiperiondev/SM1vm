/*
 * SM1_assembler.h
 *
 *  License: GNU GPL V3
 *
 *  Created on: 24 jun. 2018
 *  Author: Emiliano Augusto Gonzalez (egonzalez.hiperion@gmail.com)
 */

#define UNDER_OVER
//#define CARRY
#define EXTRAREGS
//#define EXTBITS

/////////////////////////////////////////////////////////////////////////////////////

int getWords(char *base, char target[10][20])
{
	int n=0,i,j=0;

	for(i=0;1;i++)
	{
		if(base[i]!=' '){
			target[n][j++]=base[i];
		}
		else{
			target[n][j++]='\0';
			n++;
			j=0;
		}
		if(base[i]=='\0')
		    break;
	}
	return n;

}

int opCmp(char *op, char *value){
	return strcmp(strlwr(op), value);
}

uint16_t sm1_assembly(char* line) {
	int words, value, w;
	char lineSplited[10][20], str[20];
	sscanf(lineSplited[1], "%x", &value);
	words = getWords(line, lineSplited);

	if (opCmp(lineSplited[0], "lit") == 0) {
		if (value < 32768)
			return (0x8000 | value);
		printf("ASSEMBLER ERROR: lit too long\n");
		exit(1);
	}

	if (opCmp(lineSplited[0], "jmp") == 0) {
		if (value < 8192)
			return (0x0000 | value);
		printf("ASSEMBLER ERROR: jmp too long\n");
		exit(1);
	}
	if (opCmp(lineSplited[0], "jmz") == 0) {
		if (value < 8192)
			return (0x2000 | value);
		printf("ASSEMBLER ERROR: jmz too long\n");
		exit(1);
	}
	if (opCmp(lineSplited[0], "cll") == 0) {
		if (value < 8192)
			return (0x4000 | value);
		printf("ASSEMBLER ERROR: cll too long\n");
		exit(1);
	}

	value = 0xffff;
	for (int w=0; w < 32; w++) {
		strcpy(str,ALU[w]);
        removePrefix(str);
		if (opCmp(str, lineSplited[0])) {
			value = w << 8;
			break;
		}
	}
	if (value == 0xffff) {
		printf("ASSEMBLER ERROR: unknown mnemonic\n");
		exit(1);
	}

	value |= 0x6000;

	int pos = 1;
	while (pos < words) {
		for (w=0; w < 4; w++) {
			strcpy(str, FLAGS[w]);
			removePrefix(str);
			if (opCmp(str, lineSplited[pos])) {
				value |= FLAGS_CODE[w];
				break;
			}
		}

		for (w=0; w < 6; w++) {
			strcpy(str, DELTA[w]);
			removePrefix(str);
			if (opCmp(str, lineSplited[pos])) {
				value |= DELTA_CODE[w];
				break;
			}
		}
		pos++;
	}

	return value;
}
