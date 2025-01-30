import os
import re
import subprocess

import pytest

from .config import INPUT_FILENAME, MAKEFILE_PATH, OUTPUT_FILENAME


def _run_assembler_generator_test(input_code: str, expected_output_numbers: list[int], user_input: str = "") -> None:
    with open(INPUT_FILENAME, "w") as input_file:
        input_file.write(input_code)

    compiler_result = subprocess.run(
        [os.path.join(MAKEFILE_PATH, "compiler"), INPUT_FILENAME, OUTPUT_FILENAME], capture_output=True, text=True
    )

    assert compiler_result.returncode == 0

    vm_result = subprocess.run(
        [os.path.join(MAKEFILE_PATH, "vm"), OUTPUT_FILENAME], capture_output=True, text=True, input=user_input
    )

    big_numbers_vm_result = subprocess.run(
        [os.path.join(MAKEFILE_PATH, "vm-cln"), OUTPUT_FILENAME], capture_output=True, text=True, input=user_input
    )

    assert vm_result.returncode == 0

    assert big_numbers_vm_result.returncode == 0

    vm_output_numbers = re.findall(r">\s*(-?\d+)", vm_result.stdout, re.MULTILINE)
    vm_output_numbers = list(map(int, vm_output_numbers))

    big_numbers_vm_output_numbers = re.findall(r">\s*(-?\d+)", vm_result.stdout, re.MULTILINE)
    big_numbers_vm_output_numbers = list(map(int, big_numbers_vm_output_numbers))

    assert expected_output_numbers == vm_output_numbers
    assert expected_output_numbers == big_numbers_vm_output_numbers


@pytest.mark.parametrize(
    "input_code, expected_output_numbers",
    [
        ("PROGRAM IS BEGIN WRITE 123; WRITE 124; END", [123, 124]),
        ("PROGRAM IS x[-5:5] BEGIN x[2] := 125; WRITE x[2]; END", [125]),
        ("PROGRAM IS n, x[-5:5] BEGIN x[3] := 126; n := 3; WRITE x[n]; END", [126]),
    ],
)
def test_write(input_code, expected_output_numbers):
    _run_assembler_generator_test(input_code, expected_output_numbers)


@pytest.mark.parametrize(
    "input_code, expected_output_numbers, user_input",
    [
        ("PROGRAM IS x BEGIN READ x; WRITE x; END", [123], "123\n"),
        ("PROGRAM IS x[-5:5] BEGIN READ x[-2]; WRITE x[-2]; END", [124], "124\n"),
        ("PROGRAM IS x[-5:5] BEGIN READ x[2]; WRITE x[2]; END", [125], "125\n"),
        ("PROGRAM IS x[-5:5], n BEGIN n := -5; READ x[n]; WRITE x[n]; END", [126], "126\n"),
        ("PROGRAM IS n, x[-5:5] BEGIN n := 5; READ x[n]; WRITE x[n]; END", [127], "127\n"),
    ],
)
def test_read(input_code, expected_output_numbers, user_input):
    _run_assembler_generator_test(input_code, expected_output_numbers, user_input)


@pytest.mark.parametrize(
    "input_code, expected_output_numbers",
    [
        ("PROGRAM IS x BEGIN x := 123; WRITE x; END", [123]),
        ("PROGRAM IS x[-10:-5] BEGIN x[-7] := 124; WRITE x[-7]; END", [124]),
        ("PROGRAM IS x[5:10] BEGIN x[7] := 125; WRITE x[7]; END", [125]),
        ("PROGRAM IS x[-5:5] BEGIN x[0] := 126; WRITE x[0]; END", [126]),
        ("PROGRAM IS n, x[5:10] BEGIN n := 7; x[n] := 127; WRITE x[n]; END", [127]),
        ("PROGRAM IS x[5:10], n BEGIN n := 7; x[n] := 128; WRITE x[n]; END", [128]),
    ],
)
def test_assignment(input_code, expected_output_numbers):
    _run_assembler_generator_test(input_code, expected_output_numbers)


