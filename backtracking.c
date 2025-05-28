#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_N 100

int w[MAX_N];       // Vector de números
int include[MAX_N]; // Vector de inclusión
int n;              // Cantidad de números
int W;              // Suma objetivo
int delta;          // Valor delta para la variante 2
int total_nodes;    // Contador de nodos visitados
int solution_count; // Contador de soluciones

void (*reportSolution)();

// Función para ordenar de mayor a menor
void sort_descending() {
    for (int i = 0; i < n-1; i++) {
        for (int j = i+1; j < n; j++) {
            if (w[i] < w[j]) {
                int temp = w[i];
                w[i] = w[j];
                w[j] = temp;
            }
        }
    }
}

// Función para imprimir una solución
void print_solution() {
    reportSolution();
    /*
    int counter = 0;
    printf("{ ");
    for (int i = 0; i < n; i++) {
        if (include[i]) {
            printf("%d ", w[i]);
            counter += w[i];
        }
    }
    printf("} %d\n", counter);
    */
}

// Promising para básica, suma mayor o igual y suma mayor o igual acotada
int promising(int current_sum, int remaining) {
    return current_sum + remaining >= W;
}

// Promising para suma exacta
int promising_exact(int current_sum, int remaining) {
    return current_sum <= W && current_sum + remaining >= W;
}

// Promising para suma en rango
int promising_range(int current_sum, int remaining) {
    int lower = W - delta;
    int upper = W + delta;
    
    return (current_sum + remaining >= lower) && (current_sum <= upper);
}

// Variante 1: Básica
void subset_sum_exact(int i, int current_sum, int remaining) {
    total_nodes++;

    if (current_sum == W) {
        print_solution();
        solution_count++;
        return;
    }

    // Si pasamos del final, nos pasamos de la suma o no hay posibilidad
    if (i >= n || current_sum > W || !promising_exact(current_sum, remaining)) {
        return;
    }

    // Incluir w[i]
    include[i] = 1;
    subset_sum_exact(i + 1, current_sum + w[i], remaining - w[i]);

    // Excluir w[i]
    include[i] = 0;
    subset_sum_exact(i + 1, current_sum, remaining - w[i]);
}

// Variante 2: Δ (delta) para rango
void subset_sum_range(int i, int current_sum, int remaining) {
    total_nodes++;

    // Solo verifica la solución cuando llega al final de un camino
    if (i == n) {
        if (current_sum >= (W - delta) && current_sum <= (W + delta)) {
            // Asegura que las soluciones no se repitan
            print_solution();
            solution_count++;
        }
        return;
    }

    // Poda: si no alcanza el mínimo o ya supera el máximo
    if (!promising_range(current_sum, remaining)) {
        return;
    }

    // Incluir w[i] solo si supera el límite superior
    if (current_sum + w[i] <= W + delta) {
        include[i] = 1;
        subset_sum_range(i + 1, current_sum + w[i], remaining - w[i]);
    }

    // No incluir w[i]
    include[i] = 0;
    subset_sum_range(i + 1, current_sum, remaining - w[i]);
}


// Variante 3: Mayor o igual
void subset_sum_ge(int i, int current_sum, int remaining) {
    total_nodes++;
    
    if (i == n) {
        if (current_sum >= W) {
            print_solution();
            solution_count++;
        }
        return;
    }

    // Poda: si ni sumando todos los elementos restantes se puede llegar a W
    if (!promising(current_sum, remaining)) {
        return;
    }

    // Incluir w[i]
    include[i] = 1;
    subset_sum_ge(i + 1, current_sum + w[i], remaining - w[i]);

    // No incluir w[i]
    include[i] = 0;
    subset_sum_ge(i + 1, current_sum, remaining - w[i]);
}

// Variante 4: Mayor o igual acotada
void subset_sum_ge_bounded(int i, int current_sum, int remaining) {
    total_nodes++;

    // Si ya alcanza la suma requerida
    if (current_sum >= W) {
        print_solution();
        solution_count++;
        return;
    }

    // Si ya no hay más elementos, o no se puede llegar a W, termina
    if (i == n || !promising(current_sum, remaining)) {
        return;
    }

    // Incluir w[i]
    include[i] = 1;
    subset_sum_ge_bounded(i + 1, current_sum + w[i], remaining - w[i]);

    // No incluir w[i]
    include[i] = 0;
    subset_sum_ge_bounded(i + 1, current_sum, remaining - w[i]);
}
