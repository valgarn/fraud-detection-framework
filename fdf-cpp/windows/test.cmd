@echo off
rem fdf.exe "G:\OnSource\Projects\Fraud detection\Images\FROUD_CUST\575163.jpg"

for /f "tokens=*" %%G in ('dir /b /s "G:\OnSource\Projects\Fraud detection\Images\FROUD_CUST"') do (
	echo Found %%G
)

