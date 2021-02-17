author        = "FabricMC"
description   = "A native bootstrap for fabric-installer that utlises the JRE included with minecraft.A native bootstrap for fabric-installer that utlises the JRE included with minecraft."
license       = "Apache-2.0"
srcDir        = "."
bin           = @["bootstrap"]
binDir        = "build/bin/"

requires "nim >= 1.4.2"
requires "dialogs >= 1.1.1"