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

    
    void cargarUsuarios() {
        ifstream archivo("usuarios.txt");
        if (!archivo.is_open()) {
            cout << "Error al abrir usuarios.txt. Se usará lista vacía." << endl;
            return;
        }
        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;
            stringstream ss(linea);
            string idStr, nombre, correo, contra, dir, pago, bonoStr;
            
            getline(ss, idStr, ',');
            getline(ss, nombre, ',');
            getline(ss, correo, ',');
            getline(ss, contra, ',');
            getline(ss, dir, ',');
            getline(ss, pago, ',');
            getline(ss, bonoStr, ',');

            Usuario u;
            u.idUsuario = stoi(idStr);
            u.nombre = nombre;
            u.correo = correo;
            u.contrasena = contra;
            u.direccion = dir;
            u.metodoPago = pago;
            u.tieneBono50 = (bonoStr == "1");
            
            usuarios.push_back(u);
        }
        archivo.close();
    }

    
    void cargarProductos() {
        ifstream archivo("productos.txt");
        if (!archivo.is_open()) {
            cout << "Error al abrir productos.txt. Se usará lista vacía." << endl;
            return;
        }
        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;
            stringstream ss(linea);
            string idStr, nombre, desc, precioStr, stockStr, vendidaStr;

            getline(ss, idStr, ',');
            getline(ss, nombre, ',');
            getline(ss, desc, ',');
            getline(ss, precioStr, ',');
            getline(ss, stockStr, ',');
            getline(ss, vendidaStr, ',');

            Producto p;
            p.idProducto = stoi(idStr);
            p.nombre = nombre;
            p.descripcion = desc;
            p.precio = stod(precioStr);
            p.stock = stoi(stockStr);
            p.cantidadVendidaTotal = stoi(vendidaStr);

            productos.push_back(p);
        }
        archivo.close();
    }

    
    void cargarComentarios() {
        ifstream archivo("comentarios.txt");
        if (!archivo.is_open()) {
            cout << "Error al abrir comentarios.txt. Se usará lista vacía." << endl;
            return;
        }
        string linea;
        while (getline(archivo, linea)) {
            if (linea.empty()) continue;
            stringstream ss(linea);
            string idStr, producto, usuario, contenido, fecha;

            getline(ss, idStr, ',');
            getline(ss, producto, ',');
            getline(ss, usuario, ',');
            getline(ss, contenido, ',');
            getline(ss, fecha, ',');

            Comentario c;
            c.idComentario = stoi(idStr);
            c.producto = producto;
            c.usuario = usuario;
            c.contenido = contenido;
            c.fecha = fecha;

            comentarios.push_back(c);
        }
        archivo.close();
    }

    
    void inicializarDatos() {
        cargarUsuarios();
        cargarProductos();
        cargarComentarios();
    }

    bool iniciarSesion(string email, string pass) {
        for (size_t i = 0; i < usuarios.size(); i++) {
            if (usuarios[i].correo == email && usuarios[i].contrasena == pass) {
                usuarioLogueado = &usuarios[i];
                return true;
            }
        }
        return false;
    }

    void listarProductosBajoStock() {
        cout << "\n--- Productos con stock bajo (menos de 15) ---" << endl;
        for (size_t i = 0; i < productos.size(); i++) {
            if (productos[i].stock < 15) {
                cout << "ID: " << productos[i].idProducto << " | " << productos[i].nombre << " | Stock: " << productos[i].stock << " | Precio: $" << productos[i].precio << endl;
            }
        }
    }

    void listarComentarios(string fechaFiltro) {
        cout << "\n--- Comentarios (Desde: " << fechaFiltro << ") ---" << endl;
        bool encontrado = false;
        for (size_t i = 0; i < comentarios.size(); i++) {
            if (comentarios[i].fecha >= fechaFiltro) {
                cout << "Fecha: " << comentarios[i].fecha << " | Producto: " << comentarios[i].producto 
                     << " | Usuario: " << comentarios[i].usuario << " | Contenido: " << comentarios[i].contenido << endl;
                encontrado = true;
            }
        }
        if (!encontrado) cout << "No hay comentarios desde esa fecha." << endl;
    }

    void listarUsuarios() {
        cout << "\n--- Lista de Usuarios ---" << endl;
        for (size_t i = 0; i < usuarios.size(); i++) {
            cout << "ID: " << usuarios[i].idUsuario << " | Nombre: " << aMayusculas(usuarios[i].nombre) 
                 << " | Correo: " << usuarios[i].correo << " | Direccion: " << usuarios[i].direccion 
                 << " | ¿Tiene Bono 50%?: " << (usuarios[i].tieneBono50 ? "SI" : "NO") << endl;
        }
    }

    void agregarAlCarrito(int idProd, int numCarrito) {
        if (numCarrito < 1 || numCarrito > 2) {
            cout << "Error: Un usuario no puede tener mas de 2 carritos (Elija 1 o 2)." << endl;
            return;
        }

        for (size_t i = 0; i < productos.size(); i++) {
            if (productos[i].idProducto == idProd) {
                if (productos[i].stock > 0) {
                    carrito.push_back({numCarrito, usuarioLogueado->idUsuario, productos[i].idProducto, productos[i].nombre, productos[i].precio});
                    productos[i].stock--;
                    cout << "Producto \"" << productos[i].nombre << "\" agregado al Carrito " << numCarrito << "! Stock restante: " << productos[i].stock << endl;
                } else {
                    cout << "Error: Sin stock disponible." << endl;
                }
                return;
            }
        }
        cout << "Producto no encontrado." << endl;
    }

    void listarCarrito() {
        cout << "\n--- Carritos Activos de " << usuarioLogueado->nombre << " ---" << endl;
        if (usuarioLogueado->tieneBono50) {
            cout << "¡TIENES UN BONO DEL 50% LISTO PARA SER APLICADO EN TU PROXIMO PAGO!" << endl;
        }
        
        for (int c = 1; c <= 2; c++) {
            cout << "\n> CARRITO Nro " << c << ":" << endl;
            bool vacio = true;
            double subtotalTemporal = 0;

            for (size_t i = 0; i < carrito.size(); i++) {
                if (carrito[i].idUsuario == usuarioLogueado->idUsuario && carrito[i].idCarritoInstance == c) {
                    cout << "  - Producto: " << carrito[i].nombreProducto << " ($" << carrito[i].precioUnidad << ")" << endl;
                    subtotalTemporal += carrito[i].precioUnidad;
                    vacio = false;
                }
            }
            if (vacio) {
                cout << "  (Vacio)" << endl;
            } else {
                cout << "  Subtotal estimado en este carrito: $" << subtotalTemporal << endl;
            }
        }
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
                        productos[j].cantidadVendidaTotal++; 
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

        bool aplicoDescuentoBono = false;
        if (usuarioLogueado->tieneBono50) {
            total = total * 0.50; 
            usuarioLogueado->tieneBono50 = false; 
            aplicoDescuentoBono = true;
        }

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
            if (aplicoDescuentoBono) {
                archivo << "¡NOTA!: Se aplico un bono de descuento del 50% en esta transaccion.\n";
            }
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

        if (total > valorLimiteBono) {
            usuarioLogueado->tieneBono50 = true;
            for (size_t u = 0; u < usuarios.size(); u++) {
                if (usuarios[u].idUsuario == usuarioLogueado->idUsuario) {
                    usuarios[u].tieneBono50 = true;
                }
            }
            cout << "Esta compra supero el limite de $" << valorLimiteBono 
                 << ". Se te ha otorgado un bono del 50% para tu siguiente carrito." << endl;
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

    void listarGananciasTotales() {
        cout << "\n--- Reporte Consolidado de Ganancias Totales ---" << endl;
        double sumSubtotal = 0;
        double sumImpuestos = 0;
        double sumTotal = 0;

        if (ordenesEmitidas.empty()) {
            cout << "Aun no se han registrado ventas/órdenes completadas en la plataforma." << endl;
            return;
        }

        for (const auto& orden : ordenesEmitidas) {
            sumSubtotal += orden.subtotal;
            sumImpuestos += orden.impuestos;
            sumTotal += orden.total;
        }

        cout << "Sumatoria Total Subtotales: $" << sumSubtotal << endl;
        cout << "Sumatoria Total Impuestos:  $" << sumImpuestos << endl;
        cout << "------------------------------------------" << endl;
        cout << "INGRESOS TOTALES NETOS:     $" << sumTotal << endl;
        cout << "Cantidad de ordenes fijadas: " << ordenesEmitidas.size() << endl;
    }
};

int main() {
    SistemaTienda tienda;
    tienda.inicializarDatos(); 

    string email, pass;
    cout << "--- SISTEMA DE TIENDA ---" << endl;
    cout << "Correo: "; cin >> email;
    cout << "Contrasena: "; cin >> pass;

    if (tienda.iniciarSesion(email, pass)) {
        int opcion;
        cout << "\n¡Bienvenido, " << tienda.usuarioLogueado->nombre << "!" << endl;
        do {
            cout << "\n--- MENU PRINCIPAL ---" << endl;
            cout << "1. Listar Productos (Stock < 15)\n"
                 << "2. Ver Comentarios (Por fecha)\n"
                 << "3. Listar Usuarios\n"
                 << "4. Adicionar producto al carrito\n"
                 << "5. Ver mis carritos (Muestra Carrito 1 y 2)\n"
                 << "6. Pagar un Carrito (Generar Orden .txt)\n"
                 << "7. Reporte: Productos mas vendidos\n"
                 << "8. Reporte: Ganancias de la Plataforma\n"
                 << "9. Salir\n"
                 << "Opcion: ";
            
            if (!(cin >> opcion)) {
                cout << "Por favor, ingrese un numero valido." << endl;
                cin.clear(); 
                cin.ignore(10000, '\n');
                opcion = 0;
                continue;
            }

            switch(opcion) {
                case 1: tienda.listarProductosBajoStock(); break;
                case 2: {
                    string fecha;
                    cout << "Ingrese fecha de inicio (AAAA-MM-DD): "; cin >> fecha;
                    tienda.listarComentarios(fecha);
                    break;
                }
                case 3: tienda.listarUsuarios(); break;
                case 4: {
                    int id, numC;
                    cout << "Ingrese el ID del producto: "; cin >> id;
                    cout << "¿A que carrito desea agregarlo? (1 o 2): "; cin >> numC;
                    tienda.agregarAlCarrito(id, numC);
                    break;
                }
                case 5: tienda.listarCarrito(); break;
                case 6: {
                    int numC;
                    double xLimite;
                    cout << "¿Cual de tus carritos deseas pagar? (1 o 2): "; cin >> numC;
                    cout << "Defina el valor minimo 'X' para regalar bono del 50% en esta compra: "; cin >> xLimite;
                    tienda.pagarYGenerarOrden(numC, xLimite);
                    break;
                }
                case 7: tienda.listarProductosMasVendidos(); break;
                case 8: tienda.listarGananciasTotales(); break;
                case 9: cout << "Cerrando sesion..." << endl; break;
                default: cout << "Opcion invalida." << endl;
            }
        } while (opcion != 9);
    } else {
        cout << "Error: Credenciales invalidas." << endl;
    }

    return 0;
}