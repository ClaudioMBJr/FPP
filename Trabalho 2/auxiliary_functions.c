

int jobs(int* list, int objective, int barber_quantity){
    int j = 0;
    for (int i = 0; i < barber_quantity; i++){
        if (list[i] >= objective){
            j += 1;
        }
    }
    if (j == barber_quantity){
        return 0;
    }
    return 1;
}
