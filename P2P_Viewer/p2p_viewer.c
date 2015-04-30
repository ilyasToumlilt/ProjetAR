#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SIZE 1024

void readingLine(char chaine[SIZE], FILE* file);
void display(char* in, char* out);
void footer(FILE* file);
void header(FILE* file);

int main(int argc, char *argv[]){
	if(argc < 2){
    	printf("Fichier in inconnu\n");
		exit(0);
	}
	if(argc < 3){
    	printf("Fichier out inconnu\n");
		exit(0);
	}
	
	display(argv[1], argv[2]);

    return 0;
}

void display(char* in, char* out){
    FILE* fileIn = NULL;
    FILE* fileOut = NULL;
	char chaine[SIZE] = "";

	fileOut = fopen(out, "w+");

	if ((fileIn = fopen(in, "r")) != NULL){
		header(fileOut);
        while (fgets(chaine, SIZE, fileIn) != NULL){
            readingLine(chaine, fileOut);
        }
 		footer(fileOut);
        fclose(fileIn);
    }else{
    	printf("Fichier inconnu\n");
    }

    fclose(fileOut);

}

void readingLine(char* chaine, FILE* file){
	const char* s = ";";
	char *token;
	char res[SIZE];
	int id, x, y; 

	token = strtok(chaine, s);
	if(strcmp(token, "Node") == 0){
		int x1, y1, x2, y2;

		id 	= atoi(strtok(NULL, s));
		x 	= atoi(strtok(NULL, s));
		y 	= atoi(strtok(NULL, s));
		x1 	= atoi(strtok(NULL, s));
		y1 	= atoi(strtok(NULL, s));
		x2 	= atoi(strtok(NULL, s));
		y2 	= atoi(strtok(NULL, s));

		sprintf(res, "<div id='node%d' class='node' data-x='%d' data-y='%d' title='x:%d y:%d'></div>\n<div id='space%d' class='space' data-x1='%d' data-y1='%d' data-x2='%d' data-y2='%d' data-color='white'>%d</div>\n"
			, id, x, y, x, y, id, x1, y1, x2, y2, id);
		fputs( res,file);

	}else if(strcmp(token, "Data") == 0){
		id 	= atoi(strtok(NULL, s));
		x 	= atoi(strtok(NULL, s));
		y 	= atoi(strtok(NULL, s));

		sprintf(res, "<div class='data' data-x='%d' data-y='%d' title='node:%d x:%d y:%d'></div>\n"
			, x, y, id, x, y);
		fputs(res ,file);
	}
}

void header(FILE* file){
	char* head = "<!DOCTYPE html>\n<html>\n<head>\n<meta charset='utf-8' />\n<link rel='stylesheet' href='style/style.css' />\n<script src='https://code.jquery.com/jquery-1.10.2.js'></script>\n</head>\n<body>\n<div id='grille'>\n";
	fputs(head ,file);
}
void footer(FILE* file){
	char* foot = "</div>\n<script type='text/javascript' src='geometry.js'></script>\n</body>\n</html>";
	fputs(foot ,file);
}