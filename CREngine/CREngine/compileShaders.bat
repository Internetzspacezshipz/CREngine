cd /D "%~dp0"
for %%A in (.\Shaders\*) do (
	echo %%A
	C:\VulkanSDK\1.3.239.0\Bin\glslc.exe %%A -o %%~dpA\Compiled\%%~nxA.spv
)