#!/usr/bin/env bash
# Build + serve the site locally with Jekyll, then open it in Firefox on Windows (WSL).
# Auto-installs everything it needs (git hook, bundler, gems) on first run.
#   ./run.sh            # serve on :4000 and open Windows Firefox
#   PORT=8080 ./run.sh  # use a different port
set -euo pipefail
cd "$(dirname "$0")"

PORT="${PORT:-4000}"
URL="http://localhost:${PORT}"

# Make a user-installed gem bin dir (gem install --user-install ...) discoverable.
if command -v ruby >/dev/null 2>&1; then
  export PATH="$(ruby -e 'print Gem.user_dir' 2>/dev/null)/bin:${PATH}"
fi

# --- auto-install: git hook that stamps last-edited dates into posts ---
if [ -d .git ] && [ "$(git config core.hooksPath || true)" != ".githooks" ]; then
  git config core.hooksPath .githooks
  echo "Installed git hook (auto-stamps last-edited dates)."
fi

# --- auto-install: bundler + project gems ---
if ! command -v bundle >/dev/null 2>&1; then
  echo "Installing bundler..."; gem install --user-install bundler >/dev/null 2>&1 || true
fi
if command -v bundle >/dev/null 2>&1; then
  bundle config set --local path vendor/bundle >/dev/null 2>&1 || true
  bundle check >/dev/null 2>&1 || { echo "Installing gems..."; bundle install; }
fi

# --- choose how to run jekyll ---
if command -v bundle >/dev/null 2>&1 && bundle show jekyll >/dev/null 2>&1; then
  JEKYLL=(bundle exec jekyll)
elif command -v jekyll >/dev/null 2>&1; then
  JEKYLL=(jekyll)
else
  cat >&2 <<'MSG'
Couldn't set up Jekyll automatically. Your Ruby is likely missing dev headers.
One-time fix on Ubuntu/WSL, then re-run ./run.sh:

  sudo apt update && sudo apt install -y ruby-dev build-essential
MSG
  exit 1
fi

# Once the server is accepting connections, open it in Windows Firefox.
open_firefox() {
  for _ in $(seq 1 120); do
    if (echo > "/dev/tcp/127.0.0.1/${PORT}") 2>/dev/null; then break; fi
    sleep 0.5
  done
  local ff
  for ff in \
    "/mnt/c/Program Files/Mozilla Firefox/firefox.exe" \
    "/mnt/c/Program Files (x86)/Mozilla Firefox/firefox.exe"; do
    if [ -x "$ff" ]; then "$ff" "$URL" >/dev/null 2>&1 & return; fi
  done
  if command -v powershell.exe >/dev/null 2>&1; then
    powershell.exe -NoProfile -Command "Start-Process firefox '${URL}'" >/dev/null 2>&1 && return
  fi
  cmd.exe /c start firefox "${URL}" >/dev/null 2>&1 || true
}

open_firefox &

echo "Serving on ${URL}  (Ctrl-C to stop)"
exec "${JEKYLL[@]}" serve --host 0.0.0.0 --port "${PORT}" --livereload
