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

class SistemaTienda {
public:
    vector<Usuario> usuarios;
    vector<Producto> productos;
    vector<Comentario> comentarios;
    vector<CarritoItem> carrito;
    vector<OrdenDeCompra> ordenesEmitidas;
    Usuario* usuarioLogueado;
    int contadorOrdenes = 1;

    SistemaTienda() {
        usuarioLogueado = nullptr;
    }

    void pagarYGenerarOrden(int numCarrito, double valorLimiteBono) {
        if (numCarrito < 1 || numCarrito > 2) {
            cout << "Numero de carrito invalido." << endl;
            return;
        }

        vector<Producto> itemsAComprar;
        double subtotal = 0;

        for (size_t i = 0; i < carrito.size(); i++) {
            if (carrito[i].idUsuario == usuarioLogueado->idUsuario && carrito[i].idCarritoInstance == numCarrito) {
                for (size_t j = 0; j < productos.size(); j++) {
                    if (productos[j].idProducto == carrito[i].idProducto) {
                        itemsAComprar.push_back(productos[j]);
                        productos[j].cantidadVendidaTotal++; // Suma acumulador para el reporte de más vendidos
                    }
                }
                subtotal += carrito[i].precioUnidad;
            }
        }

        if (itemsAComprar.empty()) {
            cout << "El carrito " << numCarrito << " no tiene productos para pagar." << endl;
            return;
        }

        double impuestos = subtotal * 0.19; 
        double envio = 1500.0;
        double total = subtotal + impuestos + envio;

        

        
        OrdenDeCompra nuevaOrden;
        nuevaOrden.idOrden = contadorOrdenes++;
        nuevaOrden.usuario = *usuarioLogueado;
        nuevaOrden.productos = itemsAComprar;
        nuevaOrden.subtotal = subtotal;
        nuevaOrden.impuestos = impuestos;
        nuevaOrden.envio = envio;
        nuevaOrden.total = total;

        ordenesEmitidas.push_back(nuevaOrden);

        
        string nombreArchivo = "Orden_Compra_" + to_string(nuevaOrden.idOrden) + ".txt";
        ofstream archivo(nombreArchivo);

        if (archivo.is_open()) {
            archivo << "            ORDEN DE COMPRA #" << nuevaOrden.idOrden << "\n";
            archivo << "=========================================\n";
            archivo << "CLIENTE: " << nuevaOrden.usuario.nombre << "\n";
            archivo << "CORREO: " << nuevaOrden.usuario.correo << "\n";
            archivo << "DIRECCION: " << nuevaOrden.usuario.direccion << "\n";
            
            
            archivo << "-----------------------------------------\n";
            archivo << "PRODUCTOS ADQUIRIDOS:\n";
            for (const auto& prod : nuevaOrden.productos) {
                archivo << " - ID: " << prod.idProducto << " | " << prod.nombre << " | $" << prod.precio << "\n";
            }
            archivo << "-----------------------------------------\n";
            archivo << "SUBTOTAL:  $" << nuevaOrden.subtotal << "\n";
            archivo << "IMPUESTOS: $" << nuevaOrden.impuestos << "\n";
            archivo << "ENVIO:     $" << nuevaOrden.envio << "\n";
            archivo << "TOTAL:     $" << nuevaOrden.total << "\n";
            archivo << "=========================================\n";
            archivo.close();
            cout << "\n¡Pago Procesado! Archivo \"" << nombreArchivo << "\" generado con exito." << endl;
        } else {
            cout << "Error critico al intentar escribir el archivo de texto." << endl;
        }

    

        
        auto it = carrito.begin();
        while (it != carrito.end()) {
            if (it->idUsuario == usuarioLogueado->idUsuario && it->idCarritoInstance == numCarrito) {
                it = carrito.erase(it);
            } else {
                ++it;
            }
        }
    }

    void listarProductosMasVendidos() {
        cout << "\n--- Productos Mas Vendidos y sus Ordenes de Compra ---" << endl;
        vector<Producto> ordenados = productos;
        sort(ordenados.begin(), ordenados.end(), [](const Producto& a, const Producto& b) {
            return a.cantidadVendidaTotal > b.cantidadVendidaTotal;
        });

        
        for (size_t i = 0; i < ordenados.size(); i++) {
            if (ordenados[i].cantidadVendidaTotal > 0) {
                cout << "\nID: " << ordenados[i].idProducto 
                     << " | Nombre: " << ordenados[i].nombre 
                     << " | Unidades vendidas: " << ordenados[i].cantidadVendidaTotal << endl;
                cout << "   Vendido en las Siguientes Ordenes: ";
                
                bool ordenEncontrada = false;
                for (const auto& orden : ordenesEmitidas) {
                    for (const auto& prod : orden.productos) {
                        if (prod.idProducto == ordenados[i].idProducto) {
                            cout << "[Orden #" << orden.idOrden << "] ";
                            ordenEncontrada = true;
                            break;
                        }
                    }
                }
                if (!ordenEncontrada) cout << "Ninguna (Venta directa inicial)";
                cout << endl;
            }
        }
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