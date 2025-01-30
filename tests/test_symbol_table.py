import os
import subprocess

import pytest

from .config import CORRECT_PROGRAMS_DIR, INPUT_FILENAME, MAKEFILE_PATH, OUTPUT_FILENAME


def _run_symbol_table_error_test(input_code: str, expected_error_message: str) -> None:
    with open(INPUT_FILENAME, "w") as input_file:
        input_file.write(input_code)

    result = subprocess.run(
        [os.path.join(MAKEFILE_PATH, "compiler"), INPUT_FILENAME, OUTPUT_FILENAME], capture_output=True, text=True
    )

    assert result.returncode == 1
    assert expected_error_message in result.stderr


def _run_symbol_table_working_test(input_code: str) -> None:
    with open(INPUT_FILENAME, "w") as input_file:
        input_file.write(input_code)

    result = subprocess.run(
        [os.path.join(MAKEFILE_PATH, "compiler"), INPUT_FILENAME, OUTPUT_FILENAME], capture_output=True, text=True
    )

    assert result.returncode == 0


@pytest.mark.parametrize("second_declaration", ["n", "n[1:10]"])
def test_redeclaration_of_number_variable(second_declaration):
    input_code = f"PROGRAM IS x, n, {second_declaration} BEGIN READ n; END"
    expected_error_message = "Error at line 1: redeclaration of variable `n`."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("second_declaration", ["tab", "tab[1:10]", "tab[-10:10]"])
def test_redeclaration_of_array(second_declaration):
    input_code = f"PROGRAM IS x, tab[1:10], {second_declaration} BEGIN READ n; END"
    expected_error_message = "Error at line 1: redeclaration of variable `tab`."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("amount_of_newlines", [1, 5, 100])
def test_error_line_location(amount_of_newlines):
    newlines = "\n" * amount_of_newlines
    input_code = f"PROGRAM IS x, n,{newlines}n BEGIN READ n; END"
    expected_error_message = f"Error at line {amount_of_newlines + 1}: redeclaration of variable `n`."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_incorrect_declaration_of_array():
    input_code = "PROGRAM IS x, tab[3:2] BEGIN READ n; END"
    expected_error_message = "Error at line 1: incorrect array range in `tab`. 3 is bigger than 2."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("undeclared_variable_name", ["n", "n[1]"])
def test_variable_not_declared(undeclared_variable_name):
    input_code = f"PROGRAM IS x, y BEGIN READ {undeclared_variable_name}; END"
    expected_error_message = "Error at line 1: variable `n` was not declared."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_variable_not_declared_in_brackets():
    input_code = f"PROGRAM IS t[-10:10] BEGIN READ t[n]; END"
    expected_error_message = "Error at line 1: variable `n` was not declared."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_variable_not_declared_in_brackets_in_procedure():
    input_code = f"PROCEDURE proc(x) IS t[-10:10] BEGIN READ t[n]; END PROGRAM IS x BEGIN READ x; END"
    expected_error_message = "Error at line 1: variable `n` was not declared."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_brackets_used_on_non_array():
    input_code = "PROGRAM IS x, y, n BEGIN READ n[2]; END"
    expected_error_message = "Error at line 1: variable `n` is not an array. [] cannot be used when accessing it."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_no_brackets_used_on_array():
    input_code = "PROGRAM IS x, y, tab[1:10] BEGIN READ tab; END"
    expected_error_message = "Error at line 1: variable `tab` is an array. [] need to be used when accessing it."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("wrong_index", ["-21", "31"])
