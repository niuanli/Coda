"""Generate editable native XUI assets; no designer or web runtime required."""
from pathlib import Path
import xml.etree.ElementTree as ET

root = Path(__file__).resolve().parents[1]
canvas = ET.Element('XuiCanvas', version='000c')
def element(parent, tag, **values):
    node = ET.SubElement(parent, tag)
    props = ET.SubElement(node, 'Properties')
    for key, value in values.items():
        ET.SubElement(props, key).text = str(value)
    return node
props = ET.SubElement(canvas, 'Properties')
for key, value in [('Width', '1280.000000'), ('Height', '720.000000')]: ET.SubElement(props, key).text = value
scene = element(canvas, 'XuiScene', Id='CodaScene', Width=1280, Height=720,
                ClassOverride='CodaScene', Visual='XuiSceneBlank', DefaultFocus='a')
element(scene, 'XuiImage', Id='background', Width=1280, Height=720, ImagePath='background.png', SizeMode=4)
element(scene, 'XuiImage', Id='homeArt', Width=1280, Height=720, ImagePath='studio-home.png', SizeMode=4, Show='false')
element(scene, 'XuiImage', Id='wave', Width=180, Height=120, Position='790,120,0', ImagePath='studio-wave.png', SizeMode=4, Show='false')
def text(id, content, x, y, w, h, size, color='0xffe8edf2'):
    element(scene, 'XuiText', Id=id, Text=content, Width=w, Height=h, Position=f'{x},{y},0',
            PointSize=size, TextColor=color, TextStyle=256, DropShadowColor='0x00000000')
text('brand', 'coda', 32, 32, 200, 70, 40, '0xff79e4b4')
text('subtitle', 'CONSOLE SOUND MANAGER', 284, 45, 706, 32, 12, '0xff96a9b9')
text('version', '0.4.11 / STUDIO', 1050, 45, 172, 28, 13, '0xff96a9b9')
text('title', '', 306, 145, 664, 78, 26)
text('body', '', 306, 254, 650, 277, 18)
text('detail', '', 306, 551, 650, 76, 13, '0xff96a9b9')
text('progress', '', 284, 661, 950, 36, 14, '0xff79e4b4')
text('notice', '', 284, 655, 950, 48, 13, '0xfff9d34d')
text('aboutAuthor', '', 306, 242, 650, 68, 20)
text('aboutBody', '', 306, 332, 650, 194, 14)
text('aboutFooter', '', 306, 547, 650, 76, 12, '0xff96a9b9')
text('actions', '', 36, 132, 212, 30, 12, '0xff96a9b9')
text('homeTitle', '', 306, 120, 470, 100, 30, '0xffe3f6e7')
text('tagline', '', 306, 237, 658, 60, 15, '0xffa2c7b2')
text('libraryHeading', '', 284, 348, 696, 38, 23)
text('libraryHint', '', 284, 391, 696, 52, 13, '0xff96a9b9')
text('safetyTitle', '', 1030, 148, 188, 66, 20)
text('safetyBody', '', 1030, 238, 188, 356, 14, '0xff96a9b9')
for i in range(3):
    text('cardName'+str(i), '', 300+i*240, 530, 188, 70, 16)
    text('cardInfo'+str(i), '', 300+i*240, 611, 188, 27, 11, '0xff96a9b9')
buttons = [('a',22528, 'button_a.png',182), ('x',22530,'button_x.png',410),
           ('y',22531,'button_y.png',334), ('lb',22533,'button_lb.png',258),
           ('b',22529,'button_b.png',562), ('workshop',22548,'button_start.png',486), ('exit',22549,'ButtonBACK.png',562),
           ('language',22532,'button_rb.png',626)]
for id, key, image, y in buttons:
    element(scene, 'XuiButton', Id=id, Width=224, Height=64, Position=f'20,{y},0',
            Visual='Coda_Action', Show='false', Enabled='false', UnfocusedInput='true',
            Text='', ImagePath=image, PressKey=key)
element(scene, 'XuiButton', Id='about', Width=224, Height=64, Position='1013,562,0',
        Visual='Coda_Action', Show='false', Enabled='false', UnfocusedInput='true',
        Text='', ImagePath='button_ls.png', PressKey=22550)
lst = element(scene,'XuiButton', Id='library', Width=650, Height=286, Position='300,244,0',
              Visual='Coda_Catalogue', Show='false', Enabled='false', Text='SELECT AN IMAGE')
band=element(scene,'XuiFigure', Id='catalogueSelection', Width=650, Height=36,
             Position='300,289,0', Show='false', Closed='true',
             Points='4,0,0,0,0,650,0,0,650,0,650,0,650,36,0,650,36,650,36,0,36,0,0,36,0,36,0,0,0,')
