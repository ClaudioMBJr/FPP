

//Essa função é responsável por verificar se o objetivo mínimo das threads já foi alcançado
// Ela utiliza uma lista com a quantidade de trabalho de cada thread para verificação
//A variável objetive é o objetivo minimo de cada thread
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
