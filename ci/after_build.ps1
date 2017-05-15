$SRC_DIR = $PWD.Path
$STAGE = [System.Guid]::NewGuid().ToString()

Set-Location $ENV:Temp
New-Item -Type Directory -Name $STAGE
Set-Location $STAGE

$ZIP = "$SRC_DIR\bbmod.zip"

Copy-Item "$SRC_DIR\$($Env:CONFIGURATION)\dinput8.dll" '.\'
Copy-Item "$SRC_DIR\$($Env:CONFIGURATION)\dinput8.pdb" '.\'
Copy-Item "$SRC_DIR\README.md" '.\'
Copy-Item "$SRC_DIR\CHANGELOG.md" '.\'
Copy-Item "$SRC_DIR\addons" '.\' -Recurse

7z a "$ZIP" *

Push-AppveyorArtifact "$ZIP"

Set-Location $SRC_DIR
