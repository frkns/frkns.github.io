#!/usr/bin/env bash
# Build + serve the site locally with Jekyll, then open it in Firefox on Windows (WSL).
#   ./run.sh            # serve on :4000 and open Windows Firefox
#   PORT=8080 ./run.sh  # use a different port
set -euo pipefail
cd "$(dirname "$0")"

PORT="${PORT:-4000}"
URL="http://localhost:${PORT}"

# Make a user-installed jekyll (gem install --user-install jekyll) discoverable.
if command -v ruby >/dev/null 2>&1; then
  export PATH="$(ruby -e 'print Gem.user_dir' 2>/dev/null)/bin:${PATH}"
fi

# Prefer bundler (matches GitHub Pages via the Gemfile); fall back to a direct jekyll.
if command -v bundle >/dev/null 2>&1 && bundle show jekyll >/dev/null 2>&1; then
  JEKYLL=(bundle exec jekyll)
elif command -v jekyll >/dev/null 2>&1; then
  JEKYLL=(jekyll)
else
  cat >&2 <<'MSG'
Jekyll isn't installed yet. One-time setup on Ubuntu/WSL:

  sudo apt update && sudo apt install -y ruby-dev build-essential
  gem install --user-install bundler jekyll
  bundle install        # installs the repo's Gemfile (matches GitHub Pages)

Then re-run ./run.sh
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
  # Fallbacks: let Windows resolve Firefox by name.
  if command -v powershell.exe >/dev/null 2>&1; then
    powershell.exe -NoProfile -Command "Start-Process firefox '${URL}'" >/dev/null 2>&1 && return
  fi
  cmd.exe /c start firefox "${URL}" >/dev/null 2>&1 || true
}

open_firefox &

echo "Serving on ${URL}  (Ctrl-C to stop)"
# --host 0.0.0.0 so Windows can reach it; serve auto-rebuilds on save.
exec "${JEKYLL[@]}" serve --host 0.0.0.0 --port "${PORT}" --livereload
