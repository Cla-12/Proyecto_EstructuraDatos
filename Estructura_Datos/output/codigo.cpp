#include <iostream>     // Librería estándar para entrada (cin) y salida (cout) de datos por consola
#include <fstream>      // Librería para manejo de archivos (guardar y leer datos en disco)
#include <string>       // Librería para utilizar el tipo de dato cadena de texto (string)
#include <cstdlib>      // Librería estándar de C para funciones del sistema como system("cls") y system("pause")

using namespace std;    // Permite usar elementos del espacio de nombres estándar sin escribir "std::"

// ===================================================
// DEFINICIÓN DE COLORES ANSI PARA LA CONSOLA
// ===================================================
#define RESET    "\033[0m"
#define ROJO     "\033[1;31m"
#define VERDE    "\033[1;32m"
#define AMARILLO "\033[1;33m"
#define AZUL     "\033[1;34m"
#define MAGENTA  "\033[1;35m"
#define CIAN     "\033[1;36m"
#define BLANCO   "\033[1;37m"

// ===================================================
// ESTRUCTURA DEL PEDIDO
// ===================================================
struct Nodo {               // Define la estructura de cada nodo (elemento) de las estructuras de datos
    int codigo;             // Almacena el identificador numérico único del pedido
    string cliente, plato;  // Almacenan el nombre del cliente y el nombre del plato pedido
    float precio;           // Almacena el costo monetario del pedido
    Nodo* sig;              // Puntero autorreferenciado que apunta al siguiente nodo en memoria
};                          // Fin de la definición de la estructura Nodo

// Punteros globales para administrar las 3 estructuras de datos dinámicas
Nodo* cabezaLista = NULL;   // Puntero al primer nodo de la Lista Enlazada (Registro general), inicia vacío (NULL)
Nodo* frenteCola = NULL;    // Puntero al primer nodo de la Cola de cocina (Siguiente a atender), inicia vacío
Nodo* finalCola = NULL;     // Puntero al último nodo de la Cola de cocina (Para insertar rápido al final), inicia vacío
Nodo* topePila = NULL;      // Puntero al nodo superior de la Pila de entregas (Último entregado), inicia vacío

//---------------- FUNCIONES AUXILIARES Y GRÁFICOS ----------------//

// Dibuja gráficamente los nodos conectados en línea (usado para Lista Enlazada y Cola)
void graficarEstructuraLineal(Nodo* inicio, string etiquetaInicio, string color) {
    cout << "\n" << BLANCO << "  [REPRESENTACION GRAFICA DE NODOS EN MEMORIA]" << RESET << "\n  ";
    cout << AMARILLO << etiquetaInicio << " -> " << RESET;

    Nodo* actual = inicio;
    while (actual != NULL) {
        cout << color << "[" << actual->codigo << " | " << actual->plato << "]" << RESET
             << BLANCO << " --> " << RESET;
        actual = actual->sig;
    }
    cout << ROJO << "NULL" << RESET << "\n\n";
}

// Dibuja gráficamente los nodos apilados verticalmente (usado para la Pila LIFO)
void graficarPilaVertical(Nodo* tope) {
    cout << "\n" << BLANCO << "  [REPRESENTACION GRAFICA DE LA PILA (LIFO)]" << RESET << "\n";
    Nodo* actual = tope;
    bool esPrimero = true;
    while (actual != NULL) {
        cout << "      " << MAGENTA << "+-----------------------------+" << RESET;
        if (esPrimero) {
            cout << AMARILLO << " <-- TOPE (Ultimo entregado)" << RESET;
            esPrimero = false;
        }
        cout << "\n      " << MAGENTA << "| " << BLANCO << "Cod: " << actual->codigo 
             << " | " << actual->plato << MAGENTA << " |" << RESET << "\n";
        cout << "      " << MAGENTA << "+-----------------------------+" << RESET << "\n";
        cout << "                    " << BLANCO << "|" << RESET << "\n";
        cout << "                    " << BLANCO << "v" << RESET << "\n";
        actual = actual->sig;
    }
    cout << "                   " << ROJO << "NULL" << RESET << "\n\n";
}

// Función que busca un pedido por su código dentro de la Lista Enlazada y devuelve su dirección de memoria
Nodo* buscarEnListaPorCodigo(int codigo) {
    Nodo* actual = cabezaLista;       // Crea un puntero auxiliar que empieza apuntando al inicio de la lista
    while (actual != NULL) {          // Ciclo que se repite mientras no se llegue al final de la lista (NULL)
        if (actual->codigo == codigo) // Compara si el código del nodo actual coincide con el código buscado
            return actual;            // Si coincide, retorna la dirección de memoria de ese nodo y termina la función
        actual = actual->sig;         // Si no coincide, avanza el puntero auxiliar al siguiente nodo de la lista
    }                                 // Fin del ciclo while
    return NULL;                      // Si recorrió toda la lista y no lo encontró, retorna NULL
}                                     // Fin de la función buscarEnListaPorCodigo

