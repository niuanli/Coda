"""Render three UI concepts at native console resolution; illustrative data only."""
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
import math
ROOT=Path(__file__).resolve().parents[1]; OUT=ROOT/'designs'; OUT.mkdir(exist_ok=True)
S=2
FONT=ROOT/'assets/font.ttf'
def canvas(color):
    global im,d
    im=Image.new('RGB',(1280*S,720*S),color); d=ImageDraw.Draw(im)
def rect(box,fill,r=0,outline=None):
    d.rounded_rectangle(tuple(int(v*S) for v in box),radius=int(r*S),fill=fill,outline=outline,width=S)
def text(x,y,value,size=20,color='#e8edf2'):
    d.text((x*S,y*S),value,font=ImageFont.truetype(str(FONT),size*S),fill=color,spacing=8*S)
def line(points,color,width=1): d.line([(int(x*S),int(y*S)) for x,y in points],fill=color,width=width*S)
def circle(x,y,r,fill,outline=None): d.ellipse(((x-r)*S,(y-r)*S,(x+r)*S,(y+r)*S),fill=fill,outline=outline,width=S)
def wave(x,y,w,h,color,seed=0):
    for i in range(46):
        a=(.18+.82*abs(math.sin(i*.71+seed)*math.cos(i*.16))) * h
        line([(x+i*w/46,y-a/2),(x+i*w/46,y+a/2)],color,3)
def chip(x,y,key,label,color='#71e4ba',bg='#14292b'):
    circle(x+13,y+14,13,bg,color)
    font=ImageFont.truetype(str(FONT),16*S)
    box=d.textbbox((0,0),key,font=font)
    px=(x+13)*S-(box[2]-box[0])/2-box[0]
    py=(y+14)*S-(box[3]-box[1])/2-box[1]
    d.text((px,py),key,font=font,fill=color)
    text(x+37,y-1,label,18)
def save(name): im.resize((1280,720),Image.Resampling.LANCZOS).save(OUT/(name+'.png'))

# Studio: library-first navigation, rich sound cards and a quiet utility rail.
canvas('#10161c')
rect((0,0,223,720),'#151e26')
text(34,30,'coda',46,'#79e4b4'); text(36,94,'STUDIO / 360',13,'#8495a3')
for y,label,active in [(176,'Обзор',True),(236,'Библиотека',False),(296,'Мои копии',False),(356,'Проверка звука',False)]:
    if active: rect((20,y-8,202,y+40),'#2a443c',10)
    text(39,y,label,20,'#bcefd8' if active else '#97a9b8')
text(32,622,'RU  /  EN',17,'#b1c2cc'); text(32,666,'КОНЦЕПТ 01',12,'#596f7c')
text(265,35,'ВАША ЗВУКОВАЯ СТУДИЯ',14,'#8495a3'); text(1098,35,'01 / STUDIO',14,'#8495a3')
rect((256,88,980,320),'#21392f',18)
text(286,116,'Добавь консоли\nнемного души.',41,'#e3f6e7')
wave(723,186,221,112,'#78d69b')
text(289,244,'Знакомая Xbox. Любимые звуки. Твой характер.',18,'#a2c7b2')
rect((288,280,492,312),'#91e6ac',8); text(306,283,'A   Открыть библиотеку',16,'#14271c')
text(264,352,'Выберите настроение',27); text(265,391,'Примеры обложек · ваши BIN-файлы появятся здесь',16,'#8495a3')
for x,name,c,b,seed in [(256,'Original', '#d1c58e','#353428',1),(506,'Minimal','#7bc9e1','#1c3541',2),(756,'Retro','#db9eaf','#372731',3)]:
    rect((x,435,x+226,613),b,14); wave(x+25,491,178,61,c,seed)
    text(x+20,541,name,27); text(x+20,581,'Образ звуков / BIN',15,c)
rect((1002,88,1241,613),'#1b2630',18)
text(1024,112,'Всегда с копией',24); text(1024,159,'До установки сохраним\nтекущие звуки. После —\nсравним все данные.',18,'#9bafbb')
line([(1024,266),(1218,266)],'#344450')
text(1024,294,'Ваши инструменты',17,'#94a8b5')
chip(1024,341,'X','Создать копию','#78adff'); chip(1024,398,'Y','Проверить звук','#ecd56b')
text(1024,470,'Восстановление',20); text(1024,506,'Открыть мои копии →',17,'#79e4b4')
text(262,666,'A  Выбрать     B  Назад     RB  Язык',16,'#8ca1b0')
text(966,666,'Макет · данные для примера',13,'#657b89'); save('01-studio')

# Orbit: bright blades, green accent, readable from the sofa.
canvas('#e6ebe5')
rect((0,0,1280,126),'#17241b')
text(60,27,'coda',50,'#c5ef86'); text(249,53,'ORBIT',18,'#88a97a'); text(1066,48,'RU  /  EN',19,'#c4d0bd')
text(65,149,'Добавьте характер своей консоли',37,'#243925')
text(66,199,'Звуки, копии и проверка — в одном месте.',22,'#60705e')
for x,y,w,h,color in [(758,288,448,350,'#b8c8b3'),(788,266,400,350,'#8faa84'),(832,244,340,350,'#658c54')]:
    rect((x,y,x+w,y+h),color,22)
