#include "Graph.h"
#include "hmap.h"


int rfile(char* fname, HMAP* map,int buffer) {
	FILE * pFile;
	pFile = fopen(fname, "r");
	char mystring[300];
	char *mytoken;
	int index=0;
	while (fgets(mystring, 300, pFile)) {
		
		mytoken = strtok(mystring, " :\n");
		if (mytoken == NULL) { continue; }
		if (hmap_contains(map, mytoken)) { return 0; }
		hmap_set(map, mytoken, index);
		index++;
	}

	fclose(pFile);
	return 1;
}


int main(int argc, char **argv) {

	FILE * pFile=NULL;
	
	//pFile = fopen("fakefile.txt", "r");
	int i = 0;
	 pFile = fopen(argv[1], "r");
	
	if (pFile == NULL) perror("Error opening file");
	int charcount = 0;
	int ch;
	int TFcutoff = -1;
	while (1) {
		ch = fgetc(pFile);
		if (ch == EOF)
			break;
		++charcount;
	}rewind(pFile);
	charcount =charcount;
	
	char* mystring=(char*)malloc(charcount*(sizeof(char)));//------------------------------------------------------------
	char** inputfiles;
	char** uninputfiles;
	int Num_Verticies = 0;
	
     i = 0;
	int colflag = 0;
	while (fgets(mystring, charcount, pFile) != NULL) {
		puts(mystring);
		//printf("TFcutoff is %d\n", TFcutoff);
		
		if (strchr(mystring, ':') && colflag == 0) { TFcutoff = Num_Verticies; colflag = 1; }
	
		Num_Verticies++;
	}
	
	printf("TFcutoff is %d\n", TFcutoff);
	int* lenarr = (int*)malloc(Num_Verticies*(sizeof(int)));
	int* tsArr= (int*)malloc(Num_Verticies*(sizeof(int)));
	inputfiles = (char**)malloc(Num_Verticies*(sizeof(char*)));
	uninputfiles = (char**)malloc(Num_Verticies*(sizeof(char*)));
	//--------------------------------------------------------------------------------------------------------------------------
	printf(" Num_Verticies is %d \n",Num_Verticies);
	int Num_targfiles = (Num_Verticies - TFcutoff);
	rewind(pFile);
	i = 0;
	int j = 0;
	for ( j = 0; j < Num_Verticies; j++) {tsArr[j] = 0;}
	j = 0;
	while (fgets(mystring, charcount, pFile) != NULL) {
		//puts(mystring);
		int x = strlen(mystring);
		//printf(" current len %d ", x);
		lenarr[i] = x;
		i++;
	}

	rewind(pFile);
	i = 0;
	for ( j = 0; j < Num_Verticies; j++) {
		inputfiles[j] = (char*)malloc((lenarr[j]+1) * sizeof(char));
		uninputfiles[j] = (char*)malloc((lenarr[j]+1) * sizeof(char));
	}
	//printf("\n------------------------------------------------------------------------------------------\n");
	while (fgets(mystring, charcount, pFile) != NULL) {
		strcpy(uninputfiles[i], mystring);
		strcpy(inputfiles[i], mystring);
		if (Num_Verticies-1 == i) { break; }
		//inputfiles[i][lenarr[i]] = '\0';
		//uninputfiles[i][lenarr[i]] = '\0';
		char *pos;
		if ((pos = strchr(inputfiles[i], '\n')) != NULL)
			*pos = '\0';
		if ((pos = strchr(uninputfiles[i], '\n')) != NULL)
			*pos = '\0';
		i++;
		
	}
	i = 0;
	fclose(pFile);
	//printf("\nprinting string %s\n", mystring);
	j = 0;
	
	
	for ( i = TFcutoff; i < Num_Verticies; i++) {
		int Opos = strchr(inputfiles[i],' ') - inputfiles[i];
		lenarr[i] = Opos;
		char* tmp= strtok(inputfiles[i]," ");
		
		//tmp[Opos] = '\0';
		//printf("----------------------------------------tmp is %s\n", tmp);
		
	}
	j = 0;
	for ( i = TFcutoff; i < Num_Verticies; i++) {
		for (j = 0; j < lenarr[i]; j++) {
			uninputfiles[i][j] = '*';
		}
	}
	i = 0;
	j = 0;
	for (j = 0; j < Num_Verticies; j++) {
		printf(" %d uninputfiles is %s\n", j, uninputfiles[j]);
		
	}
	
	for (j = 0; j < Num_Verticies; j++) {
		printf(" %d inputfiles is %s\n", j, inputfiles[j]);

	}
	j = 0;
//--------Done parsing----------------------------------------------------------------------------------------------------------------------
	
	HMAP * fmap = hmap_create(0, 1.0);
	GRAPH* Ajaylist = graph_build(Num_Verticies);
	
	rfile(argv[1], fmap, charcount);
	/*
	for ( j = 0; j < Num_Verticies; j++) {
		if (hmap_contains(fmap, inputfiles[j])) { printf("Error same file names entered\n"); break; }
		printf(" b4 set hmap %d inputfiles is %s\n", j, inputfiles[j]);
		//hmap_set(fmap, inputfiles[j], j); 
		if (j >= TFcutoff) { hmap_set(fmap, inputfiles[j], j); printf(" using hmap file %s : %d \n", inputfiles[j], hmap_get(fmap, inputfiles[j]));
		}
		else{ hmap_set(fmap, uninputfiles[j], j); printf("uninputfiles using hmap file %s : %d \n", uninputfiles[j], hmap_get(fmap, uninputfiles[j]));
		}
		if (!hmap_contains(fmap, inputfiles[j])) {break; }
	}
	*/
	
	
	hmap_display(fmap);
//-----------------------------------------------------------------------------------------------------------------------------------------
	for ( i = TFcutoff; i < Num_Verticies; i++) {
		//if (hmap_contains(fmap, inputfiles[j])) { printf("Error same file names entered\n"); break; }
		for ( j = 0; j < (Num_Verticies); j++) {

			if (strstr(uninputfiles[i], inputfiles[j]) && strcmp(uninputfiles[i], inputfiles[j]) != 0)
			{
				graph_add_edge(Ajaylist, i, j); printf("targ array adding edge from %s ----> %s\n", uninputfiles[i], inputfiles[j]);
			}
		}
		j = 0;
	}
	i = 0;
	for ( i = TFcutoff; i < Num_Verticies; i++) {
		int t = 0;
		int* tp = &t;
		//printf("testing %s for loops\n", inputfiles[i]);
		dfs(Ajaylist, i, tp);
		if (t) {
			free(lenarr);
			free(inputfiles);
			free(mystring);
			free(tsArr);
			graph_free(Ajaylist);
			hmap_free(fmap, 0);
			printf("***************EXITING PROGRAM***************\n"); exit(0); 
				}
	}
	timestamparr = tsArr;
	files = inputfiles;
	//touch(timestamparr, 1);
	//touch(timestamparr, 2);
	//dfs_make(Ajaylist, 5);
	//dfs_make(Ajaylist, 7);
	char input[300];
	//char inputh[300];
	while (strcmp(input, "quit") != 0) {
		gets(input);
		//scanf("%s", input);
		char* tmp=NULL;
		//strtok(input, " ");
		tmp=strchr(input, ' ')+1;
		//strtok(input, " ");
		//printf("pch1 is %s pch2 is %s \n", pch1, pch2);
		if (strcmp(input, "quit") == 0) { break; }
	    else if(strstr(input, "touch")){
			if (hmap_contains(fmap, tmp) == 0) { printf("file does not exist\n"); continue; }
			int index = hmap_get(fmap, tmp);
			if (index >= TFcutoff) { printf("file is not a basic file\n"); continue; }
			touch( timestamparr,index);
			printf("file %s has been modified\n", tmp);
		}
		else if(strcmp(input,"timestamps")==0){
			for ( j = 0; j < Num_Verticies; j++) {
				printf("timestamp %d for %s \n", timestamparr[j], inputfiles[j]);
			}
		}
		else if (strstr(input, "timestamp")){
			if (hmap_contains(fmap, tmp) == 0) { printf("file does not exist\n"); continue; }
			int index = hmap_get(fmap, tmp);
			printf("timestamp %d for %s \n", timestamparr[index], inputfiles[index]);
		}
		else if (strstr(input, "time")) { printf("clock is currently %d\n", clock); }
		else if(strstr(input, "make")){
			if (hmap_contains(fmap, tmp) == 0) { printf("file does not exist\n"); continue; }
			int index = hmap_get(fmap, tmp);
			if (index <= TFcutoff) { printf("file is not a target file\n"); continue; }
			dfs_make(Ajaylist, index);
		}
		else { printf("not a valid input please retry \n"); }

	}
//---------------FREEING 2D file array and len array---------------------------------------------------------------------------------------
	free(lenarr);
	free(inputfiles);
	free(mystring);
	free(tsArr);
	graph_free(Ajaylist);
	hmap_free(fmap, 0);

	return 0;
}
