# generate from ChatGPT
from __future__ import annotations

import argparse
import subprocess
from dataclasses import dataclass, field
from pathlib import Path
from typing import Dict, List
from cmd_output import Cmd
from uci_engine import UciEngine

cmd = Cmd()


@dataclass
class MateCase:
    fen: str = ""

def load_mate_test(path: Path, mate_depth: int) -> List[MateCase]:
    cases: List[MateCase] = []

    with path.open("r", encoding="utf-8") as f:
        for line_number, raw in enumerate(f, start=1):
            line = raw.strip()

            # empty line
            if not line:
                continue
            
            cases.append(MateCase(fen=line))
    
    return cases

def run_test_cases(engine_path: Path, cases : List[MateCase], mate_depth: int, detail: bool, max_testcases: int) -> bool:
    print(f"Loaded {len(cases)} mate test cases")
    if max_testcases:
        print(f"Running front {max_testcases} mate test cases")
    print(f"Testing mate in {mate_depth}")
    print()

    pass_count = 0
    test_depth = mate_depth * 2

    with UciEngine(engine_path) as engine:
        for index, case in enumerate(cases, start = 1):
            if (detail):
                print(f"[{index}] {case.fen}")

            engine.set_position(case.fen)
            result = engine.go_depth(test_depth)


            if (result.mate_score == mate_depth):
                pass_count += 1
                if (detail):
                    cmd.print_pass("pass")
            else:
                if (detail):
                    cmd.print_fail("fail")
                    cmd.print_fail(f"actual mate score: {result.mate_score}")
                    cmd.print_fail(f"actual score cp: {result.cp_score}")
                    cmd.print_fail(f"actual move: {result.bestmove}")
            
            if index >= max_testcases:
                break

    if max_testcases:
        print(f"Test completed with [{pass_count} / {max_testcases}], pass rate: {pass_count / max_testcases* 100}")
    else:
        print(f"Test completed with [{pass_count} / {len(cases)}], pass rate: {pass_count / len(cases) * 100}")
    print(f"using depth {test_depth} to test M{mate_depth}")

    return True

def main() -> int:
    parser = argparse.ArgumentParser(description="Run perft tests against engine CLI")
    parser.add_argument(
        "--engine",
        required=True,
        help="Path to engine executable, e.g. build/Hynobius.exe",
    )
    parser.add_argument(
        "--file",
        required=True,
        help="Path to mate test file. Note that only fen is allowed.",
    )
    parser.add_argument(
        "--mate-depth",
        required=True,
        help="Mate in [int] you want to test, e.g. 3 (this meaning mate in three)",
    )
    parser.add_argument(
        "--detail",
        action="store_true",
        help="Output detailed result.",
    )
    parser.add_argument(
        "--max-testcases",
        help="Set the maximum test cases you want to test."
    )

    args = parser.parse_args()

    engine_path = Path(args.engine)
    file_path = Path(args.file)
    mate_depth = int(args.mate_depth)
    detail = bool(args.detail)

    if not engine_path.exists():
        raise FileNotFoundError(f"engine not found: {engine_path}")
    if not file_path.exists():
        raise FileNotFoundError(f"test file not found: {file_path}")
    if mate_depth <= 0:
        raise ValueError(f"mate depth should be greater than one: {mate_depth}")
    
    cases = load_mate_test(file_path, mate_depth)

    if not cases:
        raise ValueError(f"no mate test cases found: {file_path}")
    
    if (args.max_testcases):
        max_testcases = int(args.max_testcases)
    else:
        max_testcases = len(cases)

    ok = run_test_cases(engine_path, cases, mate_depth, detail, max_testcases)

    return 0 if ok else 1

if __name__ == "__main__":
    raise SystemExit(main())