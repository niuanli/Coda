from pathlib import Path
import wave,struct,subprocess,hashlib
root=Path(__file__).resolve().parent; out=root/'build'
for case,(n,m) in enumerate([(1,1),(4090,4091),(8000,16000),(22000,22000)]):
    paths=[]
    for name,count in [('power',n),('eject',m)]:
        path=out/f'{case}-{name}.wav';paths.append(str(path))
        with wave.open(str(path),'wb') as w:
            w.setparams((1,2,8000,0,'NONE','not compressed'))
            w.writeframes(b''.join(struct.pack('<h',((i*7919)%65536)-32768) for i in range(count)))
    native=out/f'{case}-native.bin';reference=out/f'{case}-reference.bin'
    subprocess.run([str(out/'builder_tests.exe'),*paths,str(native)],check=True)
    subprocess.run([str(out/'JRunnerReference.exe'),*paths,str(reference)],check=True)
    assert native.read_bytes()==reference.read_bytes(),f'J-Runner comparison failed: case {case}'
    print(f'PASS: J-Runner exact match, {n}+{m} samples, SHA256 {hashlib.sha256(native.read_bytes()).hexdigest()}')
