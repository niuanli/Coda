# Licensing review / Проверка лицензий

Coda author / Автор Coda: **Aleksandr Nikitin aka Niuanli**.

No project-wide license has been assigned yet. Do not interpret credits, public source availability or the About disclaimer as a redistribution grant. This document records unresolved provenance; it is not a legal opinion or a claim that publication has been cleared.

Общая лицензия пока не назначена. Указание авторов, доступность исходников и дисклеймер About не заменяют разрешение на распространение. Этот документ фиксирует происхождение компонентов и нерешённые вопросы; он не подтверждает правомерность публикации.

| Component | Evidence | Required follow-up |
|---|---|---|
| J-Runner image template / C# reference | MIT text retained in `research/JRunner.LICENSE` | Preserve MIT notice in source and binary packages |
| Liberation Sans | SIL OFL 1.1 in `assets/FONT-LICENSE.md` | Preserve font license |
| Source Sans artwork provenance | SIL OFL 1.1 in `assets/SOURCE-SANS-LICENSE.md` | Preserve license and attribution |
| SonusGUI / ISDFlasher transport provenance | Existing vendor comments credit kmx360 and buddyjojo; SonusGUI repository has no standalone license file | Confirm redistribution permission or replace the inherited implementation with a demonstrably independent implementation |
| Xbox360-ISD / SoftSonus | Repository root states GPLv3, while `source/isd1200.c` and `source/nuvoton_spi.c` state GPL version 2 | Determine which copied/adapted portions are present and obtain clarification before choosing GPLv2/GPLv3 for the whole program |
| Xbox XDK-linked executable | Built using separately supplied proprietary SDK libraries | Check the applicable SDK redistribution terms and compatibility with the final project license |

Upstream sources:

- https://github.com/Byrom90/SonusGUI
- https://github.com/buddyjojo/Xbox360-ISD
- https://github.com/Octal450/J-Runner-with-Extras

The source package deliberately excludes SDKs, user audio, firmware dumps, logs and generated executables. This exclusion alone does not resolve the transport provenance or binary redistribution questions.
