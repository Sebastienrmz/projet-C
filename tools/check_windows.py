from pathlib import Path
import subprocess
import struct
import tempfile
import shutil

root = Path(__file__).resolve().parents[1]
with tempfile.TemporaryDirectory(prefix='essai projet ', dir=root/'build') as tmp:
    base = Path(tmp)
    source = root/'build/windows-package/Filtres-images-Windows'
    package = base/'Dossier avec espaces'
    shutil.copytree(source, package)
    exe = package/'filtres.exe'
    for mode, filename, output in [('1', 'lena_gray.bmp', 'result_8_negative.bmp'), ('2', 'flowers_color.bmp', 'result_24_negative.bmp')]:
        p = subprocess.run([str(exe)], input=f'{mode}\nimages/{filename}\n1\n0\n0\n', cwd=package, capture_output=True, text=True, timeout=20)
        assert p.returncode == 0, p.stdout+p.stderr
        result = package/output
        assert result.is_file(), p.stdout+p.stderr
        before = (package/'images'/filename).read_bytes()
        after = result.read_bytes()
        offset1 = struct.unpack_from('<I', before, 10)[0]
        offset2 = struct.unpack_from('<I', after, 10)[0]
        width = struct.unpack_from('<i', before, 18)[0]
        bpp = struct.unpack_from('<H', before, 28)[0]
        count = width * (bpp//8)
        assert after[offset2:offset2+count] == bytes(255-x for x in before[offset1:offset1+count])

print('OK : executable Windows et ressources testes dans un chemin avec espaces.')
