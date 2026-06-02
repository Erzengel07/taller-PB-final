#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <sstream>

using namespace std;

struct Usuario {
    int idUsuario;
    string nombre;
    string correo;
    string contrasena;
    string direccion;
    string metodoPago;
    bool tieneBono50 = false;
};

struct Producto {
    int idProducto;
    string nombre;
    string descripcion;
    double precio;
    int stock;
    int cantidadVendidaTotal = 0;
};

struct Comentario {
    int idComentario;
    string producto;
    string usuario;
    string contenido;
    string fecha;
};

struct CarritoItem {
    int idCarritoInstance;
    int idUsuario;
    int idProducto;
    string nombreProducto;
    double precioUnidad;
};

struct OrdenDeCompra {
    int idOrden;
    Usuario usuario;
    vector<Producto> productos;
    double subtotal;
    double impuestos;
    double envio;
    double total;
};

string aMayusculas(string s) {
    string resultado = s;
    transform(resultado.begin(), resultado.end(), resultado.begin(), [](unsigned char c) { 
        return toupper(c); 
    });
    return resultado;
}

int main() {
    SistemaTienda tienda;
    tienda.inicializarDatos(); 

    string email, pass;
    cout << "--- SISTEMA DE TIENDA ---" << endl;
    cout << "Correo: "; cin >> email;
    cout << "Contrasena: "; cin >> pass;

    return 0;
}