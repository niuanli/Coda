# Coda

**Give your console a little soul.** A native Xbox 360 sound manager and two-sound image workshop.

[Русский](README.ru.md) · [Third-party credits](THIRD_PARTY.md) · [Licensing review](LICENSING.md)

Author: **Aleksandr Nikitin aka Niuanli**. Current version: **0.4.11**.

## Features

- Studio interface in English and Russian, with saved language selection.
- BIN library with paging, folder refresh and file validation.
- Backup before installation, backup verification and full read-back verification after writing.
- Workshop: two WAV clips become a 45,056-byte ISD BIN image without modifying the originals.
- WAV and supported ISD PCM BIN preview through TV / HDMI, separate from sound-chip playback.
- About, author credits and third-party notices.

## Use

Copy `Coda.xex` and the complete `Media` folder together. Use a homebrew-capable Xbox 360 with a supported ISD sound chip; hardware operations require the necessary kernel access. Keep your backups and do not power off during installation.

```text
Coda.xex
Media/
Sounds/     BIN images to install
Dumps/      verified sound backups
Imports/    source WAV files
```

Keep `Sounds`, `Dumps`, `Imports` and `coda-language.txt` when updating.

| Screen | Controls |
|---|---|
| Home | A library; X backup; Y installed chip sounds; LB restore; Start workshop; RB language; L3 About; Back exit |
| Library | D-pad / left stick select; A review; X refresh; Y file preview; B back |
| WAV picker | A select; Y play / stop; B back |
| BIN preview | A power clip; X eject clip; Y stop; B back |
| About | X credits / overview; B back |

### Workshop format

Inputs: **WAV PCM, mono, 8000 Hz, 16 bit**, at most 1 MiB per file. Encoded clips must fit in 44,800 bytes combined, approximately 5.6 seconds including framing overhead. MP3, resampling and editing are not implemented. BIN preview supports ISD PCM framing `0x78`; unsupported encodings are rejected.

### Xenia

Xenia can test the interface, file operations and HDMI preview; it cannot verify the physical ISD protocol. Set `allow_game_relative_writes = true` in `[General]` of `xenia-canary.config.toml`, then restart the emulator, to allow creating BIN files beside the XEX. Files added from Windows during emulation may require restarting the title because of Xenia's filesystem cache.

## Build

Windows PowerShell and a separately supplied, lawfully obtained Xbox 360 XDK toolchain are required. XDK, firmware dumps and third-party sounds are not included.

```powershell
.\build.ps1 -SdkPath 'D:\path\to\XDK'
```

Build outputs go to the sibling `Coda-artifacts` directory, outside the source repository. Override with `-ArtifactRoot` if needed. `CODA_XDK` can supply the SDK path.

To regenerate editable XUI and locale tables, use Python 3:

```powershell
python tools/generate_scene.py
python tools/generate_locale.py
```

Artwork regeneration uses Pillow: `python tools/studio_assets.py`. Runtime PNG assets and the licensed font are intentionally kept in the source tree; compiled `.xur` themes are generated at build time.

## Tests and architecture

Run `tests\run-all.cmd` with MSVC C++ Build Tools, Python 3 and the .NET Framework C# compiler installed. Tests use synthetic audio and mocked hardware; no copyrighted sound clips are required.

- `core`: flash and image-build use cases, device/storage ports, image layout and preview decoding.
- `platform`: Xbox GPIO/filesystem adapters, background operation runner and XAudio2 playback.
- `ui`: native XUI scene, catalogue presentation and RU/EN localization.

The use cases depend on interfaces, not Xbox APIs. File I/O, audio playback and scene rendering have separate owners. The image builder is tested against the J-Runner C# reference; the preview device converts PCM to Xbox byte order at the audio boundary.

## Credits and licensing

This is an independent project, not affiliated with or endorsed by Microsoft. Use audio you own or have permission to use. Third-party credits and their available license texts are retained. **Project-wide licensing is not finalized:** see [LICENSING.md](LICENSING.md) before public distribution, including binary releases. A disclaimer does not replace permission from upstream authors or the SDK license.
