import os
import subprocess

import pytest

from .config import INPUT_FILENAME, MAKEFILE_PATH, OUTPUT_FILENAME


def _run_symbol_table_error_test(input_code: str, expected_error_message: str) -> None:
    with open(INPUT_FILENAME, "w") as input_file:
        input_file.write(input_code)

    result = subprocess.run(
        [os.path.join(MAKEFILE_PATH, "compiler"), INPUT_FILENAME, OUTPUT_FILENAME], capture_output=True, text=True
    )

    assert result.returncode == 1
    assert expected_error_message in result.stderr


@pytest.mark.parametrize("second_declaration", ["n", "n[1:10]"])
def test_redeclaration_of_variable(second_declaration):
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


def test_brackets_used_on_non_array():
    input_code = "PROGRAM IS x, y, n BEGIN READ n[2]; END"
    expected_error_message = "Error at line 1: variable `n` is not an array. [] cannot be used in assignment."

    _run_symbol_table_error_test(input_code, expected_error_message)


def test_no_brackets_used_on_array():
    input_code = "PROGRAM IS x, y, tab[1:10] BEGIN READ tab; END"
    expected_error_message = "Error at line 1: variable `tab` is an array. [] need to be used in assignment."

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
    expected_error_message = f"Error at line 1: redeclaration of variable `{second_declaration[-1]}`."

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


def test_procedure_not_declared():
    input_code = f"PROGRAM IS x, y BEGIN proc(x, y); END"
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
