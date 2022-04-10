#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <conio.h> 

#define size 128

typedef struct treeNd {
    float freq;
    char c;
    char code[size];
    struct treeNd *left;
    struct treeNd *right;

} treenodes;

void find_min(treenodes *d[], float *min, int *minIndex, float *secMin, int *secMinIndex) {
    int i, k;
    k = 0;
    *minIndex = -1;

    while (k < 26 && d[k] == NULL) k++;

    *minIndex = k;
    *min = d[k]->freq;

    for ( i = k ; i < 26; i ++ ) {
        if ( d[i] != NULL && d[i]->freq < *min ) {
            *min = d[i]->freq;
            *minIndex = i;
        }
    }

    k = 0;
    *secMinIndex = -1;

    while ((k < 26 && d[k] == NULL) || (k == *minIndex && d[k] != NULL))
    k++;

    *secMin = d[k]->freq;
    *secMinIndex = k;

    if (k == *minIndex)
    k ++;

    for ( i = k ; i < 26; i ++ ) {
        if ( d[i] != NULL && d[i]->freq < *secMin && i != *minIndex ) {
            *secMin = d[i]->freq;
            *secMinIndex = i;
        }
    }
}

void encode(treenodes *node, treenodes **chr, char dir, short level, char* code) {
    int n;
    if ( node != NULL ) {
        if ((n = strlen(code)) < level) {
            if (dir == 'R') {
                strcat(code, "1");
            } else {
                if (dir == 'L') {
                    strcat(code, "0");
                }
            }
        } else {
            if (n >= level) {
                code[n - (n - level) - 1] = 0;
                if (dir == 'R') {
                    strcat(code, "1");
                } else {
                    if (dir == 'L') {
                        strcat(code, "0");
                    }
                }
            }
        }
        if (node->c >= 'A' && node->c <= 'Z') {
            strcpy(node->code, code);
            strcpy(chr[node->c - 'A']->code, code);
        }
        encode(node->left, chr, 'L', level + 1, code);
        encode(node->right, chr, 'R', level + 1, code);
    }
}

void huffman(char *inTxt ,char *outfile ){
	
	char *key=malloc(1000);
	key[0]='>';
	treenodes *d[26], *chr[26];
    char str[size];
    int control;
    int  k, ind;
    float min, secMin;
    int minIndex, secMinIndex;
    int charNum = 0;
	treenodes *tree;

	FILE *in = fopen(inTxt, "r");
    FILE *out;

        int i;
        for (i = 'A'; i <= 'Z'; i++) {
            ind = i - 'A';
            d[ind] = NULL;
        }


        int strLength;
        charNum = 0;
        control = fgets(str, size, in) != NULL;

		while(!feof(in) || control) {
            strLength = strlen(str);
                for (i = 0; i <strLength; i ++ ) {
                str[i] = toupper(str[i]);
                if (str[i] >= 'A' && str[i] <= 'Z') {
                    charNum ++;
                    ind = str[i] - 'A';
                    if (d[ind] == NULL) {
                        d[ind] = malloc(sizeof(treenodes));
                        d[ind]->c = str[i];
                        d[ind]->freq = 1;
                        d[ind]->left = d[ind]->right = NULL;
                    } else {
                        d[ind]->freq += 1;
                    }
                }
            }
            if (control) {
                control = fgets(str, size, in) != NULL;
            }
        }

    fclose(in);

    for ( i = 0, strLength = 0 ; i < 26 ; i ++ ) {
        chr[i] = d[i];
        if (d[i] != NULL) {
            d[i]->freq /= charNum;
            
            strLength ++;
        }
    }

    int j = 1;
    do {
        find_min(d, &min, &minIndex, &secMin, &secMinIndex);

        if (minIndex != -1 && secMinIndex != -1 && minIndex != secMinIndex) {
            treenodes *temp ;
            tree =malloc(sizeof(treenodes));
            tree->freq = d[minIndex]->freq + d[secMinIndex]->freq;
            tree->c = j;
            tree->left = d[minIndex];
            temp =  malloc(sizeof(treenodes));
            temp ->c = d[secMinIndex]->c;
            temp ->freq = d[secMinIndex]->freq;
            temp ->left = d[secMinIndex]->left;
            temp ->right = d[secMinIndex]->right;
            tree->right = temp ;

            d[minIndex] = tree;
            d[secMinIndex] = NULL;
        }
        j ++;
    } while( j < strLength );

    for ( i = 0 ; i < 26 ; i ++ ) {
        if (d[i] != NULL)  {
            char code[size];
            strcpy(code, "");
            encode(tree = d[i], chr, 0, 0, code);
            treeScan(d[i],key);
            break;
        }
    }
    in = fopen(inTxt, "r");
    out = fopen(outfile, "w");
    control = fgets(str, size, in) != NULL;
    while(!feof(in) || control) {
        strLength = strlen(str);
        for (i = 0; i < strLength ; i ++ ) {
            str[i] = toupper(str[i]);
            if (str[i] >= 'A' && str[i] <= 'Z') {
                ind= str[i] - 'A';
                fputs(chr[ind]->code, out);
            }
        }
        if (control) {
            control = fgets(str, size, in) != NULL;
        }
    }

    fclose(in);
    fclose(out);

    //printf("\nbefore --> %d bit\n", charNum * 8);

    out = fopen(outfile, "r");
    control = fgets(str, size, out) != NULL;
    charNum = 0;
    while(!feof(out) || control) {
        charNum += strlen(str);
        if (control) {
            control = fgets(str, size, out) != NULL;
        }
    }
    fclose(out);

   // printf("after  --> %d bit\n", charNum);

    out = fopen(outfile, "r");
    control = fgets(str, size, out) != NULL;
    while(!feof(out) || control) {
        strLength = strlen(str);
        if (control) {
            control = fgets(str, size, out) != NULL;
        }
    }
    fclose(out);
    
    FILE *S = fopen(outfile, "a");

	fputs(key, S);
	fclose(S);
}