fill=ET.SubElement(band.find('Properties'),'Fill')
ET.SubElement(ET.SubElement(fill,'Properties'),'FillColor').text='0xff254940'
for i in range(6):
    element(scene,'XuiText', Id=f'catalogueRow{i}', Width=626, Height=36, Position=f'312,{289+i*38},0', Show='false',
            PointSize=14, TextColor='0xffe8edf2', TextStyle=20784, DropShadowColor='0x00000000')
element(scene,'XuiText', Id='cataloguePage', Width=650, Height=14, Position='300,516,0', Show='false',
        PointSize=9, TextColor='0xff96a9b9', TextStyle=256, DropShadowColor='0x00000000')
ET.indent(canvas)
ET.ElementTree(canvas).write(root/'assets/scene.xui', encoding='utf-8')

# Evolve the original native skin while keeping its tested XUI animation semantics.
skin_path = root/'assets/skin.xui'
skin = ET.parse(skin_path)
replacements = {'Byrom_LegendBtn':'Coda_Action', 'Byrom_List':'Coda_List', 'Byrom_List_Button':'Coda_ListItem',
                '0xffebeb0f':'0xff61e6b4', '0xff10109e':'0xff204c46', '0xff0f0f80':'0xff345249',
                '0xffebebeb':'0xffe8edf2', '0x96ebebeb':'0x00000000', '0x800f0f0f':'0x00000000'}
for node in skin.iter():
    if node.text in replacements: node.text = replacements[node.text]
    if node.tag == 'TextStyle' and node.text == '67114000': node.text = '5120'
# The old skin put a caption BELOW a 70x54 icon in an 80-high visual,
# while the scene allocated only 64 pixels. Own both geometries here.
for visual in list(skin.getroot().findall('XuiVisual')):
    if visual.findtext('Properties/Id') in ('Coda_Action','Coda_Catalogue','Coda_List','Coda_ListItem'):
        skin.getroot().remove(visual)
catalogue=element(skin.getroot(),'XuiVisual',Id='Coda_Catalogue',Width=650,Height=286)
element(catalogue,'XuiTextPresenter',Id='caption',Width=650,Height=36,
        Position='0,0,0',PointSize=12,TextColor='0xff96a9b9',TextStyle=256,
        DropShadowColor='0x00000000')
action = element(skin.getroot(), 'XuiVisual', Id='Coda_Action', Width=224, Height=64)
def panel(parent, id, color, opacity=1):
    figure = element(parent, 'XuiFigure', Id=id, Width=224, Height=64, Anchor=15,
                     Opacity=opacity, Closed='true',
                     Points='4,0,0,0,0,224,0,0,224,0,224,0,224,64,0,224,64,224,64,0,64,0,0,64,0,64,0,0,0,')
    fill = ET.SubElement(figure.find('Properties'), 'Fill')
    props = ET.SubElement(fill, 'Properties')
    ET.SubElement(props, 'FillColor').text = color
panel(action, 'row', '0xff1a2935')
panel(action, 'focus', '0xff254940', 0)
element(action, 'XuiImagePresenter', Id='icon', Width=32, Height=32, Position='16,16,0', SizeMode=4)
element(action, 'XuiTextPresenter', Id='caption', Width=162, Height=64, Position='56,0,0',
        TextColor='0xffe8edf2', PointSize=12, TextStyle=20768, DropShadowColor='0x00000000')
timelines = ET.SubElement(action, 'Timelines')
frames = ET.SubElement(timelines, 'NamedFrames')
for name, time in [('Normal',0),('EndNormal',1),('KillFocus',0),('EndKillFocus',1),
                   ('Focus',2),('EndFocus',3),('Press',4),('EndPress',5),
                   ('NormalDisable',6),('EndNormalDisable',7)]:
    frame = ET.SubElement(frames, 'NamedFrame')
    ET.SubElement(frame, 'Name').text = name
    ET.SubElement(frame, 'Time').text = str(time)
    if name.startswith('End'): ET.SubElement(frame,'Command').text = 'stop'
timeline = ET.SubElement(timelines,'Timeline')
ET.SubElement(timeline,'Id').text = 'focus'
ET.SubElement(timeline,'TimelineProp').text = 'Opacity'
for time, value in [(0,0),(1,0),(2,1),(3,1),(4,1),(5,1),(6,0),(7,0)]:
    key = ET.SubElement(timeline,'KeyFrame')
    ET.SubElement(key,'Time').text = str(time)
    ET.SubElement(key,'Interpolation').text = '0'
    ET.SubElement(key,'Prop').text = str(value)
ET.indent(skin)
skin.write(skin_path, encoding='utf-8')
print('Generated scene.xui and Coda skin')
