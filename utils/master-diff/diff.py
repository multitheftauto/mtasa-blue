#!/usr/bin/env python3
import subprocess
import sys
from datetime import datetime
from pathlib import Path

MASTER = 'master'
FORK = 'release/1.6.0'

REPO_ROOT = Path(__file__).resolve().parents[2]
SCRIPT_DIR = Path(__file__).resolve().parent
OUTPUT_DIR = SCRIPT_DIR / 'output'
EXCLUDED_FILE = SCRIPT_DIR / 'excluded-from-fork.yaml'


def run_git(args):
    result = subprocess.run(
        ['git'] + args,
        cwd=REPO_ROOT,
        capture_output=True,
        text=True,
    )
    if result.returncode != 0:
        print(f"git error: {result.stderr}", file=sys.stderr)
        sys.exit(1)
    return result.stdout


def parse_cherry(output):
    """Parse git cherry -v output into list of (sign, sha, subject)."""
    entries = []
    for line in output.splitlines():
        if not line.strip():
            continue
        sign = line[0]
        rest = line[2:].split(' ', 1)
        sha = rest[0]
        subject = rest[1] if len(rest) > 1 else ''
        entries.append((sign, sha, subject))
    return entries


def get_patch_id_map(commit_hashes):
    """Returns {patch_id: commit_sha} for given list of commit hashes."""
    if not commit_hashes:
        return {}

    hashes_input = ('\n'.join(commit_hashes) + '\n').encode()

    # Keep diff as bytes — repo may contain non-UTF-8 binary content
    diff_result = subprocess.run(
        ['git', 'diff-tree', '-p', '--stdin'],
        input=hashes_input,
        capture_output=True,
        cwd=REPO_ROOT,
    )

    patch_result = subprocess.run(
        ['git', 'patch-id'],
        input=diff_result.stdout,
        capture_output=True,
        cwd=REPO_ROOT,
    )

    result = {}
    for line in patch_result.stdout.decode('utf-8', errors='replace').splitlines():
        if line.strip():
            parts = line.split()
            if len(parts) == 2:
                patch_id, sha = parts
                result[patch_id] = sha
    return result


def load_exclusions(path):
    """Parse excluded-from-fork.yaml. Returns (set of shas, set of titles)."""
    excluded_shas = set()
    excluded_titles = set()
    if not path.exists():
        return excluded_shas, excluded_titles
    for line in path.read_text().splitlines():
        line = line.strip()
        if line.startswith('- commit:'):
            sha = line[len('- commit:'):].strip()
            if sha:
                excluded_shas.add(sha)
        elif line.startswith('- title:'):
            title = line[len('- title:'):].strip()
            if title:
                excluded_titles.add(title)
    return excluded_shas, excluded_titles


def main():
    OUTPUT_DIR.mkdir(parents=True, exist_ok=True)

    print(f"Running: git cherry -v {FORK} {MASTER} ...")
    master_cherry = parse_cherry(run_git(['cherry', '-v', FORK, MASTER]))
    master_only = [(sha, subj) for sign, sha, subj in master_cherry if sign == '+']
    picked_from_master = [(sha, subj) for sign, sha, subj in master_cherry if sign == '-']

    print(f"Running: git cherry -v {MASTER} {FORK} ...")
    fork_cherry = parse_cherry(run_git(['cherry', '-v', MASTER, FORK]))
    fork_only = [(sha, subj) for sign, sha, subj in fork_cherry if sign == '+']

    # Match cherry-picked master commits to their fork SHA via patch-id
    print("Computing patch-ids for cherry-picked commit matching ...")

    picked_master_shas = [sha for sha, _ in picked_from_master]
    master_patch_ids = get_patch_id_map(picked_master_shas)  # {patch_id: master_sha}

    fork_commit_shas = run_git(['log', '--format=%H', FORK, f'^{MASTER}']).split()
    fork_patch_ids = get_patch_id_map(fork_commit_shas)  # {patch_id: fork_sha}

    # Build master_sha -> fork_sha lookup
    fork_by_patch = {pid: fsha for pid, fsha in fork_patch_ids.items()}
    master_to_fork = {}
    for pid, msha in master_patch_ids.items():
        if pid in fork_by_patch:
            master_to_fork[msha] = fork_by_patch[pid]

    # Separate picked commits into matched and unmatched
    both_entries = []    # (master_sha, fork_sha, subject)
    unmatched = []       # (master_sha, subject) — picked but no patch-id match found
    for sha, subj in picked_from_master:
        fork_sha = master_to_fork.get(sha)
        if fork_sha:
            both_entries.append((sha, fork_sha, subj))
        else:
            unmatched.append((sha, subj))

    # Write summary.txt
    now = datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    sep = '=' * 72
    lines = [
        f"Generated:    {now}",
        f"Master:       {MASTER}",
        f"Fork:         {FORK}",
        "",
        "Counts:",
        f"  Master-only (not cherry-picked to fork):  {len(master_only)}",
        f"  Cherry-picked to both:                    {len(both_entries)}",
        f"  Unmatched picked (no patch-id match):     {len(unmatched)}",
        f"  Fork-only:                                {len(fork_only)}",
        "",
        sep,
        f"MASTER-ONLY — not yet cherry-picked to fork ({len(master_only)} commits)",
        sep,
    ] + [f"  + {sha}  {subj}" for sha, subj in master_only] + [
        "",
        sep,
        f"CHERRY-PICKED TO BOTH ({len(both_entries)} commits)",
        sep,
    ] + [f"  master: {msha}  fork: {fsha}  {subj}" for msha, fsha, subj in both_entries]

    if unmatched:
        lines += [
            "",
            sep,
            f"UNMATCHED PICKED — no patch-id match found ({len(unmatched)} commits)",
            sep,
        ] + [f"  - {sha}  {subj}" for sha, subj in unmatched]

    lines += [
        "",
        sep,
        f"FORK-ONLY ({len(fork_only)} commits)",
        sep,
    ] + [f"  + {sha}  {subj}" for sha, subj in fork_only]

    summary_path = OUTPUT_DIR / 'summary.txt'
    summary_path.write_text('\n'.join(lines) + '\n')
    print(f"Wrote {summary_path}")

    # Write changes-master.yaml
    master_yaml = '\n'.join(f'- {sha}  # {subj}' for sha, subj in master_only) + '\n'
    master_yaml_path = OUTPUT_DIR / 'changes-master.yaml'
    master_yaml_path.write_text(master_yaml)
    print(f"Wrote {master_yaml_path}")

    # Write changes-fork.yaml
    fork_yaml = '\n'.join(f'- {sha}  # {subj}' for sha, subj in fork_only) + '\n'
    fork_yaml_path = OUTPUT_DIR / 'changes-fork.yaml'
    fork_yaml_path.write_text(fork_yaml)
    print(f"Wrote {fork_yaml_path}")

    # Write changes-both.yaml
    both_yaml_lines = []
    for msha, fsha, subj in both_entries:
        both_yaml_lines.append(f'- master: {msha}')
        both_yaml_lines.append(f'  fork: {fsha}  # {subj}')
    both_yaml_path = OUTPUT_DIR / 'changes-both.yaml'
    both_yaml_path.write_text('\n'.join(both_yaml_lines) + '\n')
    print(f"Wrote {both_yaml_path}")

    print(f"\nDone. Output in {OUTPUT_DIR}")


if __name__ == '__main__':
    main()
