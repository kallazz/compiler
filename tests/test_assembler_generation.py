import re
import os
import subprocess

import pytest

from .config import INPUT_FILENAME, MAKEFILE_PATH, OUTPUT_FILENAME


def _run_assembler_generator_test(input_code: str, expected_output_assembler_code: str) -> None:
    with open(INPUT_FILENAME, "w") as input_file:
        input_file.write(input_code)

    result = subprocess.run(
        [os.path.join(MAKEFILE_PATH, "compiler"), INPUT_FILENAME, OUTPUT_FILENAME], capture_output=True, text=True
    )

    assert result.returncode == 0

    with open(OUTPUT_FILENAME, "r") as output_file:
        output_assembler_code = output_file.read()

    output_assembler_code = re.sub(r".#.*", "", output_assembler_code)

    assert expected_output_assembler_code in output_assembler_code


@pytest.mark.parametrize(
    "input_code, expected_output_assembler_code",
    [
        ("PROGRAM IS x BEGIN x := 123; END", "SET 123\nSTORE 4\n"),
        ("PROGRAM IS x[-10:-5] BEGIN x[-7] := 123; END", "SET 123\nSTORE 7\n"),
        ("PROGRAM IS x[5:10] BEGIN x[7] := 123; END", "SET 123\nSTORE 6\n"),
        ("PROGRAM IS x[-5:5] BEGIN x[0] := 123; END", "SET 123\nSTORE 9\n"),
        (
            "PROGRAM IS n, x[5:10] BEGIN n := 7; x[n] := 123; END",
            "SET 7\nSTORE 4\nSET 0\nADD 4\nSTORE 3\nSET 123\nSTOREI 3\n",
        ),
        (
            "PROGRAM IS x[5:10], n BEGIN n := 7; x[n] := 123; END",
            "SET 7\nSTORE 10\nSET -1\nADD 10\nSTORE 3\nSET 123\nSTOREI 3\n",
        ),
    ],
)
def test_assignment(input_code, expected_output_assembler_code):
    _run_assembler_generator_test(input_code, expected_output_assembler_code)


@pytest.mark.parametrize(
    "input_code, expected_output_assembler_code",
    [
        ("PROGRAM IS x BEGIN x := 15 + 17; END", "SET 15\nSTORE 1\nSET 17\nADD 1\nSTORE 4\n"),
        ("PROGRAM IS x BEGIN x := 15 - 17; END", "SET 15\nSTORE 1\nSET -17\nADD 1\nSTORE 4\n"),
        ("PROGRAM IS x, y, z BEGIN y := 3; z := 4; x := y + z; END", "LOAD 5\nADD 6\nSTORE 4\n"),
        ("PROGRAM IS x, y, z BEGIN y := 3; z := 4; x := y - z; END", "LOAD 5\nSUB 6\nSTORE 4\n"),
        (
            "PROGRAM IS x, y[1:10], z[1:10], n BEGIN n := 5; y[n] := 3; z[n] := 4; x := y[n] + z[n]; END",
            "LOADI 3\nADDI 2\nSTORE 4\n",
        ),
        ("PROGRAM IS x, y BEGIN y := 15; x := y + 17; END", "SET 17\nADD 5\nSTORE 4\n"),
        ("PROGRAM IS x, y BEGIN y := 17; x := 15 + y; END", "SET 15\nADD 5\nSTORE 4\n"),
        ("PROGRAM IS x, y BEGIN y := 15; x := y - 17; END", "SET -17\nADD 5\nSTORE 4\n"),
        ("PROGRAM IS x, y BEGIN y := 17; x := 15 - y; END", "SET 15\nSUB 5\nSTORE 4\n"),
        ("PROGRAM IS x, y[5:10], n BEGIN n := 5; y[n] := 17; x := 15 + y[n]; END", "SET 15\nADDI 2\nSTORE 4\n"),
    ],
)
def test_assignment_with_expression(input_code, expected_output_assembler_code):
    _run_assembler_generator_test(input_code, expected_output_assembler_code)


