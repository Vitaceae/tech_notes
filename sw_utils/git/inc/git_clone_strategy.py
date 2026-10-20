#!/usr/bin/env python

from pathlib import Path
import os
import random
import subprocess

proj_root = Path("git_clone_strategy").resolve()

if proj_root.exists():
    raise SystemExit(f"Error: '{proj_root}' exists.")

proj_root.mkdir(parents=True)
src = proj_root / "src"

# Working directory: <proj>/src
def cmd_git(*args):
    subprocess.run(["git", *args], cwd=src, check=True)

# Initialize a git repo in <proj>/src with a default branch 'main'
subprocess.run(["git", "init", "-b", "main", str(src)], check=True)
cmd_git("config", "user.name", "foo")
cmd_git("config", "user.email", "foo@bar.com")

for d in ("apps/web", "packages/shared", "assets"):
    (src / d).mkdir(parents=True, exist_ok=True)

(src / "apps/web/check.py").write_text(
    'from packages.shared.price import price\n'
    'assert price(2) == 4\nprint("PASS")\n'
)
(src / "packages/shared/price.py").write_text(
    'def price(x):\n    return x + 1\n'
)

def cmd_commit(message, day):
    cmd_git("add", ".")
    timestamp = f"2026-10-{day:02d}T00:00:00+0000"
    env = dict(os.environ, GIT_AUTHOR_DATE=timestamp, GIT_COMMITTER_DATE=timestamp)
    subprocess.run(["git", "commit", "-m", message], cwd=src, env=env, check=True)

# Simulate asset history with incompressible data and multiple commits.
for i in range(5):
    # Generate a 256KiB random binary asset using a fixed seed.
    (src / "assets/foo.bin").write_bytes(
        random.Random(i).randbytes(262144)
    )
    (src / "README.md").write_text(
        f"Git clone strategy test {i}\n"
    )
    cmd_commit(f"test-{i}", i + 1)
    
    # Create an additional feature branch
    if i == 1:
        cmd_git("branch", "feature")

# Create a side branch and then switch back to the main branch
cmd_git("switch", "feature")
(src / "feature.txt").write_text("side branch\n")
cmd_commit("feature", 6)
cmd_git("switch", "main")

# Set up the remote bare repo as the remote resource.
origin = proj_root / "origin.git"
subprocess.run(["git", "clone", "--bare", str(src), str(origin)], check=True)

# Configure the uploadpack parameters to enable filtering and SHA-1 queries.
for key in ("uploadpack.allowFilter", "uploadpack.allowAnySHA1InWant"):
    subprocess.run(["git", "-C", str(origin), "config", key, "true"], check=True)

# Intentionally use the file:// protocol to avoid hardlink optimizations that could affect the  
# benchmark results.
print("remote=" + origin.as_uri())
cmd_git("rev-parse", "HEAD")