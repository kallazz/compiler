import re
import os
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

    assert vm_result.returncode == 0

    print(f"The stdout is {vm_result.stdout}")
    output_numbers = re.findall(r">\s*(-?\d+)", vm_result.stdout, re.MULTILINE)
    output_numbers = list(map(int, output_numbers))

    assert expected_output_numbers == output_numbers


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
