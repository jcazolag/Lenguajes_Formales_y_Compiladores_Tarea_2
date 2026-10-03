#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>

#define MAX_STATES 64
#define MAX_ALPHABET 26
#define MAX_LINE 1024

// Usamos uint64_t como un conjunto de estados (bitmask)
typedef uint64_t StateSet;

// Estructura para el NFA
typedef struct {
    int num_states;
    StateSet initial_states;
    char alphabet[MAX_ALPHABET];
    int alphabet_size;
    StateSet final_states;
    StateSet transitions[MAX_STATES + 1][MAX_ALPHABET]; // Indexado desde 1 a n
} NFA;

// Estructura para el DFA resultante
typedef struct {
    StateSet dfa_states[MAX_STATES * 10]; // Mapeo de ID de estado a conjunto de NFA
    int num_dfa_states;
    int transitions[MAX_STATES * 10][MAX_ALPHABET]; // Transiciones entre IDs de DFA
    bool is_final[MAX_STATES * 10];
    int initial_state_id;
} DFA;

// Función para parsear un conjunto en el formato "{1 2}" o "0"
StateSet parse_set(char *str) {
    StateSet set = 0;
    // Si es "0", es el conjunto vacío
    if (strcmp(str, "0") == 0 || trim_whitespace(str) == '0') {
        return 0;
    }
    
    char *p = strchr(str, '{');
    if (!p) return 0;
    p++; // Saltar '{'
    
    char *end = strchr(p, '}');
    if (end) *end = '\0'; // Cortar en '}'
    
    char *token = strtok(p, " \t\n\r");
    while (token != NULL) {
        int state = atoi(token);
        if (state > 0) {
            set |= ((StateSet)1 << state); // Agregar estado al bitmask
        }
        token = strtok(NULL, " \t\n\r");
    }
    return set;
}

// Helper para limpiar espacios iniciales/finales si es necesario
char trim_whitespace(char *str) {
    while(*str == ' ' || *str == '\t') str++;
    return *str;
}

