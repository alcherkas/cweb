# cweb

An interactive, terminal-style portfolio built in pure C, compiled to WebAssembly with Emscripten, and deployed to GitHub Pages.

## Quick start

```bash
# Install Emscripten first: https://emscripten.org/docs/getting_started/index.html
make
make serve
```

Open http://localhost:8000 and type `help` in the centered CLI.

## Commands

- `help` — show available commands
- `about` — about this demo
- `projects` — sample projects
- `skills` — skills snapshot
- `contact` — contact info
- `theme` — toggle dark/light theme
- `clear` — clear the screen

## Deployment

Push to `main`. GitHub Actions builds the site and deploys the `build/` directory to GitHub Pages.
