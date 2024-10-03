@echo off
for /L %%c in (1,1,4) do (
    for /L %%t in (1,1,16) do (
        "./x64/Release/Lab2.exe" MAX.bmp blur.bmp %%c %%t
	echo ===============
    )
)
pause