@pytest.mark.parametrize(
    "input_code, expected_output_assembler_code",
    [
        ("PROGRAM IS x BEGIN READ x; END", "GET 4\n"),
        ("PROGRAM IS x[-5:5] BEGIN READ x[-2]; END", "GET 7\n"),
        ("PROGRAM IS x[-5:5] BEGIN READ x[2]; END", "GET 11\n"),
        (
            "PROGRAM IS x[-5:5], n BEGIN n := -5; READ x[n]; END",
            "SET -5\nSTORE 15\nSET 9\nADD 15\nSTORE 3\nGET 0\nSTOREI 3\n",
        ),
        (
            "PROGRAM IS n, x[-5:5] BEGIN n := 5; READ x[n]; END",
            "SET 5\nSTORE 4\nSET 10\nADD 4\nSTORE 3\nGET 0\nSTOREI 3\n",
        ),
    ],
)
def test_read(input_code, expected_output_assembler_code):
    _run_assembler_generator_test(input_code, expected_output_assembler_code)


@pytest.mark.parametrize(
    "input_code, expected_output_assembler_code",
    [
        ("PROGRAM IS BEGIN WRITE 123; END", "SET 123\nPUT 0\n"),
        ("PROGRAM IS x[-5:5] BEGIN WRITE x[2]; END", "PUT 11\n"),
        ("PROGRAM IS n, x[-5:5] BEGIN n := 3; WRITE x[n]; END", "LOADI 0\nPUT 0\n"),
    ],
)
def test_write(input_code, expected_output_assembler_code):
    _run_assembler_generator_test(input_code, expected_output_assembler_code)


@pytest.mark.parametrize(
    "input_code, expected_output_assembler_code",
    [
        ("PROGRAM IS BEGIN IF 15 = 17 THEN WRITE 1; ENDIF END", "SET 17\nSTORE 1\nSET -15\nADD 1\nJZERO 3\nSET 1\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 = 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", "SET 17\nSTORE 1\nSET -15\nADD 1\nJZERO 4\nSET 1\nPUT 0\nJUMP 3\nSET 0\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 != 17 THEN WRITE 1; ENDIF END", "SET 15\nSTORE 1\nSET -17\nADD 1\nJZERO 3\nSET 1\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 != 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", "SET 15\nSTORE 1\nSET -17\nADD 1\nJZERO 4\nSET 1\nPUT 0\nJUMP 3\nSET 0\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 > 17 THEN WRITE 1; ENDIF END", "SET 17\nSTORE 1\nSET -15\nADD 1\nJPOS 3\nSET 1\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 > 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", "SET 17\nSTORE 1\nSET -15\nADD 1\nJPOS 4\nSET 1\nPUT 0\nJUMP 3\nSET 0\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 < 17 THEN WRITE 1; ENDIF END", "SET 17\nSTORE 1\nSET -15\nADD 1\nJNEG 3\nSET 1\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 < 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", "SET 17\nSTORE 1\nSET -15\nADD 1\nJNEG 4\nSET 1\nPUT 0\nJUMP 3\nSET 0\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 >= 17 THEN WRITE 1; ENDIF END", "SET 15\nSTORE 1\nSET -17\nADD 1\nJNEG 3\nSET 1\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 >= 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", "SET 15\nSTORE 1\nSET -17\nADD 1\nJNEG 4\nSET 1\nPUT 0\nJUMP 3\nSET 0\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 <= 17 THEN WRITE 1; ENDIF END", "SET 15\nSTORE 1\nSET -17\nADD 1\nJPOS 3\nSET 1\nPUT 0\n"),
        ("PROGRAM IS BEGIN IF 15 <= 17 THEN WRITE 1; ELSE WRITE 0; ENDIF END", "SET 15\nSTORE 1\nSET -17\nADD 1\nJPOS 4\nSET 1\nPUT 0\nJUMP 3\nSET 0\nPUT 0\n"),
    ],
)
def test_if(input_code, expected_output_assembler_code):
    _run_assembler_generator_test(input_code, expected_output_assembler_code)