// Función booleana que verifica si un pedido ya se encuentra en la Cola de cocina
bool estaEnCola(int codigo) {
    Nodo* actual = frenteCola;        // Crea un puntero auxiliar que empieza en el frente de la cola
    while (actual != NULL) {          // Recorre la cola mientras el puntero actual no sea NULL
        if (actual->codigo == codigo) // Verifica si el código del nodo en cola es igual al buscado
            return true;              // Si lo encuentra, retorna verdadero (true)
        actual = actual->sig;         // Avanza al siguiente nodo en la cola
    }                                 // Fin del ciclo while
    return false;                     // Si termina el recorrido sin encontrarlo, retorna falso (false)
}                                     // Fin de la función estaEnCola

// Funcion auxiliar que verifica si un pedido ya fue registrado en la Pila de entregas
bool estaEnPila(int codigo) {
    Nodo* actual = topePila;
    while (actual != NULL) {
        if (actual->codigo == codigo)
            return true;
        actual = actual->sig;
    }
    return false;
}

//---------------- 1. GESTOR DE PEDIDOS (LISTA ENLAZADA) ----------------//

// Procedimiento para registrar e insertar un nuevo pedido al final de la Lista Enlazada
void insertarPedidoLista() {
    cout << CIAN << "\n=== REGISTRAR NUEVO PEDIDO ===\n" << RESET;

    int codigo;
    do {
        cout << "Ingrese codigo del pedido (entero positivo): ";
        if (!(cin >> codigo)) {
            cout << ROJO << "Error: Debe ingresar un numero entero.\n" << RESET;
            cin.clear();
            cin.ignore(10000, '\n');
            codigo = -1;
            continue;
        }
        if (codigo <= 0) {
            cout << ROJO << "Error: El codigo debe ser un numero entero positivo.\n" << RESET;
        } else if (buscarEnListaPorCodigo(codigo) != NULL) {
            cout << ROJO << "Error: Ya existe un pedido con ese codigo.\n" << RESET;
            codigo = -1;
        }
    } while (codigo <= 0);

    string cliente, plato;
    cin.ignore(10000, '\n');

    do {
        cout << "Ingrese nombre del cliente: ";
        getline(cin, cliente);
        if (cliente.empty())
            cout << ROJO << "Error: El nombre no puede estar vacio.\n" << RESET;
    } while (cliente.empty());

    do {
        cout << "Ingrese nombre del plato: ";
        getline(cin, plato);
        if (plato.empty())
            cout << ROJO << "Error: El plato no puede estar vacio.\n" << RESET;
    } while (plato.empty());

    float precio;
    do {
        cout << "Ingrese precio del pedido (S/): ";
        if (!(cin >> precio)) {
            cout << ROJO << "Error: Debe ingresar un precio numerico.\n" << RESET;
            cin.clear();
            cin.ignore(10000, '\n');
            precio = -1;
            continue;
        }
        if (precio <= 0)
            cout << ROJO << "Error: El precio debe ser mayor a 0.\n" << RESET;
    } while (precio <= 0);

    // 1. Crear e insertar el pedido original en la Lista Enlazada
    Nodo* nuevo = new Nodo;
    nuevo->codigo = codigo;
    nuevo->cliente = cliente;
    nuevo->plato = plato;
    nuevo->precio = precio;
    nuevo->sig = NULL;

    if (cabezaLista == NULL) {
        cabezaLista = nuevo;
    } else {
        Nodo* actual = cabezaLista;
        while (actual->sig != NULL)
            actual = actual->sig;
        actual->sig = nuevo;
    }

    // 2. Crear una copia del pedido e insertarla automaticamente en la Cola FIFO
    Nodo* copia = new Nodo;
    copia->codigo = codigo;
    copia->cliente = cliente;
    copia->plato = plato;
    copia->precio = precio;
    copia->sig = NULL;

    if (frenteCola == NULL) {
        frenteCola = copia;
        finalCola = copia;
    } else {
        finalCola->sig = copia;
        finalCola = copia;
    }

    cout << VERDE << "\n[OK] Pedido registrado correctamente en la lista enlazada.\n";
    cout << "[OK] Pedido agregado automaticamente a la cola de cocina (FIFO).\n" << RESET;
}

// Procedimiento auxiliar para quitar un pedido de la Cola de cocina por su codigo
bool eliminarPedidoCola(int codigo) {
    Nodo* actual = frenteCola;
    Nodo* anterior = NULL;

    while (actual != NULL && actual->codigo != codigo) {
        anterior = actual;
        actual = actual->sig;
    }

    if (actual == NULL)
        return false;

    if (anterior == NULL)
        frenteCola = actual->sig;
    else
        anterior->sig = actual->sig;

    if (actual == finalCola)
        finalCola = anterior;

    delete actual;
    return true;
}

// Procedimiento auxiliar para quitar un pedido del historial (Pila) por su codigo
bool eliminarPedidoPila(int codigo) {
    Nodo* actual = topePila;
    Nodo* anterior = NULL;

    while (actual != NULL && actual->codigo != codigo) {
        anterior = actual;
        actual = actual->sig;
    }

    if (actual == NULL)
        return false;

    if (anterior == NULL)
        topePila = actual->sig;
    else
        anterior->sig = actual->sig;

    delete actual;
    return true;
}

