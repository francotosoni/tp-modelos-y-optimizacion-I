#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct prenda_t{
    size_t numero;
    size_t tiempo;
    size_t lavado;
    struct prenda_t * siguiente;
};

struct lavanderia_t{
    int ** matriz_incompatibilidades;
    struct prenda_t * prendas_a_lavar;
    struct prenda_t * prendas_lavado_actual;

    size_t cantidad_prendas;
    size_t cantidad_incompatibilidades;
};

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

struct prenda_t * crear_prenda(int numero, int tiempo){

    struct prenda_t * prenda;
    prenda = malloc(sizeof(struct prenda_t));
    if(prenda == NULL)
        return NULL;

    prenda -> numero = numero;
    prenda -> tiempo = tiempo;
    prenda -> lavado = 0;
    prenda -> siguiente = NULL;

}

void insertar_prenda(struct prenda_t ** prendas, struct prenda_t * prenda){
    if(prendas == NULL)
        return;

    if((*prendas) != NULL){
    
        while((*prendas) -> siguiente != NULL){
            prendas = &((*prendas) -> siguiente);
        }

        (*prendas) -> siguiente = prenda;

    }

    else{
        *prendas = prenda;
    }
    
    return;
}

struct lavanderia_t * crear_lavanderia(){

    char separador = ' ';
    char * linea;
    char * ptr;
    char ** linea_separada;


    struct lavanderia_t * lavanderia;
    int ** matriz_incompatibilidades;
    size_t cantidad_prendas;
    size_t cantidad_incompatibilidades;

    lavanderia = malloc(sizeof(struct lavanderia_t));
    if(lavanderia == NULL)
        return NULL;

    lavanderia -> prendas_a_lavar = NULL;
    lavanderia -> prendas_lavado_actual = NULL;

    FILE * archivo_entrada = fopen("datos.txt", "r");
    if(!archivo_entrada)
        return NULL;

    while((linea = leer_linea(archivo_entrada)) != NULL){

        if(linea[0] != 'c'){

            linea_separada = split(linea, separador);
            if(linea_separada == NULL){
                free(linea);
                fclose(archivo_entrada);
                return NULL;
            }

            if(linea[0] == 'p'){

                cantidad_prendas = strtoul(linea_separada[2], &ptr, 10);
                cantidad_incompatibilidades = strtoul(linea_separada[3], &ptr, 10);

                matriz_incompatibilidades = malloc(sizeof(int*) * cantidad_prendas);
                if(matriz_incompatibilidades == NULL){
                    free(linea);
                    destruir_vector(linea_separada);
                    fclose(archivo_entrada);
                    return NULL;
                }
                int ptr2 = 1;
                for(int i = 0; i < cantidad_prendas; i++){
                    matriz_incompatibilidades[i] = calloc((cantidad_prendas), sizeof(int));
                    if(matriz_incompatibilidades[i] == NULL){
                        ptr2 = 0;
                    }
                }
                if(ptr2 == 0){
                    free(linea);
                    destruir_vector(linea_separada);
                    fclose(archivo_entrada);
                    destruir_matriz(matriz_incompatibilidades, cantidad_prendas);
                    return NULL;
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

                struct prenda_t * prenda2 = crear_prenda(prenda, tiempo);
                insertar_prenda(&(lavanderia -> prendas_a_lavar), prenda2);

            }

        destruir_vector(linea_separada);

        }
        
        free(linea);
    }

    fclose(archivo_entrada);
    lavanderia -> matriz_incompatibilidades = matriz_incompatibilidades;
    lavanderia -> cantidad_prendas = cantidad_prendas;
    lavanderia -> cantidad_incompatibilidades = cantidad_incompatibilidades;

    return lavanderia;
}

void destruir_lavanderia(struct lavanderia_t * lavanderia){

    destruir_matriz(lavanderia -> matriz_incompatibilidades, lavanderia -> cantidad_prendas);
    struct prenda_t * aux = lavanderia -> prendas_lavado_actual;
    struct prenda_t * aux2;
    for(int i = 0; i < lavanderia -> cantidad_prendas; i++){
        aux2 = aux -> siguiente;
        free(aux);
        aux = aux2;
    }

    free(lavanderia);
}

struct prenda_t * prenda_mayor_tiempo(struct prenda_t * prendas){
    if(prendas == NULL)
        return NULL;

