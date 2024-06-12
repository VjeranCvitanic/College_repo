
@echo off
echo
echo Start system_testbench.
echo
::ghdl -a --std=08 -Pxilinx-ise -fsynopsys -fexplicit .\kcpsm6.vhd .\movavg4.vhd .\program.vhd .\system_testbench.vhd

setlocal
set GHDL=ghdl
set FLAGS=--std=08 -Pxilinx-ise -fsynopsys -fexplicit  --warn-no-hide
set FLAGS_WAVE="--wave=wave.ghw"
:: Set Simulation time default 100us
set FLAGS_RUN_TIME=--stop-time=100us  

echo    1) Analyse
%GHDL% -a %FLAGS% system_testbench.vhd program.vhd movavg4.vhd kcpsm6.vhd || exit /b

echo    2) Execute
%GHDL% -e %FLAGS% -frelaxed system_testbench || exit /b

echo    3) Simulate
%GHDL% -r %FLAGS% -frelaxed system_testbench %FLAGS_WAVE% %FLAGS_RUN_TIME% || exit /b

echo    4) Do You Want to start gtkwave?
choice /c YN
if %errorlevel%==1 goto yes
if %errorlevel%==2 goto no
:yes
echo STARTING GTKWAVE
gtkwave wave.ghw	
goto :EOF
:no

endlocal 