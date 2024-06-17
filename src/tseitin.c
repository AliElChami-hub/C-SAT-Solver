#include "tseitin.h"

#include <stdio.h>

#include "err.h"
#include "util.h"
#include "variables.h"

/**
 * Inserts a clause with one literal into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    a literal
 */
void addUnaryClause(VarTable* vt, CNF* cnf, Literal a) {
    Clause* clause = mkTernaryClause(vt, a, 0, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with two literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 */
void addBinaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b) {
    Clause* clause = mkTernaryClause(vt, a, b, 0);
    addClauseToCNF(cnf, clause);
}

/**
 * Inserts a clause with three literals into the CNF.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param a    the first literal
 * @param b    the second literal
 * @param c    the third literal
 */
void addTernaryClause(VarTable* vt, CNF* cnf, Literal a, Literal b, Literal c) {
    Clause* clause = mkTernaryClause(vt, a, b, c);
    addClauseToCNF(cnf, clause);
}

/**
 * Adds clauses for a propositional formula to a CNF.
 *
 * For a propositional formula pf, clauses that are added that are equivalent to
 *
 *     x <=> pf
 *
 * where x is usually a fresh variable. This variable is also returned.
 *
 * @param vt   the underlying variable table
 * @param cnf  a formula
 * @param pf   a propositional formula
 * @return     the variable x, as described above
 */
VarIndex addClauses(VarTable* vt, CNF* cnf, const PropFormula* pf) {
    if (pf->kind == VAR) {
        // check if the kind is var and if yes return it as it is
        VarIndex Var_Index = pf->data.var;
        return Var_Index;

    } else if (pf->kind ==
               NOT) {  // if the kind is not then do the formula for it
        VarIndex lit1;
        lit1 = addClauses(vt, cnf, pf->data.single_op);
        VarIndex R = mkFreshVariable(vt);

        /////////////////////////////////////////////

        addBinaryClause(vt, cnf, -R, -lit1);
        addBinaryClause(vt, cnf, lit1, R);
        return R;

    } else if (pf->kind ==
               AND) {  // if the kind is AND then do the formula for it
        VarIndex lit1;
        lit1 = addClauses(vt, cnf, pf->data.operands[0]);
        VarIndex lit2;
        lit2 = addClauses(vt, cnf, pf->data.operands[1]);
        VarIndex R = mkFreshVariable(vt);
        //////////////////////////////////////////////

        addBinaryClause(vt, cnf, -R, lit1);
        addBinaryClause(vt, cnf, -R, lit2);
        addTernaryClause(vt, cnf, -lit1, -lit2, R);
        // addUnaryClause(vt, cnf, R);

        return R;
    } else if (pf->kind == OR) {  // if or do the formula for it as said
        VarIndex lit1;
        lit1 = addClauses(vt, cnf, pf->data.operands[0]);
        VarIndex lit2;
        lit2 = addClauses(vt, cnf, pf->data.operands[1]);
        VarIndex R = mkFreshVariable(vt);

        ///////////////////////////////////////////////

        addTernaryClause(vt, cnf, -R, lit1, lit2);
        addBinaryClause(vt, cnf, -lit1, R);
        addBinaryClause(vt, cnf, -lit2, R);
        // addUnaryClause(vt, cnf, R);

        return R;

    } else if (pf->kind == IMPLIES) {  // if the kind is implies then
        VarIndex lit1;
        lit1 = addClauses(vt, cnf, pf->data.operands[0]);
        VarIndex lit2;
        lit2 = addClauses(vt, cnf, pf->data.operands[1]);
        VarIndex R = mkFreshVariable(vt);

        /////////////////////////////////////////////////

        addTernaryClause(vt, cnf, -R, -lit1, lit2);
        addBinaryClause(vt, cnf, lit1, R);
        addBinaryClause(vt, cnf, -lit2, R);
        //  addUnaryClause(vt, cnf, R);

        return R;

    } else if (pf->kind ==
               EQUIV) {  // if the kind is equiv then do the formula for it
        VarIndex lit1;
        lit1 = addClauses(vt, cnf, pf->data.operands[0]);
        VarIndex lit2;
        lit2 = addClauses(vt, cnf, pf->data.operands[1]);
        VarIndex R = mkFreshVariable(vt);

        /////////////////////////////////////////////////////////

        addTernaryClause(vt, cnf, -R, -lit1, lit2);
        addTernaryClause(vt, cnf, -R, -lit2, lit1);
        addTernaryClause(vt, cnf, R, -lit1, -lit2);
        addTernaryClause(vt, cnf, R, lit1, lit2);
        //   addUnaryClause(vt, cnf, R);

        return R;
    }

    // Default case (should not be reached)
    // err("Not a correct kind or Invalid");
    return pf->data.var;
}

CNF* getCNF(VarTable* vt, const PropFormula* f) {
    CNF* res = mkCNF();

    VarIndex x = addClauses(vt, res, f);

    addUnaryClause(vt, res, x);

    return res;
}
