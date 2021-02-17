import os
import osproc
import streams
import strformat
import dialogs
import registry

type JavaException* = IOError

proc javaEnvPath(): string =
    if not os.existsEnv("JAVA_HOME"):
        return ""

    return os.getEnv("JAVA_HOME") & "bin\\javaw.exe"

proc getMinecraftInstallationDirectory(): string =
    return registry.getUnicodeValue("SOFTWARE\\Mojang\\InstalledProducts\\Minecraft Launcher", "InstallLocation", registry.HKEY_CURRENT_USER)

proc isValidJavaPath(path: string): bool =
    echo "Checking for valid java installation @ " & path
    try:
        let process = osproc.startProcess(
         command = path,
         args = ["-version"],
         options={}
        )

        # Forward the log output
        let strm = osproc.outputStream(process)
        var line = ""
        while strm.readLine(line):
            # Check the java version here?
            echo line

        close(process)
        return process.peekExitCode() == 0
    except Exception:
        return false

proc findJavaPath(): string =
    try:
        let mcDir = getMinecraftInstallationDirectory()
        let McJREPaths = [
            mcDir & "runtime\\jre-legacy\\windows-x64\\jre-legacy\\bin\\javaw.exe",
            mcDir & "runtime\\jre-legacy\\windows-x68\\jre-legacy\\bin\\javaw.exe",
            mcDir & "runtime\\jre-x64\\bin\\javaw.exe", 
            mcDir & "runtime\\jre-x86\\bin\\javaw.exe",
            ]

        for path in McJREPaths:
            if isValidJavaPath(path):
                return path
    except Exception:
        echo "Failed to find minecraft installation dir in the registry. Is it installed?"

    let JREPaths = [
        javaEnvPath(),
        "javaw",
        ]

    for path in JREPaths:
        if isValidJavaPath(path):
            return path

    raise JavaException.newException("Failed to find a valid installation of java")

proc runInstaller(path: string): void =
    let process = osproc.startProcess(
         command = path,
         args = ["-jar", os.paramStr(0)],
         options={poEchoCmd, poStdErrToStdOut}
        )

    # Forward the log output
    let strm = osproc.outputStream(process)
    var line = ""
    while strm.readLine(line):
        echo line

    close(process)

    # Forward the exit code
    let exitCode = process.peekExitCode()
    echo &"Fabric installer exit code: {exitCode}"
    system.quit(exitCode)

when isMainModule:
    echo "Fabric installer bootstrap"

    var path = ""

    try:
        path = findJavaPath()
    except JavaException as e:
        echo e.msg
        dialogs.error(nil, "The Fabric installer could not find Java installed on your computer. Please visit the Fabric wiki at https://fabricmc.net/wiki/ for help installing Java.")
        system.quit(-1)

    runInstaller(path)
