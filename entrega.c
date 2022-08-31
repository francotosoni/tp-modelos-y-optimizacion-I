#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char * leer_linea(FILE * archivo){

    if(archivo == NULL)
        return NULL;

    size_t largo = 1024;
    size_t char_leidos = 0;

    char * line = malloc(largo * sizeof(char));
    if(line == NULL)
        return NULL;

    while(fgets(line + char_leidos, (int)(largo - char_leidos), archivo)){

        size_t largo_leido = strlen(line + char_leidos);

        if(largo_leido > 0 && *(line + char_leidos + largo_leido - 1) == '\n'){

            *(line + char_leidos + largo_leido - 1) = '\0';

            return line;

        }

        else{

            char * aux = realloc(line, (largo * 2) * sizeof(char));

            if(aux == NULL){
                free(line);
                return NULL;
            }

            line = aux;
            largo *= 2;
        }

        char_leidos += largo_leido;

    }

    if(char_leidos == 0){
        free(line);
        return NULL;
    }

    return line;

}

size_t contar_separadores(const char* string, char separador){

    if(string == NULL)
        return 0;

    size_t n, separadores = 0;

    for(n = 0; n < strlen(string); n++){

        if(string[n] == separador)
            separadores++;

    }

    return separadores;
}

size_t contar_posicion_proximo_separador(const char* string, char separador){

    if(string == NULL)
        return 0;

    size_t n = 0;

    while(string[n] != separador && string[n] != '\0')
        n++;

    return n;
}

void destruir_vector(char ** vector){

    if(vector == NULL)
        return;

    char ** aux = vector;

    while(*vector){

        free(*vector);
        vector++;

    }

    free(aux);

    return;

}

void destruir_matriz(int ** vector, size_t cantidad){

    if(vector == NULL)
        return;

    int ** aux = vector;

    for(int i = 0; i < cantidad; i++){

        free(*vector);
        vector++;
    }

    free(aux);

    return;

}

void copiar_string(char * string, const char* string2, size_t largo){

    if(string == NULL || string2 == NULL)
        return;

    size_t n;

    for(n = 0; n < largo; n++){

        string[n] = string2[n];
    }

    string[n] = '\0';

    return;

}



char** split(const char* string, char separador){

    if(string == NULL)
        return NULL;

    size_t cantidad_substrings = contar_separadores(string, separador) + 1;

    char ** vector_substrings = calloc((cantidad_substrings+1), sizeof(void*));
    if(vector_substrings == NULL)
        return NULL;

    size_t n;

    for(n = 0; n < cantidad_substrings; n++){

        size_t largo_substring = contar_posicion_proximo_separador(string, separador);

        vector_substrings[n] = malloc(sizeof(char) * (largo_substring + 1));
        if(vector_substrings[n] == NULL){
            destruir_vector(vector_substrings);
            return NULL;
        }

        copiar_string(vector_substrings[n], string, largo_substring);

        string += largo_substring + 1;

    }

    return vector_substrings;
}

int main(int argc, char *argv[]) {

    char separador = ' ';
    char * linea;
    char * ptr;
    char ** linea_separada;

    size_t cantidad_prendas;
    size_t cantidad_incompatibilidades;

    int ** matriz_incompatibilidades;
    int * vector_lavados;
    int cantidad_lavados = 0;

    FILE * archivo_entrada = fopen("datos.txt", "r");
    if(!archivo_entrada)
        return 1;

    while((linea = leer_linea(archivo_entrada)) != NULL){

        if(linea[0] != 'c'){

            linea_separada = split(linea, separador);
            if(linea_separada == NULL){
                free(linea);
                fclose(archivo_entrada);
                return 1;
            }

            if(linea[0] == 'p'){

                cantidad_prendas = strtoul(linea_separada[2], &ptr, 10);
                cantidad_incompatibilidades = strtoul(linea_separada[3], &ptr, 10);

                matriz_incompatibilidades = malloc(sizeof(int*) * cantidad_prendas);
                if(matriz_incompatibilidades == NULL){
                    free(linea);
                    destruir_vector(linea_separada);
                    fclose(archivo_entrada);
                    return 1;
                }
                int * ptr2;
                for(int i = 0; i < cantidad_prendas; i++){
                    matriz_incompatibilidades[i] = calloc((cantidad_prendas), sizeof(int));
                    if(ptr2 != NULL){
                        ptr2 = matriz_incompatibilidades[i];
                    }
                }
                if(ptr2 == NULL){
                    free(linea);
                    destruir_vector(linea_separada);
                    fclose(archivo_entrada);
                    destruir_matriz(matriz_incompatibilidades, cantidad_prendas);
                    return 1;
                }

                vector_lavados = malloc(sizeof(int) * cantidad_prendas);
                if(vector_lavados == NULL){
                    free(linea);
                    destruir_vector(linea_separada);
                    fclose(archivo_entrada);
                    destruir_matriz(matriz_incompatibilidades, cantidad_prendas);
                    return 1;
                }

            }

            if(linea[0] == 'e'){

                int prenda = strtol(linea_separada[1], &ptr, 10);
                int incompatibilidad = strtol(linea_separada[2], &ptr, 10);

                matriz_incompatibilidades[prenda-1][incompatibilidad-1] = 1;

            }

            if(linea[0] == 'n'){

                int prenda = strtol(linea_separada[1], &ptr, 10);
                int tiempo = strtol(linea_separada[2], &ptr, 10);

                matriz_incompatibilidades[prenda-1][prenda-1] = tiempo;

            }

        destruir_vector(linea_separada);

        }
        
        free(linea);
    }

    int prenda_mayor = 0;
    int tiempo_mayor = -1;

    while(tiempo_mayor != 0){
        for(int i = 0; i < cantidad_prendas; i++){
            if(matriz_incompatibilidades[i][i] > tiempo_mayor){
                prenda_mayor=i+1;
                tiempo_mayor =  matriz_incompatibilidades[i][i];
            }
        }

        for(int i = 0; i < cantidad_prendas; i++){
            if(i != (prenda_mayor-1)){
                if(matriz_incompatibilidades[i][prenda_mayor-1] == 0 && matriz_incompatibilidades[i][i] != 0){
                    vector_lavados[i] = (cantidad_lavados+1);
                    matriz_incompatibilidades[i][i] = 0;
                }
            }

            else if(matriz_incompatibilidades[i][i] != 0){
                vector_lavados[i] = (cantidad_lavados+1);
                matriz_incompatibilidades[i][i] = 0;
                tiempo_mayor = -1;
            }
        }

        cantidad_lavados++;
    }

    for(int i = 0; i < 20; i++){
        for(int j = 0; j < 20; j++){
            printf("%d  ", matriz_incompatibilidades[i][j]);
        }
        printf("\n");
    }

    for(int i = 0; i < 20; i++){
        printf("%d %d\n", (i+1), vector_lavados[i]);
    }


    destruir_matriz(matriz_incompatibilidades, cantidad_prendas);
    fclose(archivo_entrada);
    return 0;

}
