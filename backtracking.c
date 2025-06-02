#include <stdio.h>
#include <string.h>

#define MAX_N 100
#define MAX_SOLUTIONS 10000

static int w[MAX_N];                     // Pesos de los votantes
static int include[MAX_N];               // Vector para marcar inclusión en la coalición actual
static int n;                            // Número de votantes
static int W;                            // Cuota mínima para que una coalición gane
static int critical_votes[MAX_N];        // Contador de votos críticos por votante
static int total_nodes;                  // Número de nodos explorados
static int solution_count;               // Número total de coaliciones ganadoras encontradas

// Matriz para guardar las coaliciones ganadoras (0 o 1 por votante)
static int winning_coalitions[MAX_SOLUTIONS][MAX_N];
static int coalitions_stored;            // Cuántas coaliciones hemos guardado


static int promising(int current_sum, int remaining) {
    return current_sum + remaining >= W;
}

static void reportSolution() {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        if (include[i]) sum += w[i];
    }

    for (int i = 0; i < n; i++) {
        if (include[i]) {
            int sum_without_i = sum - w[i];
            if (sum_without_i < W) {
                critical_votes[i]++;
            }
        }
    }

    if (coalitions_stored < MAX_SOLUTIONS) {
        memcpy(winning_coalitions[coalitions_stored], include, n * sizeof(int));
        coalitions_stored++;
    }
}

// Función recursiva para explorar todas las coaliciones cuya suma sea >= W
static void subset_sum_ge(int i, int current_sum, int remaining) {
    total_nodes++;

    if (i == n) {
        if (current_sum >= W) {
            reportSolution();
            solution_count++;
        }
        return;
    }

    if (!promising(current_sum, remaining)) return;

    // Incluye al votante i
    include[i] = 1;
    subset_sum_ge(i + 1, current_sum + w[i], remaining - w[i]);

    // No incluye al votante i
    include[i] = 0;
    subset_sum_ge(i + 1, current_sum, remaining - w[i]);
}

void banzhaf(int num_voters, int quota, int* weights) {
    n = num_voters;
    W = quota;
    memcpy(w, weights, n * sizeof(int));
    memset(include, 0, sizeof(include));
    memset(critical_votes, 0, sizeof(critical_votes));
    total_nodes = 0;
    solution_count = 0;
    coalitions_stored = 0;

    int total = 0;
    for (int i = 0; i < n; i++) total += w[i];

    subset_sum_ge(0, 0, total);
}

void banzhaf_get_critical_votes(int* out_critical_votes) {
    if (out_critical_votes != NULL) {
        memcpy(out_critical_votes, critical_votes, n * sizeof(int));
    }
}

int banzhaf_get_total_nodes() {
    return total_nodes;
}

int banzhaf_get_solution_count() {
    return solution_count;
}

// Retorna las coaliciones guardadas y la cantidad total
// *out_coalitions debe ser un puntero a un array de arrays de int 
int banzhaf_get_coalitions(int*** out_coalitions) {
    if (out_coalitions != NULL) {
        *out_coalitions = winning_coalitions;
    }
    return coalitions_stored;
}
