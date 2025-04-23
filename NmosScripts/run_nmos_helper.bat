:: ~~~~~
:: 
:: Code Sample License Agreement
:: Effective Date: 4/15/2025
:: Cinnafilm, Inc. ("Licensor") grants Finn Thomas ("Licensee") a non-exclusive, 
:: non-transferable, revocable license to use the provided code samples ("Code") 
:: under the following terms:
:: - Permitted Use: Licensee may use the Code solely for personal, non-commercial 
:: purposes, such as inclusion in a portfolio or demonstration during job 
:: interviews.
:: - Restrictions: Licensee may not: (a) use the Code for any commercial purpose; 
:: (b) distribute, sell, sublicense, or otherwise share the Code with third 
:: parties; (c) modify the Code for purposes beyond personal demonstration; or 
:: (d) claim ownership of the Code.
:: - Ownership: The Code remains the exclusive property of Cinnafilm, Inc.
:: - Termination: This license may be terminated by Licensor at any time with 
:: written notice to Licensee, after which Licensee must cease all use of the Code.
:: No Warranty: The Code is provided "as is," with no warranties of any kind.
:: By using the Code, Licensee agrees to these terms.
:: 
:: ~~~~~

@echo off
setlocal

echo Starting NMOS helper... If something doesn't work, please reference README.txt

echo Starting the NMOS Test Registry...
start "NMOS Registry" cmd /k "NmosTestRegistry\nmos-cpp-registry.exe"
timeout /t 3 /nobreak >nul

echo Starting the NMOS Test Sender...
start "NMOS Sender" cmd /k "NmosTestSender\NmosTestSenderWithAudio.exe"
timeout /t 3 /nobreak >nul

echo Starting the NMOS Controller...
start "NMOS Controller" cmd /k "py NmosTestController\NmosController.py"

REM Keep the batch window open until done
echo Press any key to stop all NMOS helper processes...
pause

echo Terminating all processes started by this script
taskkill /FI "WINDOWTITLE eq NMOS Registry - NmosTestRegistry\nmos-cpp-registry.exe" /T /F
taskkill /FI "WINDOWTITLE eq NMOS Sender - NmosTestSender\NmosTestSenderWithAudio.exe" /T /F

taskkill /FI "WINDOWTITLE eq NMOS Controller - py  NmosTestController\NmosController.py"
if %errorlevel% neq 0 (
    echo Trying again to kill NMOS controller
    REM Handle the error here
) else (
    taskkill /FI "WINDOWTITLE eq Administrator: NMOS Controller"
)

endlocal