// Procedimiento para eliminar un pedido de la Lista Enlazada y sincronizar las otras estructuras
void eliminarPedidoLista() {
    cout << CIAN << "\n=== ELIMINAR PEDIDO DEL SISTEMA ===\n" << RESET;
    int codigo;
    cout << "Ingrese codigo a eliminar: ";
    if (!(cin >> codigo)) {
        cout << ROJO << "Error: Debe ingresar un codigo numerico.\n" << RESET;
        cin.clear();
        cin.ignore(10000, '\n');
        return;
    }

    if (codigo <= 0) {
        cout << ROJO << "Error: El codigo debe ser un numero positivo.\n" << RESET;
        return;
    }

    Nodo* actual = cabezaLista;
    Nodo* anterior = NULL;

    while (actual != NULL && actual->codigo != codigo) {
        anterior = actual;
        actual = actual->sig;
    }

    if (actual == NULL) {
        cout << ROJO << "Pedido no encontrado en la lista.\n" << RESET;
        return;
    }

    // Elimina primero las copias asociadas para mantener sincronizadas las estructuras.
    bool estabaEnCola = eliminarPedidoCola(codigo);
    bool estabaEnPila = eliminarPedidoPila(codigo);

    if (anterior == NULL)
        cabezaLista = actual->sig;
    else
        anterior->sig = actual->sig;

    delete actual;

    cout << VERDE << "[OK] Pedido eliminado correctamente del sistema.\n" << RESET;
    if (estabaEnCola)
        cout << AMARILLO << "-> Tambien se retiro de la cola de cocina.\n" << RESET;
    if (estabaEnPila)
        cout << AMARILLO << "-> Tambien se retiro del historial de entregas.\n" << RESET;
}

// Procedimiento para buscar y mostrar los datos de un pedido específico en la Lista
void buscarPedidoLista() {
    cout << CIAN << "\n=== BUSCAR PEDIDO EN LISTA ===\n" << RESET; // Muestra encabezado
    int codigo;                                 // Variable para el código a buscar
    cout << "Ingrese codigo del pedido: ";      // Solicita el código
    cin >> codigo;                              // Lee el código

    Nodo* p = buscarEnListaPorCodigo(codigo);   // Llama a la función auxiliar de búsqueda y guarda el resultado en 'p'
    if (p != NULL) {                            // Si 'p' es distinto de NULL, el pedido sí existe
        cout << VERDE << "Encontrado -> " << RESET
             << "Codigo: " << BLANCO << p->codigo << RESET // Imprime el código del pedido
             << " | Cliente: " << p->cliente    // Imprime el nombre del cliente
             << " | Plato: " << p->plato        // Imprime el nombre del plato
             << " | Precio: S/ " << p->precio << "\n"; // Imprime el precio
    } else {                                    // Si 'p' es NULL
        cout << ROJO << "Pedido no encontrado.\n" << RESET; // Indica que no se encontró ningún registro
    }                                           // Fin del if-else
}                                               // Fin de la función buscarPedidoLista

// Procedimiento para recorrer e imprimir todos los pedidos almacenados en la Lista Enlazada
void mostrarListaPedidos() {
    cout << CIAN << "\n=== LISTA GENERAL DE PEDIDOS ===\n" << RESET; // Muestra encabezado
    if (cabezaLista == NULL) {                  // Comprueba si la lista está vacía
        cout << AMARILLO << "No hay pedidos registrados.\n" << RESET; // Muestra mensaje informativo
        return;                                 // Sale de la función
    }                                           // Fin del if

    // Dibuja el gráfico de nodos conectados en memoria
    graficarEstructuraLineal(cabezaLista, "CABEZA", CIAN);

    Nodo* actual = cabezaLista;                 // Puntero auxiliar que inicia en la cabeza de la lista
    while (actual != NULL) {                    // Recorre nodo por nodo hasta llegar a NULL
        cout << CIAN << "  * " << RESET
             << "Codigo: " << BLANCO << actual->codigo << RESET // Muestra el código del nodo actual
             << " | Cliente: " << actual->cliente // Muestra el cliente del nodo actual
             << " | Plato: " << actual->plato   // Muestra el plato del nodo actual
             << " | Precio: S/ " << actual->precio << "\n"; // Muestra el precio del nodo actual
        actual = actual->sig;                   // Avanza al siguiente nodo de la lista
    }                                           // Fin del ciclo while
}                                               // Fin de la función mostrarListaPedidos

//---------------- 2. COCINA (COLA - FIFO) ----------------//

