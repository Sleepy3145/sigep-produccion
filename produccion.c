/*
 * ============================================================================
 *  SISTEMA DE GESTION Y EVALUACION DE PRODUCCION (SIGEP)
 *  Fabrica de Componentes Electronicos
 * ----------------------------------------------------------------------------
 *  Asignatura : ISWZ1102 - Programacion I
 *  Actividad  : Ejercicio de programacion RC1
 *  Lenguaje   : C (estandar C99)
 *
 *  Descripcion:
 *    Programa que gestiona hasta 5 productos y, segun la demanda ingresada
 *    de cada uno, calcula:
 *      1. El tiempo total de fabricacion necesario.
 *      2. La cantidad total de recursos requeridos.
 *      3. Si la fabrica puede cumplir la demanda con el tiempo y los
 *         recursos disponibles.
 *    Tambien permite ingresar, editar (buscando el producto) y eliminar
 *    (buscando el producto) los productos registrados.
 *
 *  Restricciones respetadas:
 *    - No se usan estructuras (struct) ni archivos: los datos viven en
 *      memoria mientras el programa esta abierto.
 *    - Los datos se guardan en ARREGLOS PARALELOS: la posicion i de cada
 *      arreglo corresponde al mismo producto.
 *    - Los arreglos se pasan a las funciones mediante PUNTEROS.
 *    - La busqueda y la comparacion de textos son de elaboracion propia
 *      (no se usan bsearch, qsort, strcmp, strcasecmp ni similares).
 *
 *  Compilacion:  gcc -std=c99 -Wall -Wextra -o produccion produccion.c
 *  Ejecucion:    ./produccion        (en Windows: produccion.exe)
 * ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>   /* strtol, strtod */

/* ------------------------------------------------------------------------- */
/*  Constantes del sistema                                                    */
/* ------------------------------------------------------------------------- */
#define MAX_PRODUCTOS 5     /* La fabrica trabaja con 5 productos distintos  */
#define MAX_NOMBRE    40    /* Longitud maxima del nombre de un producto     */
#define MAX_LINEA     100   /* Tamano del bufer para leer del teclado        */
#define NO_ENCONTRADO (-1)  /* Valor que devuelve la busqueda si no halla    */

/* ------------------------------------------------------------------------- */
/*  Prototipos (programacion modular)                                        */
/* ------------------------------------------------------------------------- */

/* Utilidades de texto y lectura (logica propia) */
int    longitudTexto(const char *texto);
void   copiarTexto(char *destino, const char *origen, int maximo);
char   aMinuscula(char c);
int    textosIguales(const char *a, const char *b);
void   leerLinea(char *buffer, int tamano);
int    leerEntero(const char *mensaje, int minimo, int maximo);
double leerDecimal(const char *mensaje, double minimo);
void   leerNombre(const char *mensaje, char *destino);
void   pausar(void);

/* Gestion de productos */
int  buscarProducto(char nombres[][MAX_NOMBRE], int cantidad, const char *buscado);
void ingresarProducto(char nombres[][MAX_NOMBRE], int *demanda, double *tiempoUnit,
                      double *recursoUnit, int *cantidad);
void editarProducto(char nombres[][MAX_NOMBRE], int *demanda, double *tiempoUnit,
                    double *recursoUnit, int cantidad);
void eliminarProducto(char nombres[][MAX_NOMBRE], int *demanda, double *tiempoUnit,
                      double *recursoUnit, int *cantidad);
void mostrarProductos(char nombres[][MAX_NOMBRE], const int *demanda,
                      const double *tiempoUnit, const double *recursoUnit, int cantidad);

/* Capacidad y calculos de produccion */
void   configurarCapacidad(double *tiempoDisponible, double *recursosDisponibles);
double calcularTiempoTotal(const int *demanda, const double *tiempoUnit, int cantidad);
double calcularRecursosTotales(const int *demanda, const double *recursoUnit, int cantidad);
void   evaluarProduccion(char nombres[][MAX_NOMBRE], const int *demanda,
                         const double *tiempoUnit, const double *recursoUnit,
                         int cantidad, double tiempoDisponible,
                         double recursosDisponibles);

