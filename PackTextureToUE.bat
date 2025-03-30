@rem                  TexturePacker 自动生成脚本
@rem 
@rem 1.先将TexturePacker.exe的所在目录加到系统变量Path后面
@rem 2.将脚本与图片放在同一目录
@echo off
SET "PATH=?D:/Softwares/TexturePacker/bin;%PATH%"
for /f "delims=" %%i in ("%cd%") do set folder=%%~ni

@echo on
@set file_name = %folder%
@set img_format=RGBA8888
@echo "删除旧图集"
del /f /q %file_name%0.png
del /f /q %file_name%0.paper2dsprites
del /f /q %file_name%1.png
del /f /q %file_name%1.paper2dsprites
del /f /q %file_name%2.png
del /f /q %file_name%2.paper2dsprites
del /f /q %file_name%3.png
del /f /q %file_name%3.paper2dsprites
del /f /q %file_name%4.png
del /f /q %file_name%4.paper2dsprites
del /f /q %file_name%5.png
del /f /q %file_name%5.paper2dsprites

@echo "开始生成新图集"
TexturePacker ./ --sheet %file_name%(n).png --data %file_name%(n).paper2dsprites --disable-rotation --multipack --format unreal-paper2d --opt %image_format% --max-size 2048

@pause