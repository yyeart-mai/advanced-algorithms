from pathlib import Path
import os
import random
import re
import struct
import subprocess
import sys
import tempfile


SIGNATURE = b"PATRICIA"
MAX_VALUE = (1 << 64) - 1
SEED = 20261120
OPERATION_COUNT = 20_000


def check(program, directory, cases):
    commands, expected = zip(*cases)
    result = subprocess.run(
        [str(program)], cwd=directory, input="\n".join(commands) + "\n",
        stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, timeout=60,
    )
    assert result.returncode == 0, (result.returncode, result.stderr)
    assert not result.stderr, result.stderr
    assert result.stdout.endswith("\n"), result.stdout
    actual = result.stdout.splitlines()
    assert len(actual) == len(expected), (len(actual), len(expected))
    for number, (command, got, want) in enumerate(zip(commands, actual, expected), 1):
        if want == "ERROR:":
            assert re.fullmatch(r"ERROR: [A-Za-z][ -~]*", got), (number, command, got)
        else:
            assert got == want, (number, command, got, want)


def check_dictionary(program, directory):
    for first, second, second_value in (("a", "aa", MAX_VALUE), ("aa", "a", 0)):
        check(program, directory, [
            ("a", "NoSuchWord"), ("- a", "NoSuchWord"),
            ("+ a 0", "OK"), (f"+ aa {MAX_VALUE}", "OK"),
            ("+ A 9", "Exist"), ("A", "OK: 0"), ("AA", f"OK: {MAX_VALUE}"),
            (f"- {first.upper()}", "OK"), (first, "NoSuchWord"),
            (second, f"OK: {second_value}"),
            (f"+ {first} {second_value}", "OK"), (first, f"OK: {second_value}"),
            (f"- {second}", "OK"), (first, f"OK: {second_value}"),
            (f"- {first}", "OK"), (f"- {first}", "NoSuchWord"),
            ("+ a 12", "OK"), ("a", "OK: 12"),
        ])

    first = "a" * 256
    second = first[:-1] + "b"
    check(program, directory, [
        (f"+ {first} 0", "OK"), (f"+ {second} {MAX_VALUE}", "OK"),
        (first, "OK: 0"), (second.upper(), f"OK: {MAX_VALUE}"),
        (f"- {first}", "OK"), (second, f"OK: {MAX_VALUE}"),
    ])


def record(key, value):
    return struct.pack("<H", len(key)) + key.encode("ascii") + struct.pack("<Q", value)


def check_files(program, directory):
    snapshot = directory / "snapshot with spaces.bin"
    key = "a" * 256
    value = 0x0102030405060708
    check(program, directory, [(f"+ {key.upper()} {value}", "OK"), (f"! Save {snapshot.name}", "OK")])
    assert snapshot.read_bytes() == SIGNATURE + record(key, value)
    check(program, directory, [
        ("+ old 17", "OK"), (f"! Load {snapshot.name}", "OK"),
        ("old", "NoSuchWord"), (key.upper(), f"OK: {value}"),
    ])

    (directory / "empty.bin").touch()
    check(program, directory, [("! Save signature.bin", "OK")])
    assert (directory / "signature.bin").read_bytes() == SIGNATURE
    for name in ("empty.bin", "missing.bin", "signature.bin"):
        check(program, directory, [
            ("+ old 17", "OK"), (f"! Load {name}", "OK"), ("old", "NoSuchWord"),
        ])

    def check_failed_load(name):
        check(program, directory, [
            ("+ old 17", "OK"), (f"! Load {name}", "ERROR:"),
            ("OLD", "OK: 17"), ("a", "NoSuchWord"),
        ])

    first = SIGNATURE + record("a", 0)
    complete = first + record("aa", MAX_VALUE)
    damaged = directory / "damaged.bin"
    for cut in range(1, len(complete)):
        damaged.write_bytes(complete[:cut])
        if cut == len(SIGNATURE) or cut == len(first):
            check(program, directory, [
                ("+ old 17", "OK"), (f"! Load {damaged.name}", "OK"), ("old", "NoSuchWord"),
                ("a", "OK: 0" if cut == len(first) else "NoSuchWord"), ("aa", "NoSuchWord"),
            ])
        else:
            check_failed_load(damaged.name)

    for payload in (
        b"WRONGHDR", SIGNATURE + struct.pack("<H", 0), SIGNATURE + struct.pack("<H", 257),
        first + record("A", MAX_VALUE), SIGNATURE + record("a9", 0),
    ):
        damaged.write_bytes(payload)
        check_failed_load(damaged.name)

    check_failed_load(f"{snapshot.name}/child")
    inaccessible = directory / "inaccessible.bin"
    inaccessible.write_bytes(complete)
    inaccessible.chmod(0)
    try:
        if not os.access(inaccessible, os.R_OK):
            check_failed_load(inaccessible.name)
    finally:
        inaccessible.chmod(0o600)

    check(program, directory, [
        ("+ old 17", "OK"), ("! Save missing-directory/snapshot.bin", "ERROR:"), ("old", "OK: 17"),
    ])


def check_random_stream(program, directory):
    rng = random.Random(SEED)
    keys = ["a", "aa"] + ["".join(rng.choices("abcd", k=rng.randint(1, 24))) for _ in range(160)]
    reference = {}
    saved = {}
    cases = []
    for _ in range(OPERATION_COUNT):
        operation = rng.randrange(20)
        key = rng.choice(keys)
        typed_key = key.upper() if rng.randrange(2) else key
        filename = f"random-{rng.randrange(5)}.bin"
        if operation < 7:
            value = rng.randrange(MAX_VALUE + 1)
            cases.append((f"+ {typed_key} {value}", "Exist" if key in reference else "OK"))
            reference.setdefault(key, value)
        elif operation < 11:
            cases.append((f"- {typed_key}", "OK" if key in reference else "NoSuchWord"))
            reference.pop(key, None)
        elif operation < 16:
            cases.append((typed_key, f"OK: {reference[key]}" if key in reference else "NoSuchWord"))
        elif operation < 18:
            cases.append((f"! Save {filename}", "OK"))
            saved[filename] = reference.copy()
        else:
            cases.append((f"! Load {filename}", "OK"))
            reference = saved.get(filename, {}).copy()

    check(program, directory, cases)


if __name__ == "__main__":
    program = Path(sys.argv[1]).resolve()
    with tempfile.TemporaryDirectory() as scratch:
        directory = Path(scratch)
        check_dictionary(program, directory)
        check_files(program, directory)
        check_random_stream(program, directory)
    print(f"{program}: CLI checks passed, {OPERATION_COUNT} mixed commands, seed={SEED}")