/* Interfaz */
void mostrarMenu(int cantidad, double tiempoDisponible, double recursosDisponibles);

/* ========================================================================= */
/*  FUNCION PRINCIPAL                                                         */
/* ========================================================================= */
int main(void)
{
    /* Arreglos paralelos: el indice i describe al mismo producto en todos */
    char   nombres[MAX_PRODUCTOS][MAX_NOMBRE];  /* nombre del producto        */
    int    demanda[MAX_PRODUCTOS];              /* unidades demandadas        */
    double tiempoUnit[MAX_PRODUCTOS];           /* horas por unidad           */
    double recursoUnit[MAX_PRODUCTOS];          /* recursos por unidad        */

    int    cantidad = 0;               /* productos registrados actualmente   */
    double tiempoDisponible = 0.0;     /* horas disponibles en la linea       */
    double recursosDisponibles = 0.0;  /* recursos disponibles en bodega      */
    int    opcion;

    do {
        mostrarMenu(cantidad, tiempoDisponible, recursosDisponibles);
        opcion = leerEntero("Seleccione una opcion: ", 0, 6);

        switch (opcion) {
            case 1:
                ingresarProducto(nombres, demanda, tiempoUnit, recursoUnit, &cantidad);
                break;
            case 2:
                editarProducto(nombres, demanda, tiempoUnit, recursoUnit, cantidad);
                break;
            case 3:
                eliminarProducto(nombres, demanda, tiempoUnit, recursoUnit, &cantidad);
                break;
            case 4:
                mostrarProductos(nombres, demanda, tiempoUnit, recursoUnit, cantidad);
                break;
            case 5:
                configurarCapacidad(&tiempoDisponible, &recursosDisponibles);
                break;
            case 6:
                evaluarProduccion(nombres, demanda, tiempoUnit, recursoUnit, cantidad,
                                  tiempoDisponible, recursosDisponibles);
                break;
            case 0:
                printf("\nGracias por usar SIGEP. Hasta pronto.\n");
                break;
        }
        if (opcion != 0) {
            pausar();
        }
    } while (opcion != 0);

    return 0;
}

/* ========================================================================= */
/*  UTILIDADES DE TEXTO (logica propia, sin funciones de <string.h>)          */
/* ========================================================================= */

/**
 * longitudTexto: cuenta los caracteres de una cadena hasta el '\0'.
 * Entrada : puntero al texto.
 * Salida  : numero de caracteres.
 */
int longitudTexto(const char *texto)
{
    int n = 0;
    while (*(texto + n) != '\0') {
        n++;
    }
    return n;
}

/**
 * copiarTexto: copia 'origen' en 'destino' sin pasar de 'maximo' - 1
 * caracteres y siempre termina la cadena con '\0'.
 */
void copiarTexto(char *destino, const char *origen, int maximo)
{
    int i = 0;
    while (origen[i] != '\0' && i < maximo - 1) {
        destino[i] = origen[i];
        i++;
    }
    destino[i] = '\0';
}

/**
 * aMinuscula: convierte una letra mayuscula (A-Z) en minuscula.
 * Los demas caracteres se devuelven sin cambios.
 */
char aMinuscula(char c)
{
    if (c >= 'A' && c <= 'Z') {
        return (char)(c + ('a' - 'A'));
    }
    return c;
}

/**
 * textosIguales: compara dos cadenas caracter por caracter sin distinguir
 * mayusculas de minusculas ("Resistor" == "resistor").
 * Salida: 1 si son iguales, 0 si son distintas.
 */
int textosIguales(const char *a, const char *b)
{
    int i = 0;
    while (a[i] != '\0' && b[i] != '\0') {
        if (aMinuscula(a[i]) != aMinuscula(b[i])) {
            return 0;
        }
        i++;
    }
    /* Son iguales solo si ambas terminaron al mismo tiempo */
    return a[i] == '\0' && b[i] == '\0';
}

/* ========================================================================= */
/*  LECTURA Y VALIDACION DE DATOS                                             */
/* ========================================================================= */

