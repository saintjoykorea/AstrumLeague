xcopy "..\CDK\Sources\Platform\windows\bin\%1\*.dll" ".\build\%1\%2\" /syd
xcopy "..\CDK\Sources\Platform\windows\device.cfg" ".\build\%1\%2\" /syd
xcopy "..\CDK\Resources\*" ".\build\%1\%2\bundle\" /syd
xcopy "..\TalesCraft2\Resources\common\*" ".\build\%1\%2\bundle\" /syd
xcopy "..\TalesCraft2\Resources\full\common\*" ".\build\%1\%2\bundle\" /syd
xcopy "..\TalesCraft2\Resources\full\windows\*" ".\build\%1\%2\bundle\" /syd