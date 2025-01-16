#include "../parser.tab.h"

#include <iostream>
#include <fstream>

extern FILE *yyin;

int main(int argc, char **argv) {
    if (argc != 3) {
        std::cerr << "Usage: compiler <input_file_name> <output_file_name>";
        return 1;
    }

    yyin = fopen(argv[1], "r");
    if (!yyin) {
        std::cerr << "Error: could not open file " << argv[1] << '\n';
        return 1;
    }

    auto tree = std::make_shared<AbstractSyntaxTree>();

    yyparse(tree);
    std::cout << "Parsed\n";

    tree->printNodes();

    if (yyin) {
        fclose(yyin);
    }

    return 0;
}