/**
 * leerLinea: lee una linea completa del teclado y quita el salto de linea.
 * Si el usuario escribe mas de lo que cabe, descarta el sobrante para que
 * no se cuele en la siguiente lectura.
 */
void leerLinea(char *buffer, int tamano)
{
    int c, i;

    if (fgets(buffer, tamano, stdin) == NULL) {
        buffer[0] = '\0';
        return;
    }
    i = longitudTexto(buffer);
    if (i > 0 && buffer[i - 1] == '\n') {
        buffer[i - 1] = '\0';
    } else {
        /* La linea era mas larga que el bufer: vaciar el resto */
        while ((c = getchar()) != '\n' && c != EOF) {
            /* descartar */
        }
    }
}

/**
 * leerEntero: pide un numero entero y repite la pregunta hasta que el
 * valor sea valido y este dentro del rango [minimo, maximo].
 */
int leerEntero(const char *mensaje, int minimo, int maximo)
{
    char  linea[MAX_LINEA];
    char *fin;
    long  valor;

    while (1) {
        printf("%s", mensaje);
        leerLinea(linea, MAX_LINEA);
        valor = strtol(linea, &fin, 10);
        if (fin != linea && *fin == '\0' && valor >= minimo && valor <= maximo) {
            return (int)valor;
        }
        printf("  >> Valor no valido. Ingrese un entero entre %d y %d.\n", minimo, maximo);
    }
}

/**
 * leerDecimal: pide un numero real mayor o igual que 'minimo' y repite la
 * pregunta hasta que el dato sea correcto.
 */
double leerDecimal(const char *mensaje, double minimo)
{
    char   linea[MAX_LINEA];
    char  *fin;
    double valor;

    while (1) {
        printf("%s", mensaje);
        leerLinea(linea, MAX_LINEA);
        valor = strtod(linea, &fin);
        if (fin != linea && *fin == '\0' && valor >= minimo) {
            return valor;
        }
        printf("  >> Valor no valido. Ingrese un numero mayor o igual a %.2f.\n", minimo);
    }
}

/**
 * leerNombre: pide un texto no vacio y lo guarda en 'destino'.
 */
void leerNombre(const char *mensaje, char *destino)
{
    char linea[MAX_LINEA];

    do {
        printf("%s", mensaje);
        leerLinea(linea, MAX_LINEA);
        if (longitudTexto(linea) == 0) {
            printf("  >> El nombre no puede quedar vacio.\n");
        }
    } while (longitudTexto(linea) == 0);

    copiarTexto(destino, linea, MAX_NOMBRE);
}

/** pausar: espera a que el usuario presione ENTER. */
void pausar(void)
{
    char linea[MAX_LINEA];
    printf("\nPresione ENTER para continuar...");
    leerLinea(linea, MAX_LINEA);
}

/* ========================================================================= */
/*  GESTION DE PRODUCTOS                                                      */
/* ========================================================================= */

/**
 * buscarProducto: BUSQUEDA SECUENCIAL (lineal) de elaboracion propia.
 * Recorre el arreglo de nombres desde la posicion 0 hasta 'cantidad' - 1 y
 * compara cada nombre con el buscado.
 * Salida: indice del producto, o NO_ENCONTRADO (-1) si no existe.
 * Complejidad: O(n); con n <= 5 la respuesta es inmediata.
 */
int buscarProducto(char nombres[][MAX_NOMBRE], int cantidad, const char *buscado)
{
    int i;
    for (i = 0; i < cantidad; i++) {
        if (textosIguales(nombres[i], buscado)) {
            return i;
        }
    }
    return NO_ENCONTRADO;
}

/**
 * ingresarProducto: registra un producto nuevo al final de los arreglos.
 * Valida que haya espacio y que el nombre no este repetido.
 * 'cantidad' se recibe por puntero porque la funcion la incrementa.
 */