@pytest.mark.parametrize(
    "input_code, expected_output_numbers",
    [
        ("PROGRAM IS x BEGIN x := 15 + 17; WRITE x; END", [32]),
        ("PROGRAM IS x BEGIN x := 15 - 17; WRITE x; END", [-2]),
        ("PROGRAM IS x, y, z BEGIN y := 3; z := 4; x := y + z; WRITE x; END", [7]),
        ("PROGRAM IS x, y, z BEGIN y := 3; z := 4; x := y - z; WRITE x; END", [-1]),
        ("PROGRAM IS x, y[1:10], z[1:10], n BEGIN n := 5; y[n] := 3; z[n] := 4; x := y[n] + z[n]; WRITE x; END", [7]),
        ("PROGRAM IS x, y BEGIN y := 15; x := y + 17; WRITE x; END", [32]),
        ("PROGRAM IS x, y BEGIN y := 17; x := 15 + y; WRITE x; END", [32]),
        ("PROGRAM IS x, y BEGIN y := 15; x := y - 17; WRITE x; END", [-2]),
        ("PROGRAM IS x, y BEGIN y := 17; x := 15 - y; WRITE x; END", [-2]),
        ("PROGRAM IS x, y[5:10], n BEGIN n := 5; y[n] := 17; x := 15 + y[n]; WRITE x; END", [32]),
        (
            "PROGRAM IS z[-7:7], x[-5:10], y[5:10], n BEGIN n := 5; y[n] := 17; x[n] := 15; z[n] := x[n] + y[n]; WRITE z[n]; END",
            [32],
        ),
        ("PROGRAM IS x BEGIN x := 24 * 11; WRITE x; END", [264]),
        ("PROGRAM IS x BEGIN x := 16 * 8; WRITE x; END", [128]),
        ("PROGRAM IS x BEGIN x := 7 * 5; WRITE x; END", [35]),
        ("PROGRAM IS x BEGIN x := 5 * 7; WRITE x; END", [35]),
        ("PROGRAM IS x BEGIN x := -5 * 7; WRITE x; END", [-35]),
        ("PROGRAM IS x BEGIN x := -5 * -7; WRITE x; END", [35]),
        ("PROGRAM IS x BEGIN x := 5 * -7; WRITE x; END", [-35]),
        ("PROGRAM IS x, y, z BEGIN y := 5; z := 7; x := y * z; WRITE x; END", [35]),
        ("PROGRAM IS x, y[-10:-3], z[7:15] BEGIN y[-5] := 5; z[12] := 7; x := y[-5] * z[12]; WRITE x; END", [35]),
        (
            "PROGRAM IS x, y[-10:-3], n, m, z[7:15] BEGIN n := -5; m := 12; y[-5] := 5; z[12] := 7; x := y[n] * z[m]; WRITE x; END",
            [35],
        ),
        (
            "PROGRAM IS x[-100:100], y[-10:-3], n, m, l, z[7:15] BEGIN l := 3; n := -5; m := 12; y[-5] := 5; z[12] := 7; x[l] := y[n] * z[m]; WRITE x[3]; END",
            [35],
        ),
        ("PROGRAM IS x BEGIN x := 12 / 2; WRITE x; END", [6]),
        ("PROGRAM IS x BEGIN x := 17 / 2; WRITE x; END", [8]),
        ("PROGRAM IS x BEGIN x := 9 / -2; WRITE x; END", [-5]),
        ("PROGRAM IS x BEGIN x := 9 / 2; WRITE x; END", [4]),
        ("PROGRAM IS x BEGIN x := 10 / -2; WRITE x; END", [-5]),
        ("PROGRAM IS x BEGIN x := 10 / 2; WRITE x; END", [5]),
        ("PROGRAM IS x BEGIN x := 10 / 0; WRITE x; END", [0]),
        ("PROGRAM IS x, t[1:10] BEGIN t[5] := 17; x := t[5] / 2; WRITE x; END", [8]),
        ("PROGRAM IS x, t[1:10], n BEGIN n := 5; t[5] := 17; x := t[n] / 2; WRITE x; END", [8]),
        ("PROGRAM IS x BEGIN x := 24 / 11; WRITE x; END", [2]),
        ("PROGRAM IS x BEGIN x := 16 / 8; WRITE x; END", [2]),
        ("PROGRAM IS x BEGIN x := 50 / 10; WRITE x; END", [5]),
        ("PROGRAM IS x BEGIN x := 50 / -10; WRITE x; END", [-5]),
        ("PROGRAM IS x BEGIN x := -50 / 10; WRITE x; END", [-5]),
        ("PROGRAM IS x BEGIN x := -50 / -10; WRITE x; END", [5]),
        ("PROGRAM IS x BEGIN x := -3 / 2; WRITE x; END", [-2]),
        ("PROGRAM IS x, y, z BEGIN y := 35; z := 5; x := y / z; WRITE x; END", [7]),
        ("PROGRAM IS x, y[-10:-3], z[7:15] BEGIN y[-5] := 35; z[12] := 5; x := y[-5] / z[12]; WRITE x; END", [7]),
        (
            "PROGRAM IS x, y[-10:-3], n, m, z[7:15] BEGIN n := -5; m := 12; y[-5] := 35; z[12] := 5; x := y[n] / z[m]; WRITE x; END",
            [7],
        ),
        (
            "PROGRAM IS x[-100:100], y[-10:-3], n, m, l, z[7:15] BEGIN l := 3; n := -5; m := 12; y[-5] := 35; z[12] := 5; x[l] := y[n] / z[m]; WRITE x[3]; END",
            [7],
        ),
        ("PROGRAM IS x BEGIN x := 12 % 2; WRITE x; END", [0]),
        ("PROGRAM IS x BEGIN x := 13 % 2; WRITE x; END", [1]),
        ("PROGRAM IS x BEGIN x := 127 % 12; WRITE x; END", [7]),
        # ("PROGRAM IS x BEGIN x := 10 % 0; WRITE x; END", [0]),
        ("PROGRAM IS x BEGIN x := 10 % 3; WRITE x; END", [1]),
        ("PROGRAM IS x BEGIN x := 10 % -3; WRITE x; END", [-2]),
        ("PROGRAM IS x BEGIN x := -10 % 3; WRITE x; END", [2]),
        ("PROGRAM IS x BEGIN x := -10 % -3; WRITE x; END", [-1]),
        ("PROGRAM IS x BEGIN x := 8968 % -8; WRITE x; END", [0]),
        ("PROGRAM IS x BEGIN x := -5467 % 11; WRITE x; END", [0]),
        ("PROGRAM IS x BEGIN x := 548 % -2901; WRITE x; END", [-2353]),
        (
            "PROGRAM IS x[-100:100], y[-10:-3], n, m, l, z[7:15] BEGIN l := 3; n := -5; m := 12; y[-5] := 35; z[12] := 6; x[l] := y[n] % z[m]; WRITE x[3]; END",
            [5],
        ),
    ],
)
def test_assignment_with_expression(input_code, expected_output_numbers):
    _run_assembler_generator_test(input_code, expected_output_numbers)


