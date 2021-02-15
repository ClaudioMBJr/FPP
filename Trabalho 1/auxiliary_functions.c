

int jobs(int* list, int objective){
    int j = 0;
    for (int i = 0; i < 6; i++){
        if (list[i] >= objective){
            j += 1;
        }
    }
    if (j == 6){
        return 0;
    }
    return 1;
}
