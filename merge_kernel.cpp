#include <elf.h>
#include <stdio.h>
#include <cstring>
#include <stdlib.h>
#include <assert.h>
#include <vector>
using namespace std;

int main(int argc, char ** argv){

    vector<pair<long long int , long long int >> kernels; 
    char filename[1000];
    char orig_name[1000];
    char cmd [1000];

    FILE * fmeta = fopen("fmeta_data.txt","r");

    fscanf(fmeta,"%s",orig_name);
    int num_kernels;
    fscanf(fmeta,"%d",&num_kernels);
    for ( int i =0; i < num_kernels; i++){
        long long int offset;
        long long int size ;
        fscanf(fmeta,"%d %d\n",&offset,&size);
        kernels.push_back(make_pair(offset,size));
    }
    fclose(fmeta);
    sprintf(filename,"%s.new",orig_name);
    sprintf(cmd,"cp %s %s",orig_name,filename);
    system(cmd);


    FILE * f = fopen(filename,"rb+");
    printf("number of kernels = %d\n",kernels.size());
    for (int i = 0; i < kernels.size() ; i++){
        printf("kernel %d : offset = %d , size = %d\n",i,kernels[i].first, kernels[i].second);
        char * buffer = (char *) malloc(kernels[i].second+1); 
        char tmp_filename[1000];
        {
            sprintf(tmp_filename,"tmp_%02d.hsaco\0",i);
            FILE * tmpfile = fopen(tmp_filename,"rb");
            fread(buffer,kernels[i].second,1,tmpfile);
            fclose(tmpfile);
        }
        fseek(f,kernels[i].first,SEEK_SET);
        fwrite(buffer,kernels[i].second,1,f);
        free(buffer);
    }
    fclose(fmeta);
    /*
    initscr();
    printw("Hello Wrold !!");
    refresh();
    getch();
    endwin();*/
    return 0;

}
