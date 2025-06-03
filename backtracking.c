#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define MAX_N 12          // Máximo de votantes según especificación
#define MAX_SOLUTIONS 10000

typedef struct {
    int *votos;          // Array de votos en la coalición
    int size;            // Tamaño de la coalición
    bool *criticos;      // Array que marca votantes críticos
} Coalicion;

// Variables globales
int W;
static int w[MAX_N];              // Votos de cada votante
static bool include[MAX_N];       // Coalición actual durante backtracking
static int n;                     // Número de votantes
static int K;                     // Cuota necesaria para ganar
static int critical_votes[MAX_N]; // Contador de votos críticos
static int total_nodes;           // Nodos explorados
static int solution_count;        // Coaliciones ganadoras encontradas

static Coalicion *coaliciones_ganadoras = NULL;
static int num_coaliciones = 0;

// Función auxiliar para orden descendente
static int compare_desc(const void *a, const void *b) {
    return (*(int*)b - *(int*)a);
}

void clear_coalitions() {
    for (int i = 0; i < num_coaliciones; i++) {
        free(coaliciones_ganadoras[i].votos);
        free(coaliciones_ganadoras[i].criticos);
    }
    free(coaliciones_ganadoras);
    coaliciones_ganadoras = NULL;
    num_coaliciones = 0;
}

static void add_coalition(int *votos, bool *criticos) {
    // Redimensionar array
    Coalicion *temp = realloc(coaliciones_ganadoras, (num_coaliciones + 1) * sizeof(Coalicion));
    if (!temp) {
        perror("Error en realloc");
        return;
    }
    coaliciones_ganadoras = temp;

    // Configurar nueva coalición
    Coalicion *c = &coaliciones_ganadoras[num_coaliciones];
    c->size = n;
    c->votos = malloc(n * sizeof(int));
    c->criticos = malloc(n * sizeof(bool));
    
    if (!c->votos || !c->criticos) {
        perror("Error en malloc");
        return;
    }

    memcpy(c->votos, votos, n * sizeof(int));
    memcpy(c->criticos, criticos, n * sizeof(bool));
    num_coaliciones++;
}

static bool promising(int current_sum, int remaining) {
    return current_sum + remaining >= W;
}

static void process_solution() {
    int sum = 0;
    for (int i = 0; i < n; i++) {
        if (include[i]) sum += w[i];
    }

    int current_votes[MAX_N];
    bool criticos[MAX_N] = {false};

    for (int i = 0; i < n; i++) {
        current_votes[i] = include[i] ? w[i] : 0;
        
        if (include[i] && (sum - w[i] < K)) {
            critical_votes[i]++;
            criticos[i] = true;
        }
    }

    add_coalition(current_votes, criticos);
    solution_count++;
}

static void backtrack(int i, int current_sum, int remaining) {
    total_nodes++;

    if (i == n) {
        if (current_sum >= K) {
            process_solution();
        }
        return;
    }

    if (!promising(current_sum, remaining)) return;

    // Rama incluyendo al votante i
    include[i] = true;
    backtrack(i + 1, current_sum + w[i], remaining - w[i]);

    // Rama excluyendo al votante i
    include[i] = false;
    backtrack(i + 1, current_sum, remaining - w[i]);
}

void banzhaf(int num_voters, int quota, int* weights) {
    // Validación de entrada
    if (num_voters < 3 || num_voters > MAX_N || quota <= 0) {
        return;
    }

    // Inicialización
    n = num_voters;
    K = quota;
    memcpy(w, weights, n * sizeof(int));
    memset(include, 0, sizeof(include));
    memset(critical_votes, 0, sizeof(critical_votes));
    total_nodes = 0;
    solution_count = 0;
    clear_coalitions();

    // ordenar descendente
    qsort(w, n, sizeof(int), compare_desc);

    int total_votos = 0;
    for (int i = 0; i < n; i++) total_votos += w[i];

    if (K > total_votos) {
        return;
    }

    backtrack(0, 0, total_votos);
}

// Función para calcular el IPB
static void calculate_ipb(double ipb[n]) {
    int total_criticos = 0;
    for (int i = 0; i < n; i++) {
        total_criticos += critical_votes[i];
    }

    for (int i = 0; i < n; i++) {
        ipb[i] = (total_criticos > 0) ? 
                (double)critical_votes[i] / total_criticos : 0.0;
    }
}

// Función para obtener ambos resultados
void banzhaf_get_results(int* out_critical_votes, double* out_ipb) {
    if (out_critical_votes != NULL) {
        memcpy(out_critical_votes, critical_votes, n * sizeof(int));
    }
    
    if (out_ipb != NULL) {
        calculate_ipb(out_ipb);
    }
}

Coalicion* banzhaf_get_coalitions(int* num) {
    if (num) *num = num_coaliciones;
    return coaliciones_ganadoras;
}

int banzhaf_get_total_nodes() {
    return total_nodes;
}

int banzhaf_get_solution_count() {
    return solution_count;
}

void banzhaf_get_critical_votes(int* out_critical_votes) {
    if (out_critical_votes != NULL) {
        memcpy(out_critical_votes, critical_votes, n * sizeof(int));
    }
}