@ECHO OFF
for %%i in (*.vert) do %VULKAN_SDK%/Bin/glslangValidator.exe -V %%i
for %%i in (*.frag) do %VULKAN_SDK%/Bin/glslangValidator.exe -V %%i
pause