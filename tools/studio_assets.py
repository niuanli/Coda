"""Native Studio artwork and controller glyphs, rendered at 4x for clean edges."""
from pathlib import Path
from PIL import Image,ImageDraw,ImageFont
import math
root=Path(__file__).resolve().parents[1]; assets=root/'assets'; scale=2
def image(transparent=False):
    im=Image.new('RGBA',(1280*scale,720*scale),(0,0,0,0) if transparent else '#10161c')
    return im,ImageDraw.Draw(im)
def box(d,xy,color,r=0): d.rounded_rectangle(tuple(v*scale for v in xy),radius=r*scale,fill=color)
def save(im,name): im.resize((1280,720),Image.Resampling.LANCZOS).save(assets/name)
im,d=image(); box(d,(0,0,256,720),'#151e26')
box(d,(280,104,992,642),'#172029',16); box(d,(1010,104,1240,642),'#1b2630',16)
save(im,'background.png')
im,d=image(True); box(d,(280,104,992,642),'#10161c')
box(d,(280,104,992,318),'#21392f',16)
for i,(bg,col) in enumerate([('#353428','#d1c58e'),('#1c3541','#7bc9e1'),('#372731','#db9eaf')]):
    x=280+i*240; box(d,(x,451,x+224,640),bg,16)
    for j in range(30):
        height=14+44*abs(math.sin(j*.6+i)*math.cos(j*.21))
        xx=x+20+j*6.1
        d.line((xx*scale,(493-height/2)*scale,xx*scale,(493+height/2)*scale),fill=col,width=2*scale)
save(im,'studio-home.png')
im=Image.new('RGBA',(360,240),(0,0,0,0)); d=ImageDraw.Draw(im)
for j in range(28):
    height=26+178*abs(math.sin(j*.67)*math.cos(j*.15))
    x=10+j*12
    d.line((x,120-height/2,x,120+height/2),fill='#78d69b',width=5)
im.resize((180,120),Image.Resampling.LANCZOS).save(assets/'studio-wave.png')
colors={'a':'#68d35b','b':'#f46564','x':'#5da7ff','y':'#f9d34d','lb':'#cbd5de','rb':'#cbd5de','back':'#cbd5de','start':'#cbd5de','ls':'#cbd5de'}
for key,col in colors.items():
    size=4; im=Image.new('RGBA',(80*size,80*size),(0,0,0,0)); d=ImageDraw.Draw(im)
    d.ellipse((6*size,6*size,74*size,74*size),outline=col,width=3*size)
    if key=='back': d.line([(48*size,24*size),(32*size,40*size),(48*size,56*size)],fill=col,width=3*size,joint='curve')
    elif key=='start': d.line([(32*size,24*size),(48*size,40*size),(32*size,56*size)],fill=col,width=3*size,joint='curve')
    else:
        label="L3" if key=="ls" else key.upper(); font=ImageFont.truetype(str(assets/'font.ttf'),(26 if len(key)>1 else 38)*size)
        bounds=d.textbbox((0,0),label,font=font)
        x=40*size-(bounds[2]-bounds[0])/2-bounds[0]; y=40*size-(bounds[3]-bounds[1])/2-bounds[1]
        d.text((x,y),label,font=font,fill=col)
    name='ButtonBACK.png' if key=='back' else 'button_'+key+'.png'
    im.resize((80,80),Image.Resampling.LANCZOS).save(assets/name)
print('Rendered native Studio artwork and centered controller glyphs')
