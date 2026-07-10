#include <stdint.h>


void rotate(int* array, int size, int shift){
    
    int temp;

    for(int i = 0; i < shift; ++i){
        for(int j = 0; j < size-1; ++j){
            temp = array[j+1];
            array[j+1] = array[j];
            array[j] = temp;
        }
    }
}

void euler_rhythm(int* array, int total_beats, int hits){

    int sum = 0;

    for(int i = 0; i < total_beats; ++i){
        sum+=hits;
        if(sum >= total_beats){
            array[i] = 1;
            sum-=total_beats;
        } else {
            array[i] = 0;
        }
    }

    rotate(array, total_beats, total_beats-1);
}

void print_rhythm(int* array, int size){

}

int euler_main()
{
    int rhythm[32] = {0};
    int total_beats = 8;
    int hits = 3;
    int shift;

    euler_rhythm(rhythm, total_beats,  hits);
    
    print_rhythm(rhythm, total_beats);

    // rotate(rhythm, total_beats, shift);


    return 0;
}
