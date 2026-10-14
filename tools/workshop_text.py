from pathlib import Path
import json
root=Path(__file__).resolve().parents[1];path=root/'assets/locales.json'
data=json.loads(path.read_text(encoding='utf-8'))
ui={
'Workshop':('Workshop','Мастерская'),
'PickPower':('1 / Choose the power sound','1 / Звук включения'),
'PickEject':('2 / Choose the eject sound','2 / Звук лотка'),
'ImportHint':('WAV PCM: mono, 8000 Hz, 16 bit. Source files are kept unchanged.','WAV PCM: моно, 8000 Гц, 16 бит. Исходники сохраняются без изменений.'),
'NoImports':('No WAV files found.\nPlace two WAV files in Imports next to Coda.xex.\nThen reopen the workshop.','Файлов WAV пока нет.\nПоместите два WAV в Imports рядом с Coda.xex.\nЗатем откройте мастерскую снова.'),
'WorkshopReview':('3 / Create your sound image','3 / Создание образа'),
'BuildSteps':('\n\nCoda will create a full ISD image and save it in Sounds.\n\nOpen the library afterwards to review and install it.\nThe chip is not changed during image creation.','\n\nCoda соберёт полный образ ISD и сохранит его в Sounds.\n\nЗатем откройте библиотеку для установки.\nПри создании образа чип не изменяется.'),
'BuildImage':('Create BIN','Собрать BIN'),
'OpenLibrary':('Open library','Открыть библиотеку'),
'OutputPrefix':('Saved: ','Сохранено: '),
'BuildingTitle':('Creating your sound image','Сборка звукового образа'),
}
messages={
'BuildReading':('Reading source WAV files...','Чтение исходных WAV...'),
'BuildDone':('BIN created and verified. Open the library to install it.','BIN создан и проверен. Для установки откройте библиотеку.'),
'BuildReadFailed':('Cannot read the WAV files. Maximum input file size is 1 MiB.','Не удалось прочитать WAV. Максимальный размер файла — 1 МиБ.'),
'BuildInvalid':('Malformed WAV file. Check the RIFF header and audio data.','Повреждённый WAV. Проверьте заголовок RIFF и аудиоданные.'),
'BuildUnsupported':('Use WAV PCM mono, 8000 Hz, 16 bit. Resampling and MP3 are not supported yet.','Нужен WAV PCM: моно, 8000 Гц, 16 бит. Пересчёт частоты и MP3 пока недоступны.'),
'BuildEmpty':('The selected WAV contains no audio samples.','Выбранный WAV не содержит аудиосэмплов.'),
'BuildTooLarge':('Sounds do not fit in the chip. Shorten them; combined encoded audio must fit in 44800 bytes.','Звуки не помещаются в чип. Сократите их: предел закодированного аудио — 44800 байт.'),
'BuildSaveFailed':('Cannot save and verify the new BIN. Check free space and write access.','Не удалось сохранить и проверить BIN. Проверьте свободное место и доступ для записи.'),
}
for group,rows in [('ui',ui),('messages',messages)]:
    for key,(en,ru) in rows.items():data[group][key]={'en':en,'ru':ru}
path.write_text(json.dumps(data,ensure_ascii=False,indent=2),encoding='utf-8')
