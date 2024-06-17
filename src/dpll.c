#include "dpll.h"

#include <stdio.h>

#include "cnf.h"
#include "err.h"
#include "list.h"
#include "util.h"
#include "variables.h"

typedef enum Reason { CHOSEN, IMPLIED } Reason;

/**
 * Struct to represent an entry in the assignment stack. Should only be created
 * and freed by pushAssignment and popAssignment.
 */
typedef struct Assignment {
    VarIndex var;
    Reason reason;
} Assignment;

/**
 * Adds a new assignment to the assignment stack.
 *
 * @param stack  an assignment stack
 * @param var    the variable to assign
 * @param r      the reason for the assignment
 */
void pushAssignment(List* stack, VarIndex var, Reason r) {
    Assignment* a = (Assignment*)malloc(sizeof(Assignment));
    a->var = var;
    a->reason = r;
    push(stack, a);
}

/**
 * Removes the head element of an assignment stack and frees it.
 *
 * @param stack  an assignment stack
 */
void popAssignment(List* stack) {
    Assignment* a = (Assignment*)peek(stack);
    free(a);
    pop(stack);
}

/**
 * Führt eine Iteration des DPLL Algorithmus aus.
 *
 * @param vt       die zugrunde liegende Variablentabelle
 * @param stack    der Zuweisungsstack
 * @param cnf      die zu prüfende Formel
 * @return         1 falls der Algorithmus mit SAT terminieren sollte,
 *                 0 falls der Algorithmus weiterlaufen sollte,
 *                 -1 falls der Algorithmus mit UNSAT terminieren sollte
 */
/**
 * Performs one iteration of the DPLL algorithm.
 *
 * @param vt       the underlying variable table
 * @param stack    an assignment stack
 * @param cnf      the formula to check
 * @return         1 if the algorithm should terminate with SAT,
 *                 0 if the algorithm should continue,
 *                -1 if the algorithm should terminate with UNSAT
 */

int iterate(VarTable* vt, List* stack, CNF* cnf) {
    // Check if all clauses are fulfilled
    if (evalCNF(cnf) == TRUE) {
        return 1;  // Satisfiable, all clauses fulfilled
    }

    // Check if the clause is false
    if (evalCNF(cnf) == FALSE) {
        // Can we do a backtrack
        int reset_var = 0;
        ListIterator iter = mkIterator(stack);
        while (!isValid(&iter)) {
            Assignment* ass = getCurr(&iter);
            if (ass->reason == CHOSEN) {
                reset_var++;
            }
            next(&iter);
        }

        if (reset_var >= 1) {
            while (!isEmpty(stack)) {
                Assignment* top;  // take the top of the stack
                top = peek(stack);

                if (top->reason == CHOSEN) {
                    pushAssignment(stack, top->var,
                                   IMPLIED);  // Change reason to IMPLIED
                    updateVariableValue(vt, top->var,
                                        FALSE);  // update the value to false

                    return 0;

                } else {
                    updateVariableValue(
                        vt, top->var,
                        UNDEFINED);  // update the value to undefined
                                     // in the case of implied
                    popAssignment(stack);
                }
            }
        } else {
            return -1;
        }
    }

    // start a new iteration
    ListIterator it = mkIterator(&cnf->clauses);
    while (isValid(&it)) {
        Clause* currentClause = getCurr(&it);  // get the current clause

        // Check if unit clause exists
        Literal unitLiteral = getUnitLiteral(
            vt, currentClause);  // get the unit literal of the clause
        if (unitLiteral != 0) {
            VarIndex var = abs(unitLiteral);
            Reason reason = IMPLIED;

            if (unitLiteral > 0) {
                pushAssignment(stack, var, reason);
                updateVariableValue(vt, var, TRUE);

                return 0;
            } else {
                pushAssignment(stack, var, reason);
                updateVariableValue(vt, var, FALSE);

                return 0;
            }
        }
        // next iteration
        next(&it);
    }
    // Select the next free variable and set it to true
    VarIndex var = getNextUndefinedVariable(vt);

    if (var == 0) {
        return 0;  // Unsatisfiable, no more undefined variables
    }

    Reason reason = CHOSEN;

    pushAssignment(stack, var, reason);
    updateVariableValue(vt, var, TRUE);  // Update variable value

    return 0;  // Return 0 if the iteration is complete without finding a
               // solution
}

char isSatisfiable(VarTable* vt, CNF* cnf) {
    List stack = mkList();

    int res;
    do {
        res = iterate(vt, &stack, cnf);
    } while (res == 0);

    while (!isEmpty(&stack)) {
        popAssignment(&stack);
    }

    return (res < 0) ? 0 : 1;
}
