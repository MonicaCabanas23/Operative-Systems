#include <iostream>
/* Archivo de Cabecera para las funciones de OpenMP */
#include <omp.h>

int main() {
    std::cout << "Bienvenido\n" << std::endl;
    std::cout << "Este es un programa demostrativo de algunas" << std::endl;
    std::cout << "funcionalidades básicas del uso de OpenMP." << std::endl;

    std::cout << "Primer Ejemplo [ENTER]";
    std::cin.get();

    {
        std::cout << "Ejecutado en el Hilo Principal" << std::endl;
        unsigned _id = 0;
        unsigned _threads = 0;
        std::cout << "Hola Estudiante - Hilo ID: " << _id << std::endl;
    }
    std::cout << "Continuar [ENTER]" << std::endl;
    std::cin.get();
    std::cout << "\nEjecutado en Paralelo" << std::endl;
#pragma omp parallel
    {
        unsigned _threads = omp_get_num_threads();
        unsigned _id = omp_get_thread_num(); /* Devuelve el Idendificador del Hilo */
        std::cout << "Hola Estudiante - Hilo ID: " << _id << " / " << _threads << std::endl;
    }

    std::cout << "Continuar [ENTER]" << std::endl;
    std::cin.get();
    std::cout << "\nEjecutado en Paralelo indicando que queremos 4 hilos" << std::endl;
    /* Indicamos a OpenMP que utilizaremos 4 hilos en los entornos paralelos*/
    omp_set_num_threads(4);
#pragma omp parallel
    {
        unsigned _threads = omp_get_num_threads();
        unsigned _id = omp_get_thread_num(); /* Devuelve el Idendificador del Hilo */
        std::cout << "Hola Estudiante - Hilo ID: " << _id << " / " << _threads << std::endl;
    }

    std::cout << "Continuar [ENTER]" << std::endl;
    std::cin.get();
    std::cout << "\nEjecutado en Paralelo indicando que queremos 2 hilos" << std::endl;
    /* Indicamos a OpenMP que utilizaremos 2 hilos en los entornos paralelos*/
#pragma omp parallel num_threads(2)
    {
        unsigned _threads = omp_get_num_threads();
        unsigned _id = omp_get_thread_num(); /* Devuelve el Idendificador del Hilo */
        std::cout << "Hola Estudiante - Hilo ID: " << _id << " / " << _threads << std::endl;
    }

    std::cout << "Continuar [ENTER]" << std::endl;
    std::cin.get();
    std::cout << "\nEjecutado en Paralelo indicando que queremos 10 hilos - Salida Ordenada" << std::endl;
    /* Indicamos a OpenMP que utilizaremos 2 hilos en los entornos paralelos*/
    #pragma omp parallel num_threads(10)
    {
        unsigned _threads = omp_get_num_threads();
        unsigned _id = omp_get_thread_num(); /* Devuelve el Idendificador del Hilo */
        #pragma omp critical
        std::cout << "Hola Estudiante - Hilo ID: " << _id << " / " << _threads << std::endl;
    }


    std::cout << "Continuar [ENTER]" << std::endl;
    std::cin.get();
    std::cout << "\nOrdenando la Salida" << std::endl;
    /* Sincronización por medio de Barrier */
    int a[5], i;
    unsigned contador = 0;
    #pragma omp parallel
    {
        #pragma omp for /* Paralelización del Ciclo for */
        for (i = 0; i < 5; i++) {
            a[i] = i * i;
            contador++;
        }

        // Imprimiendo los resultados Actuales.
        #pragma omp master /* Solo el Hilo Principal puede ejecutar la instrucción */
        {
            std::cout << "Resultados Intermedios:" << std::endl;
            for (i = 0; i < 5; i++)
                std::cout << "a[" << i << "] = " << a[i] << std::endl;
        }

        #pragma omp barrier /* Todos los hilos deben llegar a este punto antes de que puedan avanzar */

        #pragma omp for
        for (i = 0; i < 5; i++) {
            a[i] += i;
            contador++;
        }
    }

    std::cout << "Resultados Finales:" << std::endl;
    for (i = 0; i < 5; i++)
        std::cout << "a[" << i << "] = " << a[i] << std::endl;

    std::cout << "Operaciones realizadas = " << contador << std::endl;

    std::cout << "Continuar [ENTER]" << std::endl;
    std::cin.get();
    std::cout << "\nActualizaciones atómicas" << std::endl;
    /* Sincronización por medio de Barrier y atimics*/
    contador = 0;
    #pragma omp parallel
    {
        #pragma omp for /* Paralelización del Ciclo for */
        for (i = 0; i < 5; i++) {
            a[i] = i * i;
            #pragma omp atomic
            contador++;
        }

        // Imprimiendo los resultados Actuales.
        #pragma omp master /* Solo el Hilo Principal puede ejecutar la instrucción */
        {
            std::cout << "Resultados Intermedios:" << std::endl;
            for (i = 0; i < 5; i++)
                std::cout << "a[" << i << "] = " << a[i] << std::endl;
        }

        #pragma omp barrier /* Todos los hilos deben llegar a este punto antes de que puedan avanzar */

        #pragma omp for
        for (i = 0; i < 5; i++) {
            a[i] += i;
            #pragma omp atomic
            contador++;
        }
    }

    std::cout << "Resultados Finales:" << std::endl;
    for (i = 0; i < 5; i++)
        std::cout << "a[" << i << "] = " << a[i] << std::endl;

    std::cout << "Operaciones realizadas = " << contador << std::endl;

    return 0;
}