@pytest.mark.parametrize(
    "input_code, expected_output_numbers",
    [
        ("PROGRAM IS BEGIN IF 15 = 17 THEN WRITE 1; ENDIF END", []),
        ("PROGRAM IS BEGIN IF 17 = 17 THEN WRITE 1; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 15 = 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [0]),
        ("PROGRAM IS BEGIN IF 17 = 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 15 != 17 THEN WRITE 1; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 != 17 THEN WRITE 1; ENDIF END", []),
        ("PROGRAM IS BEGIN IF 15 != 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 != 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [0]),
        ("PROGRAM IS BEGIN IF 15 > 17 THEN WRITE 1; ENDIF END", []),
        ("PROGRAM IS BEGIN IF 17 > 17 THEN WRITE 1; ENDIF END", []),
        ("PROGRAM IS BEGIN IF 17 > 15 THEN WRITE 1; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 15 > 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [0]),
        ("PROGRAM IS BEGIN IF 17 > 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [0]),
        ("PROGRAM IS BEGIN IF 17 > 15 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 15 < 17 THEN WRITE 1; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 < 17 THEN WRITE 1; ENDIF END", []),
        ("PROGRAM IS BEGIN IF 17 < 15 THEN WRITE 1; ENDIF END", []),
        ("PROGRAM IS BEGIN IF 15 < 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 < 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [0]),
        ("PROGRAM IS BEGIN IF 17 < 15 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [0]),
        ("PROGRAM IS BEGIN IF 15 >= 17 THEN WRITE 1; ENDIF END", []),
        ("PROGRAM IS BEGIN IF 17 >= 17 THEN WRITE 1; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 >= 15 THEN WRITE 1; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 15 >= 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [0]),
        ("PROGRAM IS BEGIN IF 17 >= 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 >= 15 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 15 <= 17 THEN WRITE 1; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 <= 17 THEN WRITE 1; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 <= 15 THEN WRITE 1; ENDIF END", []),
        ("PROGRAM IS BEGIN IF 15 <= 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 <= 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [1]),
        ("PROGRAM IS BEGIN IF 17 <= 15 THEN WRITE 1; ELSE WRITE 0; ENDIF END", [0]),
    ],
)
def test_if(input_code, expected_output_numbers):
    _run_assembler_generator_test(input_code, expected_output_numbers)


def test_while_loop():
    input_code = "PROGRAM IS i BEGIN i := 1; WHILE i <= 10 DO WRITE i; i := i + 1; ENDWHILE END"
    expected_output_numbers = list(range(1, 11))

    _run_assembler_generator_test(input_code, expected_output_numbers)


def test_repeat_loop():
    input_code = "PROGRAM IS i BEGIN i := 1; REPEAT WRITE i; i := i + 1; UNTIL i > 10; END"
    expected_output_numbers = list(range(1, 11))

    _run_assembler_generator_test(input_code, expected_output_numbers)


@pytest.mark.parametrize(
    "input_code, expected_output_numbers",
    [
        ("PROGRAM IS BEGIN FOR i FROM 5 TO 10 DO WRITE i; ENDFOR END", list(range(5, 11))),
        ("PROGRAM IS BEGIN FOR i FROM 10 DOWNTO 5 DO WRITE i; ENDFOR END", list(reversed(range(5, 11)))),
    ],
)
def test_for_loop(input_code, expected_output_numbers):
    _run_assembler_generator_test(input_code, expected_output_numbers)


@pytest.mark.parametrize(
    "input_code, expected_output_numbers, user_input",
    [
        (
            "PROCEDURE write_proc(proc_x) IS BEGIN WRITE proc_x; END PROGRAM IS main_x BEGIN main_x := 123; write_proc(main_x); END",
            [123],
            "",
        ),
        (
            "PROCEDURE read_proc(proc_x) IS BEGIN READ proc_x; END PROGRAM IS main_x BEGIN read_proc(main_x); WRITE main_x; END",
            [124],
            "124\n",
        ),
        (
            "PROCEDURE add(x, y, z) IS BEGIN z := x + y; END PROGRAM IS x, y, z BEGIN x := 15; y := 17; add(x, y, z); WRITE x; WRITE y; WRITE z; END",
            [15, 17, 32],
            "",
        ),
        (
            "PROCEDURE add(T x, T y, T z) IS BEGIN z[3] := x[1] + y[2]; END PROGRAM IS x[-5:5], y[0:5], z[-10:10] BEGIN x[1] := 15; y[2] := 17; add(x, y, z); WRITE x[1]; WRITE y[2]; WRITE z[3]; END",
            [15, 17, 32],
            "",
        ),
        (
            "PROCEDURE add(T x, T y, T z) IS a, b, c BEGIN a := 3; b := 1; c := 2; z[a] := x[b] + y[c]; END PROGRAM IS x[-5:5], y[0:5], z[-10:10] BEGIN x[1] := 15; y[2] := 17; add(x, y, z); WRITE x[1]; WRITE y[2]; WRITE z[3]; END",
            [15, 17, 32],
            "",
        ),
        (
            "PROCEDURE add(T x, T y, T z, a, b, c) IS BEGIN z[a] := x[b] + y[c]; END PROGRAM IS x[-5:5], y[0:5], z[-10:10], a, b, c BEGIN x[1] := 15; y[2] := 17; a := 3; b := 1; c := 2; add(x, y, z, a, b, c); WRITE x[1]; WRITE y[2]; WRITE z[3]; END",
            [15, 17, 32],
            "",
        ),
        (
            "PROCEDURE modify(T t) IS BEGIN t[10] := 125; END PROCEDURE modify_in_another_proc(T t) IS BEGIN modify(t); END PROGRAM IS t[-10:10] BEGIN modify_in_another_proc(t); WRITE t[10]; END",
            [125],
            "",
        ),
        (
            "PROCEDURE modify(T t, n) IS BEGIN t[n] := 126; END PROCEDURE modify_in_another_proc(T t, n) IS BEGIN modify(t, n); END PROGRAM IS t[-10:10], n BEGIN n := 10; modify_in_another_proc(t, n); WRITE t[10]; END",
            [126],
            "",
        ),
        (
            "PROCEDURE proc(T t) IS x BEGIN x := t[10]; WRITE x; END PROGRAM IS t[-10:10] BEGIN t[10] := 123; proc(t); END",
            [123],
            "",
        ),
        (
            "PROCEDURE proc(T t, n) IS x BEGIN x := t[n]; WRITE x; END PROGRAM IS t[-10:10], n BEGIN n := 10; t[10] := 123; proc(t, n); END",
            [123],
            "",
        ),
        (
            "PROCEDURE proc(T t) IS x BEGIN x := t[10] + t[11]; WRITE x; END PROGRAM IS t[-10:15] BEGIN t[10] := 123; t[11] := 2; proc(t); END",
            [125],
            "",
        ),
    ],
)
def test_procedures(input_code, expected_output_numbers, user_input):
    _run_assembler_generator_test(input_code, expected_output_numbers, user_input)
