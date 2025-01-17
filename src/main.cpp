#include "../parser.tab.h"

#include <iostream>
#include <fstream>

extern FILE *yyin;
extern int yylex_destroy();

int main(int argc, char *argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: compiler <input_file_name> <output_file_name>";
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "Error: could not open file " << argv[1] << '\n';
        return 1;
    }

    AbstractSyntaxTree tree;

    yyparse(tree);
    std::cout << "Parsed\n";

    tree.printNodes();

    if (yyin) {
        fclose(yyin);
    }

    yylex_destroy();

    return 0;
}