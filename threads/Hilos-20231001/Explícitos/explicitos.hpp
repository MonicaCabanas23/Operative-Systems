//
// Created by adrian on 09/09/20.
//

// #include "explicitos.h"
#include <iostream>
#include <thread>
#include <future>
#include <time.h>
#include <vector>
#include <math.h>
#include <unistd.h>
#include <chrono>

using namespace std;

void my_function(int n, int &m_fb)
{
    if (n == 1)  m_fb = 0;
    else if(n == 2) m_fb = 1;
    else {
        int prev = 0, t = 0;
        n -= 2;
        m_fb = 1;
        for (; n > 0; n--) {
            t = m_fb;
            m_fb += prev;
            prev = t;
        }
    }
}

void test_threads1(){
    int fb;
    int ctd = 10;

    // thread th (fcn, args..., ref(v));

    thread hilo (my_function , ctd, ref(fb));

    /* Otras instrucciones */
    cout << "Esperando para imprimir los resultados ..." << endl;
    hilo.join();
    cout << "Fibonacci:" << fb << endl;
}

void test_threads1A(){
    // Otra forma de declarar las promesas y futuros que no devuelven ningún valor (void)
    promise<void> prom = promise<void>();
    future<void>  fut = prom.get_future();

    thread thr(
            [](future<void> f){
                int i =0 ;
                while (i < 2){
                    cout << "Durmiendo en el Hilo" << endl;
                    sleep(1);
                    // Espera por un milisegundo, por qué esperamos por un milisegundo?
                    if (f.wait_for(std::chrono::milliseconds(1)) == future_status::ready) {
                        cout << "\nFuture status is ready" << endl;
                        break;
                    }
                    i++;
                }
                cout << "Hilo listo para terminar" << endl;
            },
            move(fut)
    );
    sleep(7);
    cout << "Voy a terminar el hilo hijo" << endl;
    prom.set_value();
    thr.join();
}

void test_threads2(){
    // Declaramos una promise la cual devolverá un entero
    auto prom_fb = promise<int>();
    auto fut_fb = prom_fb.get_future();
    int ctd = 10;

    // Es una creación de un hilo por medio de una función lambda
    // Esta función lambda tiene acceso a prom_fb por referencia, por lo cual podemos cambiar su 
    // valor directamente dentro de la función lambda ya que se encuentra dentro de su scope.
    thread hilo(
            [&prom_fb](int n){
                int fb;
                if (n == 1)  fb = 0;
                else if(n == 2) fb = 1;
                else {
                    int prev = 0, t = 0;
                    n -= 2;
                    fb = 1;
                    for (; n > 0; n--) {
                        t = fb;
                        fb += prev;
                        prev = t;
                    }
                }
                // Al terminar todos los cálculos necesario seteamos el valor que devolverá la promesa
                prom_fb.set_value(fb);
            },
            // copia del valor que toma la función como parámetro (n)
            ctd
    );
    // .detach() permite que la ejecución del hilo principal siga mientras el otro hilo se ejecuta.
    // Es lo contrario a hilo.join()
    hilo.detach();
    cout << "\nEsperando por la promesa para completarse";
    fut_fb.wait();
    auto fb = fut_fb.get();
    cout << "\nFibonacci:" << fb << endl;
}

void test_async()
{
    auto fcn = [](int x){
        int sum = 0;
        for (int i = 0; i <= x; i++) sum += i;
        return sum;
    };

    /*
    * std::launch::async - Ejecuta la función de forma asíncrona en otro hilo
    * std::launch::deferred - Ejecuta la función en el mismo hilo pero se retrasa hasta que
    *                          se solicita su valor.
    */

    future<int> result = async(launch::async, fcn, 9);
    result.wait();
    cout << "Resultado: " <<result.get() << endl;
}