// Procedimiento para insertar un pedido al final de la Cola de cocina (Orden de llegada)
void encolarPedidoCocina() {
    cout << CIAN << "\n=== ENVIAR PEDIDO A COCINA (ENCOLAR) ===\n" << RESET; // Muestra encabezado
    int codigo;                                 // Variable para almacenar el código del pedido a encolar
    do {                                        // Bucle de validación de entrada
        cout << "Ingrese codigo del pedido a preparar: "; // Pide el código
        cin >> codigo;                          // Lee el código
        if (codigo <= 0)                        // Valida que sea positivo
            cout << ROJO << "Error: Debe ingresar un numero entero positivo.\n" << RESET; // Mensaje de error
        cin.clear();                            // Limpia el flujo de entrada
    } while (codigo <= 0);                      // Repite mientras no sea positivo

    Nodo* p = buscarEnListaPorCodigo(codigo);   // Verifica que el pedido exista previamente en la Lista general
    if (p == NULL) {                            // Si no existe en la lista general
        cout << ROJO << "Error: No existe un pedido con ese codigo en la lista general.\n" << RESET; // Muestra error
        return;                                 // Sale de la función
    }                                           // Fin del if

    if (estaEnCola(codigo)) {                   // Verifica si ese pedido ya fue enviado a la cola anteriormente
        cout << AMARILLO << "Error: El pedido ya se encuentra en la cola de cocina.\n" << RESET; // Evita duplicados en cocina
        return;                                 // Sale de la función
    }                                           // Fin del if

    if (estaEnPila(codigo)) {
        cout << AMARILLO << "Error: El pedido ya fue atendido y esta en el historial de entregas.\n" << RESET;
        return;
    }

    Nodo* nuevo = new Nodo;                     // Crea un nuevo nodo en memoria dinámica para la Cola
    nuevo->codigo = p->codigo;                  // Copia el código desde el nodo encontrado en la lista
    nuevo->cliente = p->cliente;                // Copia el nombre del cliente
    nuevo->plato = p->plato;                    // Copia el nombre del plato
    nuevo->precio = p->precio;                  // Copia el precio del pedido
    nuevo->sig = NULL;                          // El puntero siguiente es NULL porque entra al final de la cola

    if (frenteCola == NULL) {                   // Si la cola de cocina está vacía
        frenteCola = nuevo;                     // El nuevo nodo es tanto el frente (primero)
        finalCola = nuevo;                      // Como el final (último) de la cola
    } else {                                    // Si ya hay pedidos esperando en la cola
        finalCola->sig = nuevo;                 // El último nodo actual apunta al nuevo nodo
        finalCola = nuevo;                      // Se actualiza el puntero finalCola al nuevo nodo ingresado
    }                                           // Fin del if-else

    cout << VERDE << "[OK] Pedido #" << codigo << " enviado a la cola de cocina.\n" << RESET; // Confirma que el pedido entró a cocina
}                                               // Fin de la función encolarPedidoCocina

// Procedimiento que saca el primer pedido de la Cola (FIFO) y lo apila en el historial de entregas (LIFO)
void desencolarYDespachar() {
    cout << CIAN << "\n=== DESENCOLAR Y DESPACHAR PEDIDO ===\n" << RESET; // Muestra encabezado
    if (frenteCola == NULL) {                   // Verifica si no hay pedidos pendientes en la cola
        cout << AMARILLO << "La cola de cocina esta vacia.\n" << RESET; // Muestra mensaje informativo
        return;                                 // Sale de la función
    }                                           // Fin del if

    Nodo* atendido = frenteCola;                // Captura el nodo que está al frente de la cola (el más antiguo)
    frenteCola = frenteCola->sig;               // Avanza el frente de la cola al siguiente pedido en espera
    if (frenteCola == NULL)                     // Si la cola quedó completamente vacía tras sacar este pedido
        finalCola = NULL;                       // Actualiza también finalCola a NULL para mantener consistencia

    cout << VERDE << "Pedido preparado y despachado:\n" << RESET; // Informa qué pedido se acaba de terminar
    cout << "Codigo: " << BLANCO << atendido->codigo << RESET // Imprime código del pedido atendido
         << " | Cliente: " << atendido->cliente // Imprime cliente del pedido atendido
         << " | Plato: " << atendido->plato     // Imprime plato del pedido atendido
         << " | Precio: S/ " << atendido->precio << "\n"; // Imprime precio del pedido atendido

    atendido->sig = topePila;                   // Enlaza el nodo atendido apuntando hacia el tope actual de la Pila
    topePila = atendido;                        // Convierte este nodo en el nuevo tope de la Pila (operación Push)
    cout << MAGENTA << "-> Registrado automaticamente en el historial de entregas (Pila).\n" << RESET; // Confirma transferencia a la Pila
}                                               // Fin de la función desencolarYDespachar

// Procedimiento para buscar un pedido específico dentro de la Cola de cocina
void buscarEnColaCocina() {
    cout << CIAN << "\n=== BUSCAR PEDIDO EN COCINA ===\n" << RESET; // Muestra encabezado
    int codigo;                                 // Variable para el código a buscar
    cout << "Ingrese codigo a buscar: ";        // Solicita el código
    cin >> codigo;                              // Lee el código

    Nodo* actual = frenteCola;                  // Puntero auxiliar que comienza en el frente de la cola
    while (actual != NULL) {                    // Recorre la cola hasta llegar a NULL
        if (actual->codigo == codigo) {         // Si encuentra el código buscado
            cout << VERDE << "En cocina -> " << RESET
                 << "Codigo: " << BLANCO << actual->codigo << RESET // Muestra código
                 << " | Cliente: " << actual->cliente // Muestra cliente
                 << " | Plato: " << actual->plato << "\n"; // Muestra plato
            return;                             // Sale de la función al haberlo encontrado
        }                                       // Fin del if
        actual = actual->sig;                   // Avanza al siguiente nodo de la cola
    }                                           // Fin del while
    cout << ROJO << "El pedido no esta en la cola de cocina.\n" << RESET; // Mensaje si recorrió toda la cola y no estaba
}                                               // Fin de la función buscarEnColaCocina