void solve_case() {
    NFA nfa;
    memset(&nfa, 0, sizeof(NFA));
    
    char line[MAX_LINE];
    
    // 2. Número de estados
    if (!fgets(line, sizeof(line), stdin)) return;
    sscanf(line, "%d", &nfa.num_states);
    
    // 3. Estados iniciales del NFA
    if (!fgets(line, sizeof(line), stdin)) return;
    char *token = strtok(line, " \t\n\r");
    while (token != NULL) {
        int st = atoi(token);
        if (st > 0) nfa.initial_states |= ((StateSet)1 << st);
        token = strtok(NULL, " \t\n\r");
    }
    
    // 4. Alfabeto
    if (!fgets(line, sizeof(line), stdin)) return;
    token = strtok(line, " \t\n\r");
    while (token != NULL) {
        nfa.alphabet[nfa.alphabet_size++] = token[0];
        token = strtok(NULL, " \t\n\r");
    }
    
    // 5. Estados finales del NFA
    if (!fgets(line, sizeof(line), stdin)) return;
    token = strtok(line, " \t\n\r");
    while (token != NULL) {
        int st = atoi(token);
        if (st > 0) nfa.final_states |= ((StateSet)1 << st);
        token = strtok(NULL, " \t\n\r");
    }
    
    // 6. Filas de la tabla de transiciones del NFA
    for (int i = 1; i <= nfa.num_states; i++) {
        if (!fgets(line, sizeof(line), stdin)) break;
        
        // El primer token es el número del estado actual, lo saltamos
        token = strtok(line, " \t\r\n"); 
        
        // Los siguientes tokens corresponden a cada símbolo del alfabeto
        for (int a = 0; a < nfa.alphabet_size; a++) {
            // Buscamos el bloque completo del conjunto (con llaves o 0)
            char *start_ptr = strtok(NULL, "");
            if (!start_ptr) break;
            
            // Re-tokenizar conservando el formato de llaves
            while(*start_ptr == ' ' || *start_ptr == '\t') start_ptr++;
            
            char block[MAX_LINE];
            int len = 0;
            if (start_ptr[0] == '0') {
                strcpy(block, "0");
                // Avanzar el puntero original simulando strtok
                start_ptr++;
            } else if (start_ptr[0] == '{') {
                char *end_ptr = strchr(start_ptr, '}');
                if (end_ptr) {
                    len = end_ptr - start_ptr + 1;
                    strncpy(block, start_ptr, len);
                    block[len] = '\0';
                    start_ptr = end_ptr + 1;
                }
            }
            
            nfa.transitions[i][a] = parse_set(block);
            
            // Devolver el remanente a strtok simulado para la siguiente iteración
            if (a < nfa.alphabet_size - 1) {
                // Preparamos la cadena interna para la próxima lectura
                char temp[MAX_LINE];
                strcpy(temp, start_ptr);
                strtok(temp, ""); 
            }
        }
    }
    
    // --- Algoritmo de Construcción de Subconjuntos ---
    DFA dfa;
    memset(&dfa, 0, sizeof(DFA));
    
    // El estado inicial del DFA es el conjunto completo de iniciales de NFA
    dfa.dfa_states[1] = nfa.initial_states;
    dfa.num_dfa_states = 1;
    dfa.initial_state_id = 1;
    dfa.is_final[1] = (dfa.dfa_states[1] & nfa.final_states) != 0;
    
    int current = 1;
    while (current <= dfa.num_dfa_states) {
        StateSet current_set = dfa.dfa_states[current];
        
        for (int a = 0; a < nfa.alphabet_size; a++) {
            StateSet next_set = 0;
            
            // Para cada estado del NFA que está activo en el conjunto actual
            for (int i = 1; i <= nfa.num_states; i++) {
                if (current_set & ((StateSet)1 << i)) {
                    next_set |= nfa.transitions[i][a];
                }
            }
            
            // Verificar si este subconjunto ya existe en nuestro DFA
            int target_id = -1;
            for (int s = 1; s <= dfa.num_dfa_states; s++) {
                if (dfa.dfa_states[s] == next_set) {
                    target_id = s;
                    break;
                }
            }
            
            // Si no existe, creamos un nuevo estado en el DFA
            if (target_id == -1) {
                dfa.num_dfa_states++;
                dfa.dfa_states[dfa.num_dfa_states] = next_set;
                dfa.is_final[dfa.num_dfa_states] = (next_set & nfa.final_states) != 0;
                target_id = dfa.num_dfa_states;
            }
            
            dfa.transitions[current][a] = target_id;
        }
        current++;
    }
    
    // --- Impresión del Output ---
    // Imprimir cabecera del alfabeto
    printf("   ");
    for (int a = 0; a < nfa.alphabet_size; a++) {
        printf(" %c", nfa.alphabet[a]);
    }
    printf("\n");
    
    // Imprimir filas de transiciones del DFA
    for (int i = 1; i <= dfa.num_dfa_states; i++) {
        // Indicadores de inicial (->) y final (*)
        if (i == dfa.initial_state_id && dfa.is_final[i]) {
            printf("->* %d", i);
        } else if (i == dfa.initial_state_id) {
            printf("->  %d", i);
        } else if (dfa.is_final[i]) {
            printf(" *  %d", i);
        } else {
            printf("    %d", i);
        }
        
        // Transiciones del estado i
        for (int a = 0; a < nfa.alphabet_size; a++) {
            printf(" %d", dfa.transitions[i][a]);
        }
        printf("\n");
    }
}

int main() {
    int num_cases;
    char line[MAX_LINE];
    if (fgets(line, sizeof(line), stdin)) {
        sscanf(line, "%d", &num_cases);
        for (int i = 0; i < num_cases; i++) {
            solve_case();
        }
    }
    return 0;
}
