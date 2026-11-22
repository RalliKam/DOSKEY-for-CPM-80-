# DOSKEY-for-CPM-80-
DOSKEY adds resident command history and line editing to CP/M 3 on the Z80. Recall and edit your last eight commands in just 1.7 KB of RAM.

# DOSKEY

DOSKEY adds resident command history to **CP/M Plus (CP/M 3.0) on the Z80**. Press ↑ at the prompt to recall a command, edit it, and press Return to run it again. The ring buffer holds the last eight commands of up to 80 characters each. The extension uses **1,734 bytes** including its buffers and stack, which fits in seven 256-byte pages.

## Quick start

1. Transfer [DOSKEY.COM](./DOSKEY.COM) to a CP/M 3 drive in binary mode.
2. Type `DOSKEY` at the CP/M prompt. Command history is active as soon as the prompt returns.
3. Run `DIR *.COM`. Press ↑ or **Ctrl+P** to recall it, press Backspace three times to remove `COM`, type `PAS`, and press Return. The Console Command Processor (CCP) now runs `DIR *.PAS`.

The history survives program changes and warm boots. A cold boot removes the extension and clears the history.

## Keyboard controls

| Key | Action |
| --- | --- |
| ↑ / ↓ or Ctrl+P / Ctrl+N | Recall an older or newer command, respectively |
| ← / → | Move the cursor one character left or right |
| Printable ASCII character | Insert the character at the cursor |
| Backspace or Rubout (`7Fh`) | Delete the character to the left of the cursor |
| Delete (`ESC [ 3 ~`) | Delete the character under the cursor |
| Return | Run the edited command |

DOSKEY recognizes arrow keys in both VT100 cursor modes (`ESC [` and `ESC O`). If you browse the history while typing a command, press ↓ past the newest entry to return to your draft. DOSKEY restores the cursor to where you left it.

Empty lines and consecutive duplicates take up no history slots. Once all eight slots are full, the next entry replaces the oldest. You can edit and submit lines of 81 to 255 characters in full if the calling program provides a large enough buffer. DOSKEY stores lines of up to 80 characters in the history.

## Build from source

You need Borland **Turbo Pascal 3.0 for CP/M-80** and the CP/M Plus utility `GENCOM.COM`.

1. Copy [MKDOSKEY.PAS](./MKDOSKEY.PAS) and [DOSKEY.INC](./DOSKEY.INC) to the same drive as ASCII text with CP/M line endings (CR/LF). Make that drive current.
2. Start `TURBO`. In the main menu, press **M** and select `MKDOSKEY.PAS` as the main file.
3. Press **O** for Compiler Options, then **C** to select *Com-file*. Keep the default start address. Press **E** and enter `8000` (hex) as the end address. Press **Q** to return to the main menu, **C** to compile, and **Q** to exit Turbo Pascal.
4. At the CP/M prompt, run the following commands in order. Include the square brackets in the GENCOM command.

```text
MKDOSKEY
GENCOM DOSKEY [NULL]
DOSKEY
```

`MKDOSKEY` generates the relocatable module [DOSKEY.RSX](./DOSKEY.RSX). `GENCOM` packages the module into the executable `DOSKEY.COM`. To install DOSKEY on another CP/M 3 machine, transfer just the COM file.

## How it works

The CP/M loader installs DOSKEY as a *Resident System Extension* (RSX). DOSKEY handles buffered console input through BDOS function 10 (`0Ah`), so programs that call this function interactively can also use the history. It forwards all other BDOS calls through the `NEXT` vector. Only the module generator uses the Pascal runtime.

The build was verified with Borland Turbo Pascal 3.01A for CP/M-80. Installation and command recall were tested at the CCP prompt under banked CP/M 3.0 in the AltairZ80 simulator. The [CP/M text guide (in German)](./DOSKEY.TXT) gives the memory breakdown and explains how DOSKEY handles prefilled input buffers.
