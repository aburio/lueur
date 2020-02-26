import subprocess

revision = subprocess.check_output(["git", "rev-parse", "--short", "HEAD"]).strip()
print("-DBUILD_COMMIT=\\\"%s\\\"" % revision)