void treeScan(treenodes *node,char *key) {
	
    if ( node != NULL ) {
        if (node->c >= 'A' && node->c <= 'Z') {
           // printf("\t%c - \tkodu: %s\n", node->c, node->code);
			    strncat(key, &node->c, 1);   
			    strcat(key,node->code);
        }
        treeScan(node->left,key);
        treeScan(node->right,key);
    }
}

typedef struct Data{
int offset;
int length;
char ch;

}Data;

void y(Data res,FILE* out){
	short int K=res.offset<<6;
    short int off_len=K+res.length;
    fwrite(&off_len, sizeof (off_len), 1, out);
    fwrite(&res.ch, 1, sizeof(res.ch),out);
 
}

float filesize(char*textTxt){
	
    float filesize;

FILE *file = fopen(textTxt, "rb");
fseek(file,0,SEEK_END);
filesize=ftell(file);
fclose(file);
    return filesize*0.000977;
}

Data encoder(char call[],int callLength,char fur[],int furLength){
    Data data;
if(callLength==0){
    data.offset=0;
    data.length=0;
    data.ch=fur[0];
    return data;
}

if(furLength==0){
    data.offset=-1;
    data.length=-1;
    data.ch=' ';
    return data;
}


int en_similiarity=0;
int en_distance=0;

int ArrCombLength=callLength+furLength; // concate string length
char *ArrComb = malloc(ArrCombLength); //  concate two arrays


//concate two strings

    memcpy(ArrComb,call,callLength); 
    memcpy(ArrComb+callLength,fur,furLength); 


    int call_index=callLength;
int p;
for(p=0; p<callLength; p++ ){

    int similiarity=0;
        // similiarity check 
        while(ArrComb[p+similiarity]==ArrComb[call_index+similiarity]){

            similiarity=similiarity+1;

            if( (call_index+similiarity)==ArrCombLength){
                similiarity=similiarity-1;
                break;
            }
            if((p+similiarity)>=call_index){
                break;
            }

        }
            // similiarity and distance comparison
            if(similiarity>en_similiarity){
                en_distance=p;
                en_similiarity=similiarity;
        }


}         
            data.offset=en_distance;
            data.length=en_similiarity;
            data.ch=ArrComb[call_index+en_similiarity];
            return data;
}