    struct prenda_t * prenda_mayor = prendas;

    while(prendas -> siguiente != NULL){
        if(prendas -> siguiente -> tiempo > prenda_mayor -> tiempo)
            prenda_mayor = prendas -> siguiente;

        prendas = prendas -> siguiente;
        
    }

    return prenda_mayor;

}

struct prenda_t * remover_prenda(struct prenda_t ** prendas, int numero){
    if(prendas == NULL)
        return NULL;

    if((*prendas) == NULL)
        return NULL;

    struct prenda_t * aux;

    if((*prendas) -> numero == numero){
        aux = (*prendas);
        (*prendas) = aux -> siguiente;
        aux -> siguiente = NULL;
        return(aux);
    }

    while((*prendas) -> siguiente != NULL){
        if((*prendas) -> siguiente -> numero == numero){
            aux = (*prendas) -> siguiente;
            (*prendas) -> siguiente = aux -> siguiente;
            aux -> siguiente = NULL;
            return aux;
        }
        prendas = &((*prendas) -> siguiente);
    }


    return NULL;
}

bool chequear_incompatibilidad(struct lavanderia_t * lavanderia, int cantidad_lavados, struct prenda_t * prenda){

    struct prenda_t * aux = lavanderia -> prendas_lavado_actual;

    while(aux != NULL){
        if(aux -> lavado == cantidad_lavados){
            if(lavanderia -> matriz_incompatibilidades[prenda->numero-1][aux->numero-1] == 1)
                return true;
        }

        aux = aux -> siguiente;
    }

    return false;
}

void imprimir_lavados(struct lavanderia_t * lavanderia){
    if(lavanderia == NULL)
        return;

    FILE * archivo_salida = fopen("resultado.txt", "w");
    if(!archivo_salida)
        return;

    struct prenda_t * prenda = lavanderia -> prendas_lavado_actual;

    while(prenda != NULL){
        fprintf(archivo_salida, "%lu %lu\n", prenda -> numero, prenda -> lavado);
        prenda = prenda -> siguiente;
    }

    fclose(archivo_salida);
    return;

}

int main(int argc, char *argv[]) {

    int cantidad_lavados = 1;
    struct lavanderia_t * lavanderia;

    lavanderia = crear_lavanderia();

    struct prenda_t * prenda;

    while(lavanderia -> prendas_a_lavar != NULL){

        prenda = prenda_mayor_tiempo(lavanderia -> prendas_a_lavar);
        remover_prenda(&(lavanderia -> prendas_a_lavar), prenda -> numero);
        insertar_prenda(&(lavanderia -> prendas_lavado_actual), prenda);
        prenda -> lavado = cantidad_lavados;

        while(lavanderia -> prendas_a_lavar != NULL){

            struct prenda_t * prenda2 = prenda_mayor_tiempo(lavanderia -> prendas_a_lavar);
            remover_prenda(&(lavanderia -> prendas_a_lavar), prenda2 -> numero);
            bool incompatible = chequear_incompatibilidad(lavanderia, cantidad_lavados, prenda2);
            if(incompatible == false)
                prenda2 -> lavado = cantidad_lavados;
            insertar_prenda(&(lavanderia -> prendas_lavado_actual), prenda2);

        }

        for(int i = 0; i < lavanderia -> cantidad_prendas; i++){
                struct prenda_t * prenda3 = remover_prenda(&(lavanderia -> prendas_lavado_actual), i+1); 
                if(prenda3 -> lavado == 0)        
                    insertar_prenda(&(lavanderia -> prendas_a_lavar), prenda3);
                else
                    insertar_prenda(&(lavanderia -> prendas_lavado_actual), prenda3);

        }

        cantidad_lavados++;

    }

    imprimir_lavados(lavanderia);

    destruir_lavanderia(lavanderia);

    return 0;

}