def test_array_index_out_of_bounds(wrong_index):
    input_code = f"PROGRAM IS x, y, tab[-20:30] BEGIN READ tab[{wrong_index}]; END"
    expected_error_message = (
        f"Error at line 1: array index out of bounds for variable `tab`. Index: {wrong_index}, Range: [-20, 30]."
    )

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize(
    "second_declaration",
    ["PROCEDURE proc (a, T t) IS BEGIN READ a; END", "PROCEDURE proc (T a, b, c) IS BEGIN READ a; END"],
)
def test_procedure_redeclaration(second_declaration):
    input_code = f"PROCEDURE proc (a, T t) IS BEGIN READ a; END {second_declaration} PROGRAM IS a BEGIN READ a; END"
    expected_error_message = f"Error at line 1: redeclaration of procedure `proc`."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("second_declaration", ["a", "T a", "t", "T t"])
def test_procedure_argument_redeclaration(second_declaration):
    input_code = f"PROCEDURE proc (a, T t, {second_declaration}) IS BEGIN READ a; END PROGRAM IS a BEGIN READ a; END"
    expected_error_message = f"Error at line 1: redeclaration of argument `{second_declaration[-1]}`."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("argument", ["proc", "T proc"])
def test_argument_declared_with_same_name_as_procedure(argument):
    input_code = f"PROCEDURE proc (a, T t, {argument}) IS BEGIN READ a; END PROGRAM IS a BEGIN READ a; END"
    expected_error_message = "Error at line 1: argument with the same name as procedure `proc`."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("variable", ["proc", "proc[1:10]"])