void ingresarProducto(char nombres[][MAX_NOMBRE], int *demanda, double *tiempoUnit,
                      double *recursoUnit, int *cantidad)
{
    char nombre[MAX_NOMBRE];
    int  pos;

    printf("\n=== INGRESAR PRODUCTO ===\n");
    if (*cantidad >= MAX_PRODUCTOS) {
        printf("No hay espacio: ya hay %d productos registrados.\n", MAX_PRODUCTOS);
        printf("Elimine uno para poder ingresar otro.\n");
        return;
    }

    leerNombre("Nombre del producto: ", nombre);
    if (buscarProducto(nombres, *cantidad, nombre) != NO_ENCONTRADO) {
        printf("Ya existe un producto llamado \"%s\". No se registro.\n", nombre);
        return;
    }

    pos = *cantidad;                       /* primera posicion libre */
    copiarTexto(nombres[pos], nombre, MAX_NOMBRE);
    demanda[pos]     = leerEntero("Cantidad demandada (unidades): ", 0, 1000000);
    tiempoUnit[pos]  = leerDecimal("Tiempo de fabricacion por unidad (horas): ", 0.0);
    recursoUnit[pos] = leerDecimal("Recursos necesarios por unidad: ", 0.0);

    (*cantidad)++;
    printf("Producto \"%s\" registrado correctamente (%d de %d).\n",
           nombres[pos], *cantidad, MAX_PRODUCTOS);
}

/**
 * editarProducto: busca un producto por su nombre y permite cambiar su
 * nombre, demanda, tiempo por unidad o recursos por unidad.
 */
void editarProducto(char nombres[][MAX_NOMBRE], int *demanda, double *tiempoUnit,
                    double *recursoUnit, int cantidad)
{
    char buscado[MAX_NOMBRE];
    char nuevoNombre[MAX_NOMBRE];
    int  pos, opcion, otro;

    printf("\n=== EDITAR PRODUCTO ===\n");
    if (cantidad == 0) {
        printf("No hay productos registrados.\n");
        return;
    }

    leerNombre("Nombre del producto a editar: ", buscado);
    pos = buscarProducto(nombres, cantidad, buscado);
    if (pos == NO_ENCONTRADO) {
        printf("No se encontro el producto \"%s\".\n", buscado);
        return;
    }

    do {
        printf("\nProducto encontrado: %s\n", nombres[pos]);
        printf("  1. Nombre ................ %s\n", nombres[pos]);
        printf("  2. Demanda ............... %d unidades\n", demanda[pos]);
        printf("  3. Tiempo por unidad ..... %.3f h\n", tiempoUnit[pos]);
        printf("  4. Recursos por unidad ... %.2f\n", recursoUnit[pos]);
        printf("  0. Terminar edicion\n");
        opcion = leerEntero("Dato a modificar: ", 0, 4);

        switch (opcion) {
            case 1:
                leerNombre("Nuevo nombre: ", nuevoNombre);
                otro = buscarProducto(nombres, cantidad, nuevoNombre);
                if (otro != NO_ENCONTRADO && otro != pos) {
                    printf("  >> Ya existe otro producto con ese nombre.\n");
                } else {
                    copiarTexto(nombres[pos], nuevoNombre, MAX_NOMBRE);
                    printf("  Nombre actualizado.\n");
                }
                break;
            case 2:
                demanda[pos] = leerEntero("Nueva demanda (unidades): ", 0, 1000000);
                printf("  Demanda actualizada.\n");
                break;
            case 3:
                tiempoUnit[pos] = leerDecimal("Nuevo tiempo por unidad (horas): ", 0.0);
                printf("  Tiempo actualizado.\n");
                break;
            case 4:
                recursoUnit[pos] = leerDecimal("Nuevos recursos por unidad: ", 0.0);
                printf("  Recursos actualizados.\n");
                break;
        }
    } while (opcion != 0);
}

/**
 * eliminarProducto: busca un producto por su nombre y lo elimina
 * desplazando una posicion a la izquierda todos los productos que estan
 * despues de el, en los cuatro arreglos paralelos a la vez.
 */
