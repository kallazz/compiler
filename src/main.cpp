#include "../parser.tab.h"

#include <fstream>
#include <iostream>

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
    const int parsingResult = yyparse(tree);

    if (yyin) {
        fclose(yyin);
    }

    yylex_destroy();

    if (parsingResult == 1) {
        return 1;
    }

    if (!tree.fillSymbolTable()) {
        const CompilationError compilationError = tree.getCompilationError();
        printCompilationError(compilationError);
        return 1;
    }

    std::ofstream outputFile(argv[2]);
    if (!outputFile) {
        std::cerr << "Error: could not open file " << argv[2] << '\n';
        return 1;
    }

    const std::string assemblerCode = tree.generateAssembler();
    outputFile << assemblerCode;

    outputFile.close();

    return 0;
}