def test_variable_declared_with_same_name_as_procedure(variable):
    input_code = f"PROCEDURE proc (a, T t) IS BEGIN READ a; END PROGRAM IS a, {variable} BEGIN READ a; END"
    expected_error_message = "Error at line 1: variable with the same name as procedure `proc`."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize(
    "declaration, redeclared_variable_name",
    [("a", "a"), ("a[1:10]", "a"), ("t", "t"), ("t[-10:-1]", "t"), ("x, a", "a")],
)
def test_procedure_variable_declared_with_same_name_as_argument(declaration, redeclared_variable_name):
    input_code = f"PROCEDURE proc (a, T t) IS {declaration} BEGIN READ a; END PROGRAM IS a BEGIN READ a; END"
    expected_error_message = f"Error at line 1: redeclaration of variable `{redeclared_variable_name}`."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_procedure_not_declared():
    input_code = f"PROGRAM IS x, y BEGIN proc(x, y); END"
    expected_error_message = "Error at line 1: procedure `proc` was not declared."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_procedure_not_declared_when_recursive_call():
    input_code = f"PROCEDURE proc (a, T t) IS BEGIN proc(a, t); END PROGRAM IS a BEGIN READ a; END"
    expected_error_message = "Error at line 1: procedure `proc` was not declared."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_too_many_variables_passed_to_procedure():
    input_code = f"PROCEDURE proc (a, T t) IS BEGIN READ a; END PROGRAM IS a, b, t[1:10] BEGIN proc(a, t, b); END"
    expected_error_message = "Error at line 1: too many arguments provided to `proc`. Expected: 2, Got: 3"

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_not_enough_variables_passed_to_procedure():
    input_code = f"PROCEDURE proc (a, T t) IS BEGIN READ a; END PROGRAM IS a BEGIN proc(a); END"
    expected_error_message = "Error at line 1: not enough arguments provided to `proc`. Expected: 2, Got: 1"

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("passed_variables", ["a, c", "c, a"])
def test_not_declared_variable_passed_to_procedure(passed_variables):
    input_code = f"PROCEDURE proc (a, T t) IS BEGIN READ a; END PROGRAM IS a BEGIN proc({passed_variables}); END"
    expected_error_message = "Error at line 1: variable `c` was not declared."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize(
    "variables, wrong_variable, types",
    [("a, b", "b", ["ARRAY", "NUMBER"]), ("a[-3:-1], b[5:7]", "a", ["NUMBER", "ARRAY"])],
)
def test_incorrect_type_passed_to_procedure(variables, wrong_variable, types):
    input_code = f"PROCEDURE proc (a, T t) IS BEGIN READ a; END PROGRAM IS {variables} BEGIN proc(a, b); END"
    expected_error_message = (
        f"Error at line 1: argument `{wrong_variable}` has incorrect type. Expected: {types[0]}, Got: {types[1]}."
    )

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize(
    "variables, wrong_variable, types",
    [("x, y", "y", ["ARRAY", "NUMBER"]), ("x[-3:-1], y[5:7]", "x", ["NUMBER", "ARRAY"])],
)
def test_incorrect_type_passed_to_procedure_from_another_procedure_declarations(variables, wrong_variable, types):
    input_code = f"PROCEDURE proc_a (a, T t) IS BEGIN READ a; END PROCEDURE proc_b(a, T t) IS {variables} BEGIN proc_a(x, y); END PROGRAM IS a BEGIN READ a; END"
    expected_error_message = (
        f"Error at line 1: argument `{wrong_variable}` has incorrect type. Expected: {types[0]}, Got: {types[1]}."
    )

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize(
    "arguments, wrong_variable, types",
    [("x, y", "y", ["ARRAY", "NUMBER"]), ("T x, T y", "x", ["NUMBER", "ARRAY"])],
)
def test_incorrect_type_passed_to_procedure_from_another_procedure_arguments(arguments, wrong_variable, types):
    input_code = f"PROCEDURE proc_a (a, T t) IS BEGIN READ a; END PROCEDURE proc_b({arguments}) IS BEGIN proc_a(x, y); END PROGRAM IS a BEGIN READ a; END"
    expected_error_message = (
        f"Error at line 1: argument `{wrong_variable}` has incorrect type. Expected: {types[0]}, Got: {types[1]}."
    )

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_undeclared_procedure_called_in_procedure():
    input_code = "PROCEDURE proc_a (a, T t) IS BEGIN proc_b(a, t); END PROGRAM IS BEGIN proc(a, b); END"
    expected_error_message = "Error at line 1: procedure `proc_b` was not declared."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_later_declared_procedure_called_in_procedure():
    input_code = "PROCEDURE proc_a (a, T t) IS BEGIN proc_b(a, t); END PROCEDURE proc_b (a, T t) IS BEGIN READ a; END PROGRAM IS BEGIN proc(a, b); END"
    expected_error_message = "Error at line 1: procedure `proc_b` was not declared."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_procedures_calling_each_other_in_correct_order_with_correct_arguments():
    input_code = (
        "PROCEDURE proc_a (a, T b, c, T d) IS BEGIN READ a; END\n"
        "PROCEDURE proc_b (a, T b) IS x, y, z[10:10] BEGIN proc_a(x, z, y, b); END\n"
        "PROCEDURE proc_c (a, b, T c) IS BEGIN proc_b(b, c); END\n"
        "PROGRAM IS a, b, c[-10:10] BEGIN proc_c(a, b, c); END"
    )

    _run_symbol_table_working_test(input_code)


@pytest.mark.parametrize("variable_declaration", ["i", "i[-10:10]"])
def test_for_loop_iterator_named_like_variable_in_main(variable_declaration):
    input_code = f"PROGRAM IS {variable_declaration}, n BEGIN FOR i FROM 1 TO n DO WRITE i; ENDFOR END"
    expected_error_message = "Error at line 1: redeclaration of variable `i`."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize(
    "arguments, variable_declarations", [("i", "n"), ("T i", "n"), ("x", "i, n"), ("x", "n, i[-10:10]")]
)
def test_for_loop_iterator_named_like_symbol_in_procedure(arguments, variable_declarations):
    input_code = f"PROCEDURE proc ({arguments}) IS {variable_declarations} BEGIN FOR i FROM 1 TO n DO WRITE i; ENDFOR END PROGRAM IS a BEGIN READ a; END"
    expected_error_message = "Error at line 1: redeclaration of variable `i`."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_iterator_passed_to_procedure():
    input_code = f"PROCEDURE proc (a, b, T t) IS BEGIN READ a; END PROGRAM IS n, x, y[-2:-1] BEGIN n := 10; FOR i FROM 1 TO n DO proc(x, i, y); ENDFOR END"
    expected_error_message = "Error at line 1: iterator variable `i` cannot be passed to procedure."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("command", ["READ i;", "i := 10;"])
