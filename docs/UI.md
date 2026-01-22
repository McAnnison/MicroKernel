# UI plan (meets assignment requirement)

The assignment requires a UI (GUI, CLI, or web-based) appropriate for the project.
For an OS kernel prototype, the fastest and most reliable UI is an **interactive CLI**.

## Current UI (implemented)

### Serial CLI (interactive)
- Runs over COM1 and works with QEMU `-serial stdio`.
- Gives an interactive prompt (`mk>`) so the team can demonstrate features quickly.

Commands (MVP)
- `help` — list commands
- `clear` — clear VGA text screen
- `echo <text>` — print text
- `about` — short info
- `halt` — halt CPU

Why serial?
- No keyboard driver required.
- Works immediately in QEMU and is easy to record for demos.

## Next UI iterations (optional)

### VGA “dashboard” (non-interactive)
- Reserve top few lines for system status (services, IPC stats).
- Keep scroll area for logs.

### Keyboard input
- Add PS/2 keyboard driver and move CLI input to VGA.

### Graphical UI
- VESA linear framebuffer (or Bochs/QEMU framebuffer) + bitmap font.
- Out of scope for a 7-day prototype unless the rest is already stable.

## Demo guidance
Run `make run` and type commands into the QEMU serial console.
Capture the serial output for your submission screenshots/video.