void eliminarProducto(char nombres[][MAX_NOMBRE], int *demanda, double *tiempoUnit,
                      double *recursoUnit, int *cantidad)
{
    char buscado[MAX_NOMBRE];
    char eliminado[MAX_NOMBRE];
    int  pos, i, confirmar;

    printf("\n=== ELIMINAR PRODUCTO ===\n");
    if (*cantidad == 0) {
        printf("No hay productos registrados.\n");
        return;
    }

    leerNombre("Nombre del producto a eliminar: ", buscado);
    pos = buscarProducto(nombres, *cantidad, buscado);
    if (pos == NO_ENCONTRADO) {
        printf("No se encontro el producto \"%s\".\n", buscado);
        return;
    }

    printf("Se eliminara \"%s\" (demanda %d u.).\n", nombres[pos], demanda[pos]);
    confirmar = leerEntero("Confirmar? (1 = Si, 0 = No): ", 0, 1);
    if (confirmar == 0) {
        printf("Operacion cancelada.\n");
        return;
    }

    copiarTexto(eliminado, nombres[pos], MAX_NOMBRE);
    /* Desplazamiento a la izquierda para no dejar huecos */
    for (i = pos; i < *cantidad - 1; i++) {
        copiarTexto(nombres[i], nombres[i + 1], MAX_NOMBRE);
        demanda[i]     = demanda[i + 1];
        tiempoUnit[i]  = tiempoUnit[i + 1];
        recursoUnit[i] = recursoUnit[i + 1];
    }
    (*cantidad)--;
    printf("Producto \"%s\" eliminado. Quedan %d producto(s).\n", eliminado, *cantidad);
}

/**
 * mostrarProductos: imprime en forma de tabla todos los productos
 * registrados con sus datos.
 */
void mostrarProductos(char nombres[][MAX_NOMBRE], const int *demanda,
                      const double *tiempoUnit, const double *recursoUnit, int cantidad)
{
    int i;

    printf("\n=== PRODUCTOS REGISTRADOS (%d de %d) ===\n", cantidad, MAX_PRODUCTOS);
    if (cantidad == 0) {
        printf("No hay productos registrados.\n");
        return;
    }
    printf("%-3s %-22s %10s %12s %14s\n", "#", "Producto", "Demanda", "Horas/unid", "Recursos/unid");
    printf("--------------------------------------------------------------------\n");
    for (i = 0; i < cantidad; i++) {
        printf("%-3d %-22s %10d %12.3f %14.2f\n",
               i + 1, nombres[i], demanda[i], tiempoUnit[i], recursoUnit[i]);
    }
}

/* ========================================================================= */
/*  CAPACIDAD Y CALCULOS DE PRODUCCION                                        */
/* ========================================================================= */

/**
 * configurarCapacidad: registra el tiempo y los recursos disponibles de la
 * fabrica. Recibe punteros porque modifica variables declaradas en main.
 */
void configurarCapacidad(double *tiempoDisponible, double *recursosDisponibles)
{
    printf("\n=== CAPACIDAD DE LA FABRICA ===\n");
    printf("Valores actuales: %.2f horas | %.2f recursos\n",
           *tiempoDisponible, *recursosDisponibles);
    *tiempoDisponible    = leerDecimal("Tiempo de produccion disponible (horas): ", 0.0);
    *recursosDisponibles = leerDecimal("Cantidad de recursos disponibles: ", 0.0);
    printf("Capacidad actualizada.\n");
}

/**
 * calcularTiempoTotal: suma (demanda_i x tiempo por unidad_i) de todos los
 * productos.  Tiempo total = Suma de d_i x t_i
 */
double calcularTiempoTotal(const int *demanda, const double *tiempoUnit, int cantidad)
{
    double total = 0.0;
    int i;
    for (i = 0; i < cantidad; i++) {
        total += demanda[i] * tiempoUnit[i];
    }
    return total;
}

/**
 * calcularRecursosTotales: suma (demanda_i x recursos por unidad_i).
 * Recursos totales = Suma de d_i x r_i
 */
double calcularRecursosTotales(const int *demanda, const double *recursoUnit, int cantidad)
{
    double total = 0.0;
    int i;
    for (i = 0; i < cantidad; i++) {
        total += demanda[i] * recursoUnit[i];
    }
    return total;
}

