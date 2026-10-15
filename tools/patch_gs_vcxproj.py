from pathlib import Path

p = Path(r"C:/warzlocal/warz-server/WO_GameServer/WO_GameServer.vcxproj")
t = p.read_text(encoding="utf-8")

t = t.replace(
    "KY_BUILD_SHIPPING;WO_SERVER;",
    "KY_BUILD_SHIPPING;WO_SERVER;WARZ_LOCAL_NAV_STUB;",
)
t = t.replace(
    "KY_BUILD_DEBUG;WO_SERVER;",
    "KY_BUILD_DEBUG;WO_SERVER;WARZ_LOCAL_NAV_STUB;",
)

old_inc = r".\Sources;.\..\..\..\src\EclipseStudio\Sources;"
new_inc = r".\Sources\NavStub;.\Sources;.\..\..\..\src\EclipseStudio\Sources;"
if old_inc not in t:
    raise SystemExit("include pattern not found")
t = t.replace(old_inc, new_inc)

for name in [
    "AutodeskCustomBridges.cpp",
    "AutodeskNavAgent.cpp",
    "AutodeskNavAvoidanceFilter.cpp",
    "AutodeskNavMesh.cpp",
    "AutodeskNavProfile.cpp",
]:
    needle = rf'<ClCompile Include="..\..\..\src\GameEngine\ai\AutodeskNav\{name}" />'
    repl = (
        rf'<ClCompile Include="..\..\..\src\GameEngine\ai\AutodeskNav\{name}">'
        r"<ExcludedFromBuild>true</ExcludedFromBuild></ClCompile>"
    )
    if needle not in t:
        raise SystemExit(f"missing {name}")
    t = t.replace(needle, repl)

if "AutodeskNavStub.cpp" not in t:
    insert = (
        '    <ClCompile Include="Sources\\NavStub\\AutodeskNavStub.cpp" />\n'
        '    <MASM Include="Sources\\PhysXTypeInfoShim.asm" />\n'
    )
    marker = (
        r'<ClCompile Include="..\..\..\src\GameEngine\ai\AutodeskNav\AutodeskNavProfile.cpp">'
        r"<ExcludedFromBuild>true</ExcludedFromBuild></ClCompile>"
    )
    t = t.replace(marker, marker + "\n" + insert)

if "masm.props" not in t:
    t = t.replace(
        r'<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />',
        r'<Import Project="$(VCTargetsPath)\Microsoft.Cpp.props" />'
        "\n  <ImportGroup Label=\"ExtensionSettings\">\n"
        r'    <Import Project="$(VCTargetsPath)\BuildCustomizations\masm.props" />'
        "\n  </ImportGroup>",
    )
    t = t.replace(
        r'<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />',
        r'<Import Project="$(VCTargetsPath)\Microsoft.Cpp.targets" />'
        "\n  <ImportGroup Label=\"ExtensionTargets\">\n"
        r'    <Import Project="$(VCTargetsPath)\BuildCustomizations\masm.targets" />'
        "\n  </ImportGroup>",
    )

t = t.replace(
    "wbemuuid.lib;%(AdditionalDependencies)</AdditionalDependencies>",
    "wbemuuid.lib;legacy_stdio_definitions.lib;CrashRpt1301.lib;%(AdditionalDependencies)</AdditionalDependencies>",
)
t = t.replace(
    r"C:\warzlocal\warz-client\External\ChilKat\Libs;",
    r"C:\warzlocal\warz-client\External\ChilKat\Libs;C:\warzlocal\warz-client\External\CrashRpt\Lib;",
)

p.write_text(t, encoding="utf-8")
print(
    "OK",
    t.count("WARZ_LOCAL_NAV_STUB"),
    "Excluded",
    t.count("ExcludedFromBuild"),
    "stub",
    "AutodeskNavStub.cpp" in t,
)