rect((60,260,792,611),'#f8faf5',18, '#d0d8cb')
rect((60,260,76,611),'#80b840',5)
text(112,294,'01',58,'#89b34f'); text(210,307,'Библиотека звуков',35,'#243925')
text(112,384,'Выберите образ для включения и лотка.\nCoda сохранит текущие звуки перед установкой.',23,'#62705d')
rect((112,516,407,574),'#6ca333',12); text(138,528,'A   Выбрать образ',26,'#ffffff')
text(871,279,'02',42,'#daefc8'); text(871,340,'Мои\nкопии',37,'#ffffff')
text(871,457,'Сохранить\nи восстановить',22,'#e4f1d9')
line([(869,531),(1125,531)],'#a7c58e'); text(871,552,'X  Создать копию',20,'#ffffff')
text(73,664,'A  Выбрать      Y  Проверить звук      B  Назад',20,'#52674b')
text(1008,674,'КОНЦЕПТ 02 / ORBIT',12,'#74866e'); save('02-orbit')

# Pulse: expressive music identity, large waveform and purposeful actions.
canvas('#17131d'); rect((0,0,1280,720),'#17131d')
text(58,27,'coda',51,'#ffc286'); text(202,54,'PULSE',16,'#b9a0c5')
text(938,45,'Библиотека    Копии    RU / EN',18,'#d0b9da')
rect((52,127,1228,608),'#2b2035',24)
for r,color in [(169,'#211a29'),(137,'#342c3b'),(102,'#231c2a'),(61,'#dd9966'),(17,'#17131d')]: circle(989,337,r,color)
text(83,167,'У КАЖДОГО СТАРТА ЕСТЬ СВОЙ ЗВУК',15,'#d8a585')
text(80,209,'Включай\nпо-своему.',68,'#fff2e5')
text(84,405,'Новый характер для знакомой консоли.',24,'#c3adc9')
wave(85,488,648,57,'#e6a373',3)
rect((83,547,373,591),'#ffc286',11); text(105,554,'A  Найти свой звук',23,'#2c1c18')
text(844,531,'POWER + EJECT',16,'#d8a585')
rect((52,631,425,697),'#2a2131',13); chip(76,648,'X','Сохранить текущие звуки','#7bacff')
rect((443,631,814,697),'#2a2131',13); chip(470,648,'Y','Проверить на консоли','#ecd56b')
text(863,647,'Копия до записи. Проверка после.',18,'#af97b9')
text(864,677,'КОНЦЕПТ 03 / ДАННЫЕ ПРИМЕРА',12,'#775e86'); save('03-pulse')

html='''<!doctype html><html lang="ru"><meta charset="utf-8"><title>Coda — варианты дизайна</title>
<style>body{margin:0;background:#10151b;color:#ebeff3;font:18px system-ui}main{max-width:1280px;margin:auto;padding:40px 24px}h1{font-weight:500}p{color:#9eadba}nav{display:flex;gap:12px;margin:26px 0}button{background:#26323f;color:white;border:1px solid #405363;border-radius:10px;padding:12px 22px;font:inherit;cursor:pointer}button[aria-selected=true]{background:#76dcb2;color:#10251b}img{width:100%;border-radius:18px}section{display:none}section.active{display:block}small{color:#97a5b2}</style>
<main><h1>Coda / три направления</h1><p>Концепты интерфейса для Xbox 360. Это макеты, а не выбранная тема приложения.</p><nav>'''
names=[('01-studio','Studio','Библиотека в центре: обложки, настроение, боковая навигация и отдельная зона резервных копий.'),('02-orbit','Orbit','Отсылка к интерфейсу Xbox 360: светлая палитра, зелёный акцент и крупные панели.'),('03-pulse','Pulse','Музыкальный характер: крупная типографика, винил, выразительная волна и тёплый акцент.')]
for i,(_,name,_) in enumerate(names): html+=f'<button aria-selected="{str(i==0).lower()}" onclick="choose({i})">0{i+1} / {name}</button>'
html+='</nav>'
for i,(file,name,desc) in enumerate(names): html+=f'<section class="{"active" if i==0 else ""}"><img src="{file}.png" alt="Концепт {name}"><p>{desc}</p></section>'
html+='''<small>Во всех вариантах остаются установка BIN, проверенная копия до стирания, восстановление и тест звуков. Иконки A/B/X/Y сохраняют цвета геймпада.</small></main><script>function choose(i){document.querySelectorAll('section').forEach((s,n)=>s.classList.toggle('active',n===i));document.querySelectorAll('button').forEach((b,n)=>b.setAttribute('aria-selected',n===i))}</script></html>'''
(OUT/'index.html').write_text(html,encoding='utf-8'); print('Rendered three Coda concepts and gallery')
