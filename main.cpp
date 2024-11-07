#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

#define NUM_ALUMNOS 10
#define MAX_NUM_PRUEBA 3

int alumnos = 0;
int pruebas = 0;
int numero_prueba = 0;
int contador = 0;

sem_t aluSem;
sem_t profSem;
sem_t mutex;

void entregarPrueba() {
    printf("Profesor entrega las pruebas.\n");
    sleep(1);
}

void recibirPrueba() {
    printf("Alumno recibe la prueba.\n");
    sleep(1);
}

void completaPrueba() {
    printf("Alumno completa la prueba.\n");
    sleep(1);
}

void calificaPrueba() {
    printf("Profesor califica la prueba %d.\n", numero_prueba);
    sleep(1);
}

void* funcion_profesor(void* arg){
    while(numero_prueba < MAX_NUM_PRUEBA){
        sem_wait(&profSem);
        sem_wait(&mutex);
        
        if (numero_prueba > 0){
            for(int i = 0; i < NUM_ALUMNOS; i++){
                calificaPrueba();
            }
        }
        
        if (numero_prueba < MAX_NUM_PRUEBA){
            pruebas = NUM_ALUMNOS;
            entregarPrueba();
        }
        
        sem_post(&aluSem);
        sem_post(&mutex);
    }
}

void* funcion_alumno(void* arg){
    while(numero_prueba < MAX_NUM_PRUEBA){
        sem_wait(&aluSem);
        sem_wait(&mutex);
        
        if (pruebas != 0){
            recibirPrueba();
            pruebas--;
            sem_post(&aluSem);
            sem_post(&mutex);
        } else if(contador < NUM_ALUMNOS){
            if(numero_prueba < MAX_NUM_PRUEBA)
                completaPrueba();
            contador++;
            sem_post(&aluSem);
            sem_post(&mutex);
        } else{
            contador = 0;
            numero_prueba++;
            sem_post(&profSem);
            sem_post(&mutex);
        }
    }
}

int main() {
    
    pthread_t profesor, alumnos[NUM_ALUMNOS];
    
    sem_init(&profSem, 0, 1);
    sem_init(&aluSem, 0, 0);
    sem_init(&mutex, 0, 1);
    
    pthread_create(&profesor, NULL, funcion_profesor, NULL);

    for (int i = 0; i < NUM_ALUMNOS; i++) {
        pthread_create(&alumnos[i], NULL, funcion_alumno, NULL);
    }
    
    pthread_join(profesor, NULL);
    for (int i = 0; i < NUM_ALUMNOS; i++) {
        pthread_join(alumnos[i], NULL);
    }
    
    sem_destroy(&profSem);
    sem_destroy(&aluSem);
    sem_destroy(&mutex);
    
    return 0;
}
