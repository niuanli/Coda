from pathlib import Path
import json
root=Path(__file__).resolve().parents[1]; path=root/'assets/locales.json'
data=json.loads(path.read_text(encoding='utf-8'))
values={
    'HomeTitle':('Give your console\na little soul.','Добавь консоли\nнемного души.'),
    'HomeBody':('Familiar Xbox. Favorite sounds. Your character.','Знакомая Xbox. Любимые звуки. Твой характер.'),
    'SafetyTitle':('Always backed up','Всегда с копией'),
    'StudioSafety':('Before installation, save and verify the current sounds.\n\nAfter installation, compare every byte.\n\nKeep the console powered on until completion.','Перед установкой сохраним и проверим текущие звуки.\n\nПосле установки сравним каждый байт.\n\nНе выключайте консоль до завершения.'),
    'Collection':('In your library','В вашей библиотеке'),
    'EmptyCollection':('Three steps to your sound','Три шага к своему звуку'),
    'CollectionHint':('First three images from Sounds. Open the library to see every file.','Первые три образа из Sounds. Все файлы доступны в библиотеке.'),
    'EmptyCollectionHint':('Place BIN images in Sounds, then open the library.','Поместите образы BIN в Sounds, затем откройте библиотеку.'),
    'StepSave':('Save your sounds','Сохранить звуки'),
    'StepInstall':('Install an image','Установить образ'),
    'StepTest':('Hear the result','Послушать результат'),
    'StepSaveInfo':('Verified backup','Проверенная копия'),
    'StepInstallInfo':('Read-back verification','Проверка после записи'),
    'StepTestInfo':('Console speaker','Динамик консоли'),
    'ImageValid':('BIN / ready to install','BIN / готов к установке'),
    'ImageInvalid':('BIN / invalid image','BIN / неверный образ'),
}
for key,(en,ru) in values.items(): data['ui'][key]={'en':en,'ru':ru}
path.write_text(json.dumps(data,ensure_ascii=False,indent=2),encoding='utf-8')
print('Updated Studio copy for RU/EN')