/**
 * evaluarProduccion: muestra el detalle por producto, los totales y decide
 * si la fabrica puede cumplir la demanda.
 * Se cumple la demanda solo si:
 *      tiempo total   <= tiempo disponible   Y
 *      recursos total <= recursos disponibles
 */
void evaluarProduccion(char nombres[][MAX_NOMBRE], const int *demanda,
                       const double *tiempoUnit, const double *recursoUnit,
                       int cantidad, double tiempoDisponible,
                       double recursosDisponibles)
{
    double tiempoTotal, recursosTotales;
    int    cumpleTiempo, cumpleRecursos, i;

    printf("\n=== EVALUACION DE LA PRODUCCION ===\n");
    if (cantidad == 0) {
        printf("No hay productos registrados. Ingrese productos primero.\n");
        return;
    }
    if (tiempoDisponible <= 0.0 && recursosDisponibles <= 0.0) {
        printf("Aviso: la capacidad de la fabrica no ha sido configurada (opcion 5).\n");
    }

    /* Detalle por producto */
    printf("%-22s %8s %12s %14s\n", "Producto", "Demanda", "Horas", "Recursos");
    printf("------------------------------------------------------------\n");
    for (i = 0; i < cantidad; i++) {
        printf("%-22s %8d %12.2f %14.2f\n", nombres[i], demanda[i],
               demanda[i] * tiempoUnit[i], demanda[i] * recursoUnit[i]);
    }

    tiempoTotal     = calcularTiempoTotal(demanda, tiempoUnit, cantidad);
    recursosTotales = calcularRecursosTotales(demanda, recursoUnit, cantidad);
    cumpleTiempo    = tiempoTotal <= tiempoDisponible;
    cumpleRecursos  = recursosTotales <= recursosDisponibles;

    printf("------------------------------------------------------------\n");
    printf("%-22s %8s %12.2f %14.2f\n", "TOTAL REQUERIDO", "", tiempoTotal, recursosTotales);
    printf("%-22s %8s %12.2f %14.2f\n", "DISPONIBLE", "", tiempoDisponible, recursosDisponibles);

    printf("\nTiempo   : %s", cumpleTiempo ? "SUFICIENTE" : "INSUFICIENTE");
    if (cumpleTiempo) {
        printf(" (sobran %.2f h)\n", tiempoDisponible - tiempoTotal);
    } else {
        printf(" (faltan %.2f h)\n", tiempoTotal - tiempoDisponible);
    }

    printf("Recursos : %s", cumpleRecursos ? "SUFICIENTES" : "INSUFICIENTES");
    if (cumpleRecursos) {
        printf(" (sobran %.2f)\n", recursosDisponibles - recursosTotales);
    } else {
        printf(" (faltan %.2f)\n", recursosTotales - recursosDisponibles);
    }

    if (cumpleTiempo && cumpleRecursos) {
        printf("\nRESULTADO: La fabrica SI puede cumplir con la demanda.\n");
    } else {
        printf("\nRESULTADO: La fabrica NO puede cumplir con la demanda.\n");
    }
}

/* ========================================================================= */
/*  INTERFAZ                                                                  */
/* ========================================================================= */

/** mostrarMenu: imprime el menu principal con el estado actual. */
void mostrarMenu(int cantidad, double tiempoDisponible, double recursosDisponibles)
{
    printf("\n==============================================\n");
    printf("   SIGEP - Gestion y Evaluacion de Produccion\n");
    printf("   Fabrica de Componentes Electronicos\n");
    printf("==============================================\n");
    printf(" Productos: %d/%d | Tiempo disp.: %.2f h | Recursos disp.: %.2f\n",
           cantidad, MAX_PRODUCTOS, tiempoDisponible, recursosDisponibles);
    printf("----------------------------------------------\n");
    printf(" 1. Ingresar producto\n");
    printf(" 2. Editar producto\n");
    printf(" 3. Eliminar producto\n");
    printf(" 4. Ver productos\n");
    printf(" 5. Configurar capacidad (tiempo y recursos)\n");
    printf(" 6. Calcular y evaluar produccion\n");
    printf(" 0. Salir\n");
    printf("----------------------------------------------\n");
}
