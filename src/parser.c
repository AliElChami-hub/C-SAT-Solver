#include "parser.h"

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "err.h"
#include "lexer.h"
#include "list.h"
#include "propformula.h"
#include "util.h"

/**
 * Assigns symbols to strings.
 *
 * Aborts the program with an error message if an invalid input is detected.
 *
 * @param str  a string to translate
 * @return     the resulting symbol
 */

FormulaKind toKind(const char* str) {
    if (strcmp(str, "&&") == 0) {
        return AND;  // compare if the kind is && and return its kind
    } else if (strcmp(str, "||") == 0) {
        return OR;  // compare if the kind is || and return its kind
    } else if (strcmp(str, "!") == 0) {
        return NOT;  // compare if the kind is ! and return its kind
    } else if (strcmp(str, "=>") == 0) {
        return IMPLIES;  // compare if the kind is => and return its kind
    } else if (strcmp(str, "<=>") == 0) {
        return EQUIV;  // compare if the kind is <=> and return its kind
    } else if (isalpha((unsigned char)str[0])) {  // to  be checked
        for (size_t i = 1; i < strlen(str); i++) {
            if (!isalnum((unsigned char)str[i])) {
                err("Invalid formula kind");
                return -1;  // Return an invalid formula kind
            }
        }
        return VAR;  // compare if the variable consist of numbers and letters
                     // and $ sign and return its kind
    } else {
        err("Invalid formula kind");
        return -1;  // Return an invalid formula kind
    }
}

PropFormula* parseFormula(FILE* input, VarTable* vt) {
    List stack = mkList();

    char* token =
        nextToken(input);  // take the first token to enter the while loop

    if (strlen(token) > 31 || token == NULL) {
        err("Token exceeded the capacity for the token");  // check if the token
                                                           // does not exceed
                                                           // the limit of the
                                                           // character
    }

    while (token != NULL) {
        FormulaKind kind;
        kind = toKind(token);

        if (strlen(token) > 31) {
            err("Token exceeded the capacity for the token");
            // check if the token does not exceed the limit of the characters
        }

        if (kind == VAR) {  // check if the kind is variable and enter
            PropFormula* formula_var =
                mkVarFormula(vt, token);  // make a formula with the variable

            push(&stack, formula_var);  // push the formula to the stack

        } else if (kind == NOT) {  // check if the kind is not and enter

            PropFormula* operand_Negation = peek(&stack);  // take the eoperand
            if (isEmpty(&stack) || operand_Negation == NULL) {
                err("Invalid formula syntax: Empty stack");
            }

            pop(&stack);  // pop the operand

            PropFormula* formula_NOT = mkUnaryFormula(
                kind, operand_Negation);  // make the unary formula

            if (formula_NOT == NULL) {
                err("Invalid formula syntax");
            }

            push(&stack, formula_NOT);
            free(token);

        } else if (kind == AND || kind == OR || kind == IMPLIES ||
                   kind == EQUIV) {  // check if the kind is binary and enter
            PropFormula* operand2 =
                peek(&stack);  // take the peek of the stack and save it

            if (isEmpty(&stack) || operand2 == NULL) {
                err("Invalid formula syntax: Empty stack");
            }

            pop(&stack);  // pop the head

            PropFormula* operand1 =
                peek(&stack);  // take the peek of the stack and save it
            if (isEmpty(&stack) || operand1 == NULL) {
                err("Invalid formula syntax: Empty stack");
            }

            pop(&stack);  // pop the head

            PropFormula* formula_Bin = mkBinaryFormula(
                kind, operand1, operand2);  // make the binary formula with the
                                            // two operands using the binary
            push(&stack, formula_Bin);

            free(token);  // free the token when you are done
        } else {
            err("Invalid Input");
        }

        token = nextToken(input);
    }

    PropFormula* formula = peek(&stack);

    pop(&stack);
    // free(token);
    clearList(&stack);
    return formula;
}
