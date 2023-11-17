#include <iostream>

#define DEBUG

const int Max_Formula_Length = 256;

typedef enum Operation {
    Num, Var,
    Add, Sub, Mul, Div, Pow,
    Ln, Sin, Cos, Tan, Arcsin, Arccos, Arctan,
} Operation;

typedef struct Term {
    Operation operation = Num;
    int value = 0;              /* restore numbers' value */ 
    std::string literal = "";   /* restore variables' literal */
    Term *operand[2] = { nullptr, nullptr };

    int priority = 0;           /* for output */

    Term(){}
    Term(Term &term) {
        operation = term.operation;
        value = term.value;
        literal = term.literal;
        if (term.operand[0] != nullptr)
            operand[0] = new Term(*term.operand[0]);
        if (term.operand[1] != nullptr)
            operand[1] = new Term(*term.operand[1]);
    }
    Term(Operation __operation)
        { operation = __operation; }
    Term(Operation __operation, int __value) 
        { operation = __operation, value = __value; }
    Term(Operation __operation, std::string __literal) 
        { operation = __operation, literal = __literal; }
    Term(Operation __operation, Term *__left_operand) {
        operation = __operation;
        operand[0] = __left_operand;
    }
    Term(Operation __operation, Term *__left_operand, Term *__right_operand) {
        operation = __operation;
        operand[0] = __left_operand;
        operand[1] = __right_operand;
    }
    ~Term() {
        for (int k = 0; k < 2; k++) {
            if (operand[k] != nullptr) {
                operand[k]->~Term();
                delete operand[k];
            } 
        }
    }
} Term;

bool is_number(std::string s) {
    int len = s.length();
    for (int i = 0; i < len; i++) {
        if (s[i] < '0' || s[i] > '9') {
            return false;
        }
    }
    return true;
}

int priority(Operation operation) {
    if (operation == Num || operation == Var) return 0;
    else if (operation >= Ln && operation <= Arctan) return 1;
    else if (operation == Add || operation == Sub) return 2;
    else if (operation == Mul || operation == Div) return 3;
    else if (operation == Pow) return 4;
    
    return 0x7fffffff;
}

std::string binary_operator_symbol[] = {
    "+", "-", "mul", "/", "^"
};

std::string unary_operator_symbol[] = {
    "ln", "sin", "cos", "tan", "arcsin", "arccos", "arctan"
};

/* we seperate out the caculation of left&right operand to ensure the digit works correctly */
Term *build_formula_tree(int formula_len, std::string formula[], int &digit) {
    if (digit >= formula_len) {
        std::cerr << "Unexpected end of formula!\n";
        exit(-1);
    }

    if (is_number(formula[digit])) {
        return new Term(Num, atoi(formula[digit].c_str()));
    }

    for (int i = 0; i <= Pow - Add; i++) {
        if (formula[digit] == binary_operator_symbol[i]) {
            Term *left_operand = build_formula_tree(formula_len, formula, ++digit);
            Term *right_operand = build_formula_tree(formula_len, formula, ++digit);
            return new Term((Operation)(Add + i), left_operand, right_operand);
        }
    }

    for (int i = 0; i <= Arctan - Ln; i++) {
        if (formula[digit] == unary_operator_symbol[i]) {
            Term *left_operand = build_formula_tree(formula_len, formula, ++digit);
            return new Term((Operation)(Ln + i), left_operand);
        }
    }

    return new Term(Var, formula[digit]);
}

Term *simplify(Term *node) {
    if (node == nullptr) return node;

    Term *u = node->operand[0] = simplify(node->operand[0]);
    Term *v = node->operand[1] = simplify(node->operand[1]);
    Operation operation = node->operation;

    Term *res = node;

    return res;
}

