from pathlib import Path
import re

p = Path(r"C:/warzlocal/warz-server/MasterServer/MasterServer.vcxproj")
t = p.read_text(encoding="utf-8")
if "MeshStub.cpp" not in t:
    m = re.search(r'<ClCompile Include="[^"]+\.cpp" />', t)
    if not m:
        raise SystemExit("no ClCompile found")
    insert = '\n    <ClCompile Include="Sources\\MeshStub.cpp" />'
    t = t[: m.end()] + insert + t[m.end() :]
if "ImageHasSafeExceptionHandlers" not in t:
    t = t.replace(
        "<SubSystem>Console</SubSystem>",
        "<SubSystem>Console</SubSystem>\n      <ImageHasSafeExceptionHandlers>false</ImageHasSafeExceptionHandlers>",
    )
p.write_text(t, encoding="utf-8")
print("ok", t.count("MeshStub.cpp"))
