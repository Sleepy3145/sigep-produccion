# SIGEP – Sistema de Gestión y Evaluación de Producción

Programa en C para una fábrica de componentes electrónicos que produce **5 productos**.
A partir de la demanda de cada producto calcula:

1. El **tiempo total de fabricación** necesario: `T = Σ demanda[i] × tiempoUnit[i]`
2. Los **recursos totales** necesarios: `R = Σ demanda[i] × recursoUnit[i]`
3. Si la fábrica **puede cumplir la demanda**: solo si `T ≤ tiempo disponible` **y** `R ≤ recursos disponibles`

Además permite **ingresar**, **editar** (buscando el producto) y **eliminar** (buscando el producto) productos.

> Actividad evaluativa RC1 – ISWZ1102 Programación I.

## Restricciones que cumple

- No usa estructuras (`struct`) ni archivos: los datos viven en memoria mientras el programa está abierto.
- Guarda los datos en **arreglos paralelos** (`nombres`, `demanda`, `tiempoUnit`, `recursoUnit`) y los pasa a las funciones mediante **punteros**.
- La **búsqueda secuencial** (`buscarProducto`) y la comparación de textos (`textosIguales`) son de elaboración propia: no usa `bsearch`, `qsort`, `strcmp` ni similares.

## Compilar y ejecutar

```bash
gcc -std=c99 -Wall -Wextra -o produccion produccion.c
./produccion          # en Windows: produccion.exe
```

También se puede abrir `produccion.c` en Dev-C++, Code::Blocks o Visual Studio Code y compilarlo directamente.

## Menú

| Opción | Acción |
|---|---|
| 1 | Ingresar producto (máximo 5, sin nombres repetidos) |
| 2 | Editar producto (nombre, demanda, tiempo o recursos por unidad) |
| 3 | Eliminar producto (con confirmación) |
| 4 | Ver productos |
| 5 | Configurar capacidad (tiempo y recursos disponibles) |
| 6 | Calcular y evaluar producción |
| 0 | Salir |

## Ejemplo

| Producto | Demanda | Horas/u | Recursos/u |
|---|---|---|---|
| Resistor SMD | 2000 | 0.010 | 0.20 |
| Capacitor ceramico | 1500 | 0.015 | 0.30 |
| Diodo LED | 1000 | 0.020 | 0.40 |
| Transistor NPN | 800 | 0.030 | 0.60 |
| Placa PCB | 200 | 0.250 | 2.50 |

Con 160 horas y 2500 recursos disponibles, el programa calcula **136.50 h** y **2230 recursos** → la fábrica **SÍ** puede cumplir la demanda.

## Estructura del código

| Función | Responsabilidad |
|---|---|
| `main` | Declara los arreglos paralelos y controla el menú |
| `ingresarProducto` | Registra un producto nuevo |
| `buscarProducto` | Búsqueda secuencial propia (devuelve índice o -1) |
| `editarProducto` | Modifica los datos de un producto encontrado |
| `eliminarProducto` | Elimina desplazando los elementos siguientes |
| `mostrarProductos` | Lista los productos |
| `configurarCapacidad` | Registra tiempo y recursos disponibles |
| `calcularTiempoTotal` / `calcularRecursosTotales` | Sumatorias de producción |
| `evaluarProduccion` | Muestra totales y decide si se cumple la demanda |
| `textosIguales`, `copiarTexto`, `longitudTexto` | Manejo de cadenas propio |
| `leerEntero`, `leerDecimal`, `leerNombre` | Lectura validada desde el teclado |