Term *deriv_formula_tree(Term *formula_node, std::string &independent_variable) {

    std::string &x = independent_variable;
    Operation operation = formula_node->operation;
    Term *u = formula_node->operand[0];
    Term *v = formula_node->operand[1];

    Term *res = nullptr;

    if (operation == Num) {
        res =  new Term(Num, 0);
    }
    else if (operation == Var) {
        if (formula_node->literal == x) {
            res =  new Term(Num, 1);
        }
        else {
            res =  new Term(Num, 0);
        }
    }
    else if (operation == Add) {
        res =  new Term(
            Add,
            deriv_formula_tree(u, x),
            deriv_formula_tree(v, x)
        );
    }
    else if (operation == Sub) {
        res =  new Term(
            Sub,
            deriv_formula_tree(u, x),
            deriv_formula_tree(v, x)
        );
    }
    else if (operation == Mul) {
        res =  new Term(
            Add,
            new Term(
                Mul,
                deriv_formula_tree(u, x),
                new Term(*v)
            ),
            new Term(
                Mul,
                new Term(*u),
                deriv_formula_tree(v, x)
            )
        );
    }
    else if (operation == Div) {
        res =  new Term(
            Div,
            new Term(
                Sub,
                new Term(
                    Mul,
                    deriv_formula_tree(u, x),
                    new Term(*v)
                ),
                new Term(
                    Mul,
                    new Term(*u),
                    deriv_formula_tree(v, x)
                )
            ),
            new Term(
                Pow,
                new Term(*v),
                new Term(Num, 2)
            )
        );
    }
    /* d(u ^ v) = v * u ^ (v - 1) * du + u ^ v * ln(u) * dv */
    else if (operation == Pow) {
        res =  new Term(
            Add,
            new Term(
                Mul,
                new Term(
                    Mul,
                    new Term(*v),
                    new Term(
                        Pow,
                        new Term(*u),
                        new Term(
                            Sub,
                            new Term(*v),
                            new Term(Num, 1)
                        )
                    )
                ),
                deriv_formula_tree(u, x)
            ),
            new Term(
                Mul,
                new Term(
                    Mul,
                    new Term(
                        Pow,
                        new Term(*u),
                        new Term(*v)
                    ),
                    new Term(
                        Ln,
                        new Term(*u)
                    )
                ),
                deriv_formula_tree(v, x)
            )
        );
    }
    else if (operation == Ln) {
        res =  new Term(
            Mul,
            new Term(
                Div,
                new Term(Num, 1),
                new Term(*u)
            ),
            deriv_formula_tree(u, x)
        );
    }
    else if (operation == Sin) {
        res =  new Term(
            Mul,
            new Term(
                Cos,
                new Term(*u)
            ),
            deriv_formula_tree(u, x)
        );
    }
    else if (operation == Cos) {
        res =  new Term(
            Mul,
            new Term(Num, -1),
            new Term(
                Mul,
                new Term(
                    Sin,
                    new Term(*u)
                ),
                deriv_formula_tree(u, x)
            )
        );
    }
    else if (operation == Tan) {
        res = deriv_formula_tree(
            new Term(
                Div,
                new Term(Sin, new Term(*u)),
                new Term(Cos, new Term(*v))
            ),
            x
        );
    }

    return simplify(res);
}

void print_formula_tree(Term *node, int indent = 0) {
    if (node == nullptr) return;

    for (int i = 0; i < indent; i++) {
        std::cout << "  ";
    }

    std::string operation_name[] = {
        "Num", "Var",
        "Add", "Sub", "Mul", "Div", "Pow",
       " Ln", "Sin", "Cos", "Tan", "Arcsin", "Arccos", "Arctan",
    };

    if (node->operation == Num) {
        std::cout << operation_name[node->operation] << " " << node->value << std::endl;
    }
    else if (node->operation == Var) {
        std::cout << operation_name[node->operation] << " " << node->literal << std::endl;
    }
    else {
        std::cout << operation_name[node->operation] << std::endl;
    }

    print_formula_tree(node->operand[0], indent + 1);
    print_formula_tree(node->operand[1], indent + 1);
}

void output_formula_tree(Term *node, bool is_first_call = true) {
    if (node == nullptr) return;

    if (node->operation == Num) {
        std::cout << node->value << " ";
    }
    else if (node->operation == Var) {
        std::cout << node->literal << " ";
    }
    else if (node->operation >= Add && node->operation <= Sub) {
        output_formula_tree(node->operand[0], false);
        std::cout << binary_operator_symbol[node->operation - Add] << " ";
        output_formula_tree(node->operand[1], false);
    }
    else if (node->operation == Mul) {
        std::cout << "( ";
        output_formula_tree(node->operand[0], false);
        std::cout << ") ";
        std::cout << "*" << " ";
        std::cout << "( ";
        output_formula_tree(node->operand[1], false);
        std::cout << ") ";
    }
    else if (node->operation >= Div && node->operation <= Pow) {
        std::cout << "( ";
        output_formula_tree(node->operand[0], false);
        std::cout << ") ";
        std::cout << binary_operator_symbol[node->operation - Add] << " ";
        std::cout << "( ";
        output_formula_tree(node->operand[1], false);
        std::cout << ") ";
    }
    else if (node->operation >= Ln && node->operation <= Arctan) {
        std::cout << unary_operator_symbol[node->operation - Ln];
        std::cout << "( ";
        output_formula_tree(node->operand[0], false);
        std::cout << ") ";
    }
    if (is_first_call) std::cout << std::endl;
}

// #undef DEBUG
#ifdef DEBUG
int main(void) {
    int argc = 8;
    std::string argv[] = { "", "x", "^", "x", "sin", "mul", "2", "x" };
#else
int main(int argc, char **argv) {
#endif

    if (argc < 2) {
        std::cerr << "No variable specified!\n";
        exit(-1);
    }
    std::string independent_variable = argv[1];

    if (argc > 2 + Max_Formula_Length) {
        std::cerr << "The formula is too long!\n";
        exit(-1);
    }
    int formula_len = 0;
    std::string formula[Max_Formula_Length];
    for (int i = 2; i < argc; i++) {
        formula[formula_len++] = argv[i];
    }

    int digit = 0;
    Term *formula_root = build_formula_tree(formula_len, formula, digit);

    #ifdef DEBUG
        print_formula_tree(formula_root);

        output_formula_tree(formula_root);
    #endif

    Term *result_root = deriv_formula_tree(formula_root, independent_variable);

    #ifdef DEBUG
        print_formula_tree(result_root);
    #endif

    output_formula_tree(result_root);

    return 0;
}