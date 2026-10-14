"""Pin J-Runner's two-sound ISD template and generate an independent C# oracle."""
from pathlib import Path
import re,hashlib
root=Path(__file__).resolve().parents[1]
source=(root/'research/JRunner.SoundEditor.cs').read_text(encoding='utf-8-sig')
def method(signature):
    start=source.index(signature); brace=source.index('{',start); depth=1; end=brace+1
    while depth:
        if source[end]=='{':depth+=1
        if source[end]=='}':depth-=1
        end+=1
    return source[start:end]
generate=method('public byte[] generate_isd_bin(byte[] startup, byte[] eject)')
out='// ISD template from J-Runner with Extras (MIT), see research/JRunner.LICENSE.\n#pragma once\nnamespace soundstudio {\n'
for csname,name in [('header','IsdHeader'),('table','IsdTable')]:
    values=re.search(r'byte\[\] '+csname+r' = \{(.*?)\};',generate,re.S).group(1)
    out+='static const Byte '+name+'[] = {'+values+'};\n'
out+='}\n'; (root/'src/core/ImageTemplate.h').write_text(out)
parts=[method('private byte[] split_nible(int num)'),method('public byte[] wav_to_raw(byte[] wav)'),method('public static int bytearray_to_int(byte[] value)'),method('public static string ByteArrayToString('),generate]
oracle='using System; using System.IO; using System.Diagnostics;\nclass ISD_NAND { public int count; public byte[] isd_bin=new byte[0xB000],startup,eject,header,table;\n'+'\n'.join(parts)+'\n}\n'
oracle+='class Reference { static void Main(string[] args) { var n=new ISD_NAND(); File.WriteAllBytes(args[2],n.generate_isd_bin(n.wav_to_raw(File.ReadAllBytes(args[0])),n.wav_to_raw(File.ReadAllBytes(args[1])))); } }\n'
(root/'tests/JRunnerReference.cs').write_text(oracle)
print('Reference SHA256:',hashlib.sha256(source.encode()).hexdigest())