// Procedimiento para mostrar todos los pedidos que esperan en la Cola de cocina
void mostrarColaCocina() {
    cout << CIAN << "\n=== PEDIDOS EN COCINA (ORDEN DE LLEGADA - FIFO) ===\n" << RESET; // Muestra encabezado
    if (frenteCola == NULL) {                   // Verifica si la cola está vacía
        cout << AMARILLO << "La cola de cocina esta vacia.\n" << RESET; // Informa que no hay pedidos
        return;                                 // Sale de la función
    }                                           // Fin del if

    // Dibuja el gráfico de nodos conectados en la cola
    graficarEstructuraLineal(frenteCola, "FRENTE", VERDE);

    Nodo* actual = frenteCola;                  // Puntero auxiliar que inicia en el frente de la cola
    while (actual != NULL) {                    // Recorre todos los nodos hasta el final
        cout << VERDE << "  * " << RESET
             << "Codigo: " << BLANCO << actual->codigo << RESET // Muestra código
             << " | Cliente: " << actual->cliente // Muestra cliente
             << " | Plato: " << actual->plato   // Muestra plato
             << " | Precio: S/ " << actual->precio << "\n"; // Muestra precio
        actual = actual->sig;                   // Avanza al siguiente nodo
    }                                           // Fin del while
}                                               // Fin de la función mostrarColaCocina

//---------------- 3. HISTORIAL DE ENTREGAS (PILA - LIFO) ----------------//

// Procedimiento para insertar manualmente un pedido en el tope de la Pila de entregas (Push)
void apilarEntregaManual() {
    cout << CIAN << "\n=== REGISTRAR ENTREGA MANUAL (PUSH) ===\n" << RESET; // Muestra encabezado
    int codigo;                                 // Variable para el código del pedido
    do {                                        // Bucle de validación
        cout << "Ingrese codigo del pedido entregado: "; // Pide el código
        cin >> codigo;                          // Lee el código
        if (codigo <= 0)                        // Verifica que sea positivo
            cout << ROJO << "Error: Debe ingresar un numero entero positivo.\n" << RESET; // Error
        cin.clear();                            // Limpia cin
    } while (codigo <= 0);                      // Repite hasta ser válido

    Nodo* p = buscarEnListaPorCodigo(codigo);   // Busca el pedido en la Lista general
    if (p == NULL) {                            // Si no existe en la lista general
        cout << ROJO << "Error: No existe ese codigo en la lista general.\n" << RESET; // Muestra error
        return;                                 // Sale de la función
    }                                           // Fin del if

    if (estaEnCola(codigo)) {
        cout << AMARILLO << "Error: El pedido sigue pendiente en cocina; primero debe ser atendido.\n" << RESET;
        return;
    }
    if (estaEnPila(codigo)) {
        cout << AMARILLO << "Error: El pedido ya se encuentra en el historial de entregas.\n" << RESET;
        return;
    }

    Nodo* nuevo = new Nodo;                     // Reserva memoria para un nuevo nodo en la Pila
    nuevo->codigo = p->codigo;                  // Copia el código
    nuevo->cliente = p->cliente;                // Copia el cliente
    nuevo->plato = p->plato;                    // Copia el plato
    nuevo->precio = p->precio;                  // Copia el precio
    nuevo->sig = topePila;                      // El nuevo nodo apunta al antiguo tope de la pila
    topePila = nuevo;                           // El nuevo nodo se convierte en el nuevo tope de la pila

    cout << VERDE << "[OK] Pedido apilado en el historial de entregas.\n" << RESET; // Confirma la inserción en la pila
}                                               // Fin de la función apilarEntregaManual

// Procedimiento para eliminar y retirar el elemento superior de la Pila (Pop)
void desapilarEntregaPop() {
    cout << CIAN << "\n=== RETIRAR ULTIMA ENTREGA (POP) ===\n" << RESET; // Muestra encabezado
    if (topePila == NULL) {                     // Verifica si la pila está vacía
        cout << AMARILLO << "La pila de entregas esta vacia.\n" << RESET; // Muestra mensaje de pila vacía
        return;                                 // Sale de la función
    }                                           // Fin del if

    Nodo* temp = topePila;                      // Guarda la dirección del nodo superior (tope actual) en temp
    topePila = topePila->sig;                   // Mueve el tope de la pila al nodo que estaba debajo

    cout << AMARILLO << "Se retiro de la pila -> " << RESET
         << "Codigo: " << BLANCO << temp->codigo << RESET // Muestra código del pedido retirado
         << " | Cliente: " << temp->cliente     // Muestra cliente
         << " | Plato: " << temp->plato << "\n"; // Muestra plato

    delete temp;                                // Libera la memoria dinámica del nodo retirado
}                                               // Fin de la función desapilarEntregaPop

