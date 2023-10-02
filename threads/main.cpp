#include <iostream>
#include <functional>
#include <iostream>
#include <thread>
#include <future>

using namespace std;

void serie_geometrica(unsigned n, function<void(double)> callback)
{
    double valor = 0;
    unsigned long potencia = 1;
    cout << "Printing in another thread" << endl;
    for (unsigned i = 0; i < n; i++)
    {
        valor += 1.0 /  potencia;
        potencia *= 2;
        thread thr (callback, valor);
        thr.join();
    }
}

void serie_armonica(unsigned n, function<void(double)> callback)
{
    double valor = 0;
    cout << "Printing in another thread" << endl;
    for (unsigned i = 1; i <= n; i++)
    {
        valor += 1.0 / i;
        thread thr (callback, valor);
        thr.join();
    }
}

void serie_alternada(unsigned n, function<void(double)> callback)
{
    double valor = 0;
    float signo = 1;
    cout << "Printing in another thread" << endl;
    for (unsigned i = 1; i <= n; i++)
    {
        valor += signo / i;
        signo *= -1;
        thread thr (callback, valor);
        thr.join();
    }
}

int main() {
    cout << "Cálculo de Series Numéricas:" << endl;

    auto imprimir_serie = [](double x){
        std::cout << x << " ";
    };

    cout << "\n\nIniciando el cálculo de la serie numéricas" << endl;
    cout << "Series Numéricas:"    << endl;
    cout << "A - Serie Geométrica: " << endl;
    thread hilo1 (serie_geometrica, 25, imprimir_serie);
    hilo1.join();
    cout << endl << endl;

    cout << "B - Serie Armónica: "   << endl;
    thread hilo2 (serie_armonica, 25, imprimir_serie);
    hilo2.join();
    cout << endl << endl;

    cout << "C - Serie Alternada: "  << endl;
    thread hilo3 (serie_alternada, 25, imprimir_serie);
    hilo3.join();
    cout << endl << endl;

    return 0;
}
