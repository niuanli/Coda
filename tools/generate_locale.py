"""Regenerate compiled locale tables from the editable UTF-8 catalog."""
from pathlib import Path
import json
root=Path(__file__).resolve().parents[1]
catalog=json.loads((root/'assets/locales.json').read_text(encoding='utf-8'))
header=root/'src/ui/Locale.h'
content=header.read_text()
import re
content=re.sub(r'enum TextId \{[^}]+\};', 'enum TextId { '+', '.join('Txt'+key for key in catalog['ui'])+', TxtCount };', content)
header.write_text(content)
def wide(text): return 'L'+json.dumps(text,ensure_ascii=True)
stages={'en':['Ready','Connect','Read backup','Check backup','Save backup','Erase','Install','Verify','Complete','Failed','Cancelled'],
        'ru':['Готово','Подключение','Чтение копии','Проверка копии','Сохранение копии','Стирание','Установка','Проверка','Завершено','Ошибка','Отменено']}
groups=[('Text','TextId',list(catalog['ui'].values()),'TxtCount'),
        ('Message','OperationMessage',[{'en':'','ru':''}]+list(catalog['messages'].values()),'MsgCount'),
        ('StageText','Stage',[dict(en=a,ru=b) for a,b in zip(stages['en'],stages['ru'])],'Cancelled+1')]
cpp='#include "Locale.h"\nnamespace soundstudio {\n'
for name,arg,rows,limit in groups:
    cpp+=f'const wchar_t* Locale::{name}({arg} id) const {{\n    static const wchar_t* values[][2] = {{\n'
    cpp+=''.join('        {'+wide(row['en'])+', '+wide(row['ru'])+'},\n' for row in rows)
    cpp+=f'    }};\n    if ((unsigned)id >= {limit}) return L"";\n    return values[id][russian_ ? 1 : 0];\n}}\n'
cpp+='}\n'
(root/'src/ui/Locale.cpp').write_text(cpp,encoding='ascii')
print('Generated RU/EN locale tables')