// Procedimiento para buscar un pedido dentro de la Pila de entregas
void buscarEnPilaEntregas() {
    cout << CIAN << "\n=== BUSCAR EN HISTORIAL DE ENTREGAS ===\n" << RESET; // Muestra encabezado
    int codigo;                                 // Variable para el código buscado
    cout << "Ingrese codigo a buscar: ";        // Solicita el código
    cin >> codigo;                              // Lee el código

    Nodo* actual = topePila;                    // Puntero auxiliar que inicia en el tope (cima) de la pila
    while (actual != NULL) {                    // Recorre hacia abajo mientras no llegue a NULL
        if (actual->codigo == codigo) {         // Si encuentra el código en la pila
            cout << VERDE << "Entregado -> " << RESET
                 << "Codigo: " << BLANCO << actual->codigo << RESET // Imprime código
                 << " | Cliente: " << actual->cliente // Imprime cliente
                 << " | Plato: " << actual->plato // Imprime plato
                 << " | Precio: S/ " << actual->precio << "\n"; // Imprime precio
            return;                             // Termina la función
        }                                       // Fin del if
        actual = actual->sig;                   // Baja al siguiente nodo de la pila
    }                                           // Fin del while
    cout << ROJO << "El pedido no se encuentra en la pila de entregas.\n" << RESET; // Mensaje si no está en la pila
}                                               // Fin de la función buscarEnPilaEntregas

// Procedimiento para mostrar todos los pedidos entregados (desde el último hasta el primero) y sumar ventas
void mostrarPilaEntregas() {
    cout << CIAN << "\n=== HISTORIAL DE ENTREGAS (MAS RECIENTE ARRIBA - LIFO) ===\n" << RESET; // Encabezado
    if (topePila == NULL) {                     // Comprueba si la pila está vacía
        cout << AMARILLO << "No hay entregas registradas.\n" << RESET; // Mensaje informativo
        return;                                 // Sale de la función
    }                                           // Fin del if

    // Dibuja la pila de forma vertical
    graficarPilaVertical(topePila);

    Nodo* actual = topePila;                    // Puntero auxiliar que comienza en el tope de la pila
    float totalVentas = 0;                      // Variable acumuladora para sumar el dinero recaudado
    while (actual != NULL) {                    // Recorre cada nodo de la pila hasta NULL
        cout << MAGENTA << "  * " << RESET
             << "Codigo: " << BLANCO << actual->codigo << RESET // Muestra código
             << " | Cliente: " << actual->cliente // Muestra cliente
             << " | Plato: " << actual->plato   // Muestra plato
             << " | Precio: S/ " << actual->precio << "\n"; // Muestra precio
        totalVentas += actual->precio;          // Suma el precio del nodo actual al acumulador totalVentas
        actual = actual->sig;                   // Avanza hacia el siguiente nodo inferior en la pila
    }                                           // Fin del while
    cout << VERDE << "\n  >> Total recaudado en entregas: S/ " << totalVentas << "\n" << RESET; // Imprime la suma total de ventas
}                                               // Fin de la función mostrarPilaEntregas

//---------------- 4. PERSISTENCIA DE DATOS (ARCHIVO TXT) ----------------//

// Procedimiento para guardar los pedidos de la Lista Enlazada en un archivo de texto
void guardarDatosArchivo() {
    ofstream archivo("pedidos.txt");            // Crea y abre el archivo "pedidos.txt" en modo escritura
    Nodo* actual = cabezaLista;                 // Puntero auxiliar que inicia en la cabeza de la lista
    while (actual != NULL) {                    // Recorre toda la lista enlazada
        archivo << actual->codigo << ";"        // Escribe el código seguido de un punto y coma (delimitador)
                << actual->cliente << ";"       // Escribe el nombre del cliente seguido de punto y coma
                << actual->plato << ";"         // Escribe el nombre del plato seguido de punto y coma
                << actual->precio << "\n";      // Escribe el precio y un salto de línea para el siguiente registro
        actual = actual->sig;                   // Avanza al siguiente nodo de la lista
    }                                           // Fin del while
    archivo.close();                            // Cierra el archivo para asegurar que los datos se guarden en disco
    cout << VERDE << "[OK] Datos guardados correctamente en 'pedidos.txt'.\n" << RESET; // Confirma el guardado exitoso
}                                               // Fin de la función guardarDatosArchivo

// Procedimiento para leer los pedidos desde el archivo de texto y cargarlos en la Lista Enlazada
void cargarDatosArchivo() {
    ifstream archivo("pedidos.txt");            // Abre el archivo "pedidos.txt" en modo lectura
    if (!archivo.is_open()) {                   // Verifica si el archivo existe y se pudo abrir correctamente
        cout << ROJO << "No se encontro el archivo 'pedidos.txt'.\n" << RESET; // Muestra aviso si no existe archivo previo
        return;                                 // Sale de la función
    }                                           // Fin del if

    int codigo;                                 // Variable temporal para leer el código
    string cliente, plato;                      // Variables temporales para leer cliente y plato
    float precio;                               // Variable temporal para leer el precio
    char sep;                                   // Variable auxiliar para capturar el carácter separador ';'

    while (archivo >> codigo >> sep) {          // Lee el entero 'codigo' y el primer ';' mientras haya líneas
        getline(archivo, cliente, ';');         // Lee el texto de 'cliente' hasta encontrar el siguiente ';'
        getline(archivo, plato, ';');           // Lee el texto de 'plato' hasta encontrar el siguiente ';'
        archivo >> precio;                      // Lee el número decimal 'precio' al final de la línea

        if (buscarEnListaPorCodigo(codigo) == NULL) { // Verifica que el código no esté ya cargado en memoria
            Nodo* nuevo = new Nodo{codigo, cliente, plato, precio, NULL}; // Crea el nodo con los datos leídos
            if (cabezaLista == NULL)            // Si la lista está vacía
                cabezaLista = nuevo;            // El nodo cargado es la cabeza
            else {                              // Si la lista ya tiene elementos
                Nodo* aux = cabezaLista;        // Puntero auxiliar para ir al final
                while (aux->sig != NULL)        // Recorre hasta el último nodo
                    aux = aux->sig;             // Avanza al siguiente
                aux->sig = nuevo;               // Enlaza el nodo cargado al final de la lista
            }                                   // Fin del if-else
        }                                       // Fin del if de verificación de duplicados
    }                                           // Fin del while de lectura de archivo
    archivo.close();                            // Cierra el archivo de lectura
    cout << VERDE << "[OK] Datos cargados correctamente desde 'pedidos.txt'.\n" << RESET; // Confirma la carga de datos
}                                               // Fin de la función cargarDatosArchivo