def test_iterator_modified(command):
    input_code = f"PROGRAM IS n BEGIN n := 10; FOR i FROM 1 TO n DO {command} ENDFOR END"
    expected_error_message = "Error at line 1: modification of iterator variable `i` is not allowed."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("wrong_usage", ["t[x] := x;", "y := x;", "WRITE x;"])
def test_variable_not_initialized(wrong_usage):
    input_code = f"PROGRAM IS t[-10:10], x, y BEGIN {wrong_usage} END"
    expected_error_message = "Error at line 1: variable `x` was not initialized."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("initialization_command", ["READ x;", "x := 10;", "x := t[0];"])
def test_variable_initialized_correctly(initialization_command):
    input_code = f"PROGRAM IS t[-10:10], x, y BEGIN {initialization_command} t[0] := x; y := x; END"

    _run_symbol_table_working_test(input_code)


@pytest.mark.parametrize("wrong_usage", ["t[x] := x;", "y := x;", "WRITE x;"])
def test_variable_not_initialized_in_procedure(wrong_usage):
    input_code = f"PROCEDURE proc(p) IS t[-10:10], x, y BEGIN {wrong_usage} END PROGRAM IS n BEGIN READ n; END"
    expected_error_message = "Error at line 1: variable `x` was not initialized."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("initialization_command", ["READ x;", "x := 10;", "x := t[0];"])
def test_variable_initialized_correctly_in_procedure(initialization_command):
    input_code = f"PROCEDURE proc(p) IS t[-10:10], x, y BEGIN {initialization_command} t[0] := x; y := x; END PROGRAM IS n BEGIN READ n; END"

    _run_symbol_table_working_test(input_code)


@pytest.mark.parametrize("wrong_usage, variable", [("x := y;", "y"), ("y := x;", "x")])
def test_assigning_uninitialized_variables_to_each_other(wrong_usage, variable):
    input_code = f"PROGRAM IS t[-10:10], x, y BEGIN {wrong_usage} END"
    expected_error_message = f"Error at line 1: variable `{variable}` was not initialized."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("wrong_usage, variable", [("x := y;", "y"), ("y := x;", "x")])
def test_assigning_uninitialized_variables_to_each_other_in_procedure(wrong_usage, variable):
    input_code = f"PROCEDURE proc(p) IS t[-10:10], x, y BEGIN {wrong_usage} END PROGRAM IS n BEGIN READ n; END"
    expected_error_message = f"Error at line 1: variable `{variable}` was not initialized."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_assigning_uninitialized_variable_to_itself():
    input_code = f"PROGRAM IS x, y BEGIN x := x; y := x; END"
    expected_error_message = f"Error at line 1: variable `x` was not initialized."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_variable_initialized_afer_procedure_call():
    input_code = "PROCEDURE proc (a) IS n BEGIN READ n; END PROGRAM IS x, y BEGIN proc(x); y := x; END"

    _run_symbol_table_working_test(input_code)


def test_memory_limit_exceeded():
    array_max_index = 2**62
    input_code = f"PROGRAM IS t[0:{array_max_index}] BEGIN t[0] := 0; END"
    expected_error_message = f"Error at line 1: memory limit exceeded."

    _run_symbol_table_error_test(input_code, expected_error_message)


@pytest.mark.parametrize("correct_program_file_name", os.listdir(CORRECT_PROGRAMS_DIR))
def test_correct_programs(correct_program_file_name):
    with open(os.path.join(CORRECT_PROGRAMS_DIR, correct_program_file_name), "r") as file:
        input_code = file.read()

    _run_symbol_table_working_test(input_code)
