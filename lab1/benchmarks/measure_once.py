import os
import subprocess
import sys
from time import perf_counter_ns


def main():
    command = sys.argv[1:]
    if not command:
        raise SystemExit("Usage: measure_once.py PROGRAM [ARG ...]")

    start = perf_counter_ns()
    child = subprocess.Popen(command)
    _, status, usage = os.wait4(child.pid, 0)
    elapsed_ns = perf_counter_ns() - start

    child.returncode = os.waitstatus_to_exitcode(status)
    print(f"real_ms={elapsed_ns / 1_000_000:.3f}", file=sys.stderr)
    print(f"max_rss_bytes={usage.ru_maxrss}", file=sys.stderr)
    print(f"exit_code={child.returncode}", file=sys.stderr)
    return child.returncode if child.returncode >= 0 else 128 - child.returncode


if __name__ == "__main__":
    raise SystemExit(main())