//---------------- MENÚ PRINCIPAL ----------------//

// Procedimiento que controla la interfaz de usuario por consola y los submenús
void menu() {
    int opcionPrincipal;                        // Variable para almacenar la opción elegida en el menú principal

    do {                                        // Bucle principal que mantiene el programa activo hasta elegir Salir (5)
        do {                                    // Bucle de validación para la opción del menú principal
            cout << AZUL << "\n======================================================\n" << RESET;
            cout << CIAN << "     SISTEMA DE GESTION DE PEDIDOS - RESTAURANTE\n" << RESET; // Título del sistema
            cout << AZUL << "======================================================\n" << RESET;
            cout << BLANCO << "  1. " << RESET << "Gestor de Pedidos " << CIAN << "(Lista Enlazada)\n" << RESET; // Opción 1
            cout << BLANCO << "  2. " << RESET << "Cocina " << VERDE << "(Cola - FIFO)\n" << RESET; // Opción 2
            cout << BLANCO << "  3. " << RESET << "Historial de Entregas " << MAGENTA << "(Pila - LIFO)\n" << RESET; // Opción 3
            cout << BLANCO << "  4. " << RESET << "Guardar / Cargar Datos " << AMARILLO << "(Archivo)\n" << RESET; // Opción 4
            cout << BLANCO << "  5. " << ROJO << "Salir\n" << RESET; // Opción 5
            cout << AZUL << "------------------------------------------------------\n" << RESET;
            cout << "Seleccione una opcion (1-5): "; // Solicita selección al usuario
            cin >> opcionPrincipal;             // Lee la opción ingresada

            if (opcionPrincipal < 1 || opcionPrincipal > 5) { // Valida si el número está fuera del rango 1-5
                cout << ROJO << "Opcion invalida. Por favor ingrese un numero entre 1 y 5.\n" << RESET; // Mensaje de error
            }                                   // Fin del if
            cin.clear();                        // Limpia errores de entrada
        } while (opcionPrincipal < 1 || opcionPrincipal > 5); // Repite mientras la opción no sea válida

        switch (opcionPrincipal) {              // Estructura selectiva múltiple según la opción principal elegida
            case 1: {                           // Caso 1: Submenú de la Lista Enlazada
                int opLista;                    // Variable para la opción del submenú de lista
                do {                            // Bucle del submenú de lista
                    cout << CIAN << "\n+--------------------------------------------+\n";
                    cout << "|        GESTOR DE PEDIDOS (LISTA)           |\n";
                    cout << "+--------------------------------------------+\n" << RESET;
                    cout << "  1. Registrar nuevo pedido\n"; // Subopción 1
                    cout << "  2. Eliminar pedido\n"; // Subopción 2
                    cout << "  3. Buscar pedido por codigo\n"; // Subopción 3
                    cout << "  4. Mostrar todos los pedidos (y grafico)\n"; // Subopción 4
                    cout << "  5. Volver al menu principal\n"; // Subopción 5
                    cout << "Seleccione una opcion (1-5): "; // Pide subopción
                    cin >> opLista;             // Lee subopción

                    switch (opLista) {          // Evalúa la subopción elegida
                        case 1: insertarPedidoLista(); break; // Llama a insertar en lista y sale del switch
                        case 2: eliminarPedidoLista(); break; // Llama a eliminar de lista y sale del switch
                        case 3: buscarPedidoLista(); break; // Llama a buscar en lista y sale del switch
                        case 4: mostrarListaPedidos(); break; // Llama a mostrar lista y sale del switch
                        case 5: cout << AMARILLO << "Volviendo al menu principal...\n" << RESET; break; // Mensaje de retorno
                        default: cout << ROJO << "Opcion invalida.\n" << RESET; break; // Manejo de opción fuera de rango
                    }                           // Fin del switch interno

                    system("pause");            // Pausa la consola ("Presione una tecla para continuar...")
                    system("cls");              // Limpia la pantalla de la consola
                } while (opLista != 5);         // Repite el submenú mientras no elija volver (5)
                break;                          // Sale del caso 1 del switch principal
            }                                   // Fin del bloque case 1

            case 2: {                           // Caso 2: Submenú de la Cola de Cocina
                int opCola;                     // Variable para la opción del submenú de cola
                do {                            // Bucle del submenú de cola
                    cout << VERDE << "\n+--------------------------------------------+\n";
                    cout << "|         COCINA (COLA DE ATENCION)          |\n";
                    cout << "+--------------------------------------------+\n" << RESET;
                    cout << "  1. Despachar siguiente pedido (Desencolar -> Pila)\n"; // Subopción 1
                    cout << "  2. Buscar pedido en cocina\n"; // Subopción 2
                    cout << "  3. Mostrar pedidos en cocina (y grafico)\n"; // Subopción 3
                    cout << "  4. Volver al menu principal\n"; // Subopción 4
                    cout << "Seleccione una opcion (1-4): "; // Pide subopción
                    cin >> opCola;              // Lee subopción

                    switch (opCola) {           // Evalúa subopción de cola
                        case 1: desencolarYDespachar(); break; // Desencola y apila entrega
                        case 2: buscarEnColaCocina(); break; // Busca en cola
                        case 3: mostrarColaCocina(); break; // Muestra cola
                        case 4: cout << AMARILLO << "Volviendo al menu principal...\n" << RESET; break; // Retorna
                        default: cout << ROJO << "Opcion invalida.\n" << RESET; break; // Opción incorrecta
                    }                           // Fin del switch interno

                    system("pause");            // Pausa la ejecución hasta presionar una tecla
                    system("cls");              // Limpia la pantalla
                } while (opCola != 4);          // Repite mientras no elija volver (4)
                break;                          // Sale del caso 2
            }                                   // Fin del bloque case 2

            case 3: {                           // Caso 3: Submenú de la Pila de Entregas
                int opPila;                     // Variable para la opción del submenú de pila
                do {                            // Bucle del submenú de pila
                    cout << MAGENTA << "\n+--------------------------------------------+\n";
                    cout << "|       HISTORIAL DE ENTREGAS (PILA)         |\n";
                    cout << "+--------------------------------------------+\n" << RESET;
                    cout << "  1. Retirar ultima entrega (Pop)\n"; // Subopción 1
                    cout << "  2. Buscar entrega por codigo\n"; // Subopción 2
                    cout << "  3. Ver historial de entregas (y grafico)\n"; // Subopción 3
                    cout << "  4. Volver al menu principal\n"; // Subopción 4
                    cout << "Seleccione una opcion (1-4): "; // Pide subopción
                    cin >> opPila;              // Lee subopción

                    switch (opPila) {           // Evalúa subopción de pila
                        case 1: desapilarEntregaPop(); break; // Elimina del tope (Pop)
                        case 2: buscarEnPilaEntregas(); break; // Busca en la pila
                        case 3: mostrarPilaEntregas(); break; // Muestra pila y total recaudado
                        case 4: cout << AMARILLO << "Volviendo al menu principal...\n" << RESET; break; // Retorna
                        default: cout << ROJO << "Opcion invalida.\n" << RESET; break; // Opción incorrecta
                    }                           // Fin del switch interno

                    system("pause");            // Pausa la consola
                    system("cls");              // Limpia la consola
                } while (opPila != 4);          // Repite mientras no elija volver (4)
                break;                          // Sale del caso 3
            }                                   // Fin del bloque case 3

            case 4: {                           // Caso 4: Submenú de Persistencia en Archivo
                int opArchivo;                  // Variable para la opción de archivo
                do {                            // Bucle del submenú de archivo
                    cout << AMARILLO << "\n+--------------------------------------------+\n";
                    cout << "|           PERSISTENCIA DE DATOS            |\n";
                    cout << "+--------------------------------------------+\n" << RESET;
                    cout << "  1. Guardar pedidos en archivo\n"; // Subopción 1
                    cout << "  2. Cargar pedidos desde archivo\n"; // Subopción 2
                    cout << "  3. Volver al menu principal\n"; // Subopción 3
                    cout << "Seleccione una opcion (1-3): "; // Pide subopción
                    cin >> opArchivo;           // Lee subopción

                    switch (opArchivo) {        // Evalúa subopción de archivo
                        case 1: guardarDatosArchivo(); break; // Guarda lista en pedidos.txt
                        case 2: cargarDatosArchivo(); break; // Carga lista desde pedidos.txt
                        case 3: cout << AMARILLO << "Volviendo al menu principal...\n" << RESET; break; // Retorna
                        default: cout << ROJO << "Opcion invalida.\n" << RESET; break; // Opción incorrecta
                    }                           // Fin del switch interno

                    system("pause");            // Pausa la consola
                    system("cls");              // Limpia la consola
                } while (opArchivo != 3);       // Repite mientras no elija volver (3)
                break;                          // Sale del caso 4
            }                                   // Fin del bloque case 4

            case 5:                             // Caso 5: Salir del sistema
                cout << VERDE << "Saliendo del programa...\n" << RESET; // Mensaje de despedida
                break;                          // Sale del switch principal
        }                                       // Fin del switch principal

    } while (opcionPrincipal != 5);             // Mantiene el programa ejecutándose mientras la opción no sea 5
}                                               // Fin de la función menu

// Función principal donde inicia la ejecución del programa en C++
int main() {
    system("cls");                              // Limpia la consola al iniciar para habilitar secuencias ANSI en Windows
    menu();                                     // Invoca a la función menu() para arrancar el sistema
    return 0;                                   // Retorna 0 al sistema operativo indicando ejecución exitosa
}                                               // Fin de la función main