void lz77encoder(char *txt,char *outTxt){

        Data result;
        int x=16;
        int maxcall=1024;

        int maxlh = pow(2,(x-(log2(maxcall))));
        FILE *txtDoc; // text doc to read
        txtDoc=fopen(txt,"r");

        FILE *writtenfile; // uncompressed file
        writtenfile=fopen(outTxt,"wb");

          int Length=-1; // Text length
            FILE *f;
            char ch;
            f=fopen(txt, "r");
            while(feof(f)==NULL){
                    ch=getc(f);
                    Length++;
                }
            fclose(f);
        // The process of reading the text character by character and assigning it to the array

        char character;
        char* text = malloc(Length);
        character=fgetc(txtDoc);
        int id=0;
        while(character!=EOF){
            text[id]=character;
            character=fgetc(txtDoc);
            id++;
        }
        fclose(txtDoc);
        int callIterator=0;
        int furIterator=0;
        int control=0;
        int counter=0;
        while(furIterator<Length){

            int aI=callIterator; // keep these variables to give the starting place in the arrays
            int iL=furIterator; 

            int bA=furIterator-callIterator; // hold its size to create the Search Array
            int bI=maxlh; 


            if(bI>Length){
                if(control==0){
                   bI=Length;
                   control++;
                }
                else if(control>0){
                    bI=Length-bA;
                }
            }
            else if(bI<=Length){
                 control++;
                 if(control>0){
                 bI=Length-bA;
                }
            }
            // Filling the search string and forward string according to the available positions from the text string            char*callseq=malloc(bA);
        char*callseq=malloc(bA);
int k;
            for(k= 0; k<bA; k++, aI++){
                callseq[k]=text[aI];
            }

            char *furArr=malloc(bI);
int o;
            for(o= 0; o<bI; o++,iL++){
                furArr[o]=text[iL];
            }


            result=encoder(callseq,bA,furArr,bI); // Lz77 searching for


            y(result,writtenfile);

            furIterator=furIterator+result.length+1;
            callIterator=furIterator-maxcall;

            if(callIterator<0){
                callIterator=0;
            }

        }

            fclose(writtenfile);

}

void deflate(char *loc , char*inDeflate , char *deflateOut){
	
	huffman(loc,inDeflate);
	lz77encoder(inDeflate,deflateOut);
}

int main() {
		
		int testNum=3;
		
 char loc[3][30] ={
	  "input\\alphabet.txt",
	  "input\\sample.txt",
	  "input\\a.txt"
	  };

 char outputloc[3][30] ={
	  "output\\alphabet.LZFG",
      "output\\sample.LZFG",
      "output\\a.LZFG"
      };
  char inputDEFLATE[3][30] ={
	  "inputDeflate\\alphabet.txt",
      "inputDeflate\\sample.txt",
      "inputDeflate\\a.txt"
      };      
 char outputDEFLATE[3][30] ={
	  "outputDeflate\\alphabet.def",
      "outputDeflate\\sample.def",
      "outputDeflate\\a.def"
      };     
      
		int i;
		printf("\t============== THE COMPRESSION TEST ============== \n");
		
		
		printf("\t============== LZ77 TEST START============== \n\n");
		for(i=0; i<testNum; i++){
			printf("+\tTest %d test is starting -> File : %s , Size :%2.2f Kb\n",i+1,loc[i],filesize(loc[i]));
            lz77encoder(loc[i],outputloc[i]);
            float ratio = 100-filesize(outputloc[i])/filesize(loc[i])*100;
            printf("+\tCompression Rate : | %% %f |",ratio);
            ratio<0 ? printf("(Padding Process)\n") : printf("\n");
            printf("+\tTest %d Result -> File : %s , Size :%2.2f Kb\n+\n",i+1,outputloc[i],filesize(outputloc[i]));		
		}
        printf("\t============== LZSS TEST OVER ============== \n");
        
	    printf("\n\n");
	    
		printf("\t============== DEFLATE TEST START =============\n+\n"); 
		for(i=0; i<testNum; i++){
			printf("+\tTest %d test is starting -> File : %s , Size :%2.2f Kb\n",i+1,loc[i],filesize(loc[i]));
            deflate(loc[i],inputDEFLATE[i],outputDEFLATE[i]);
            float ratio = 100-filesize(outputDEFLATE[i])/filesize(loc[i])*100; 
            printf("+\tCompression Rate : | %% %f |",ratio);
            ratio<0 ? printf(" (Padding Process)\n") : printf("\n");
            printf("+\tTest %d Result -> File : %s , Size :%2.2f Kb\n+\n",i+1,outputDEFLATE[i],filesize(outputDEFLATE[i]));	
		}
		printf("\t============== DEFLATE TEST OVER ============== \n"); 	    
	
	getch();
    return 1;
}


