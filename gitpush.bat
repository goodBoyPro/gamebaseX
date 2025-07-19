chcp 65001
@echo off
for /f "tokens=*" %%a in ('time /t') do set current_time=%%a
for /f "tokens=*" %%a in ('date /t') do set current_date=%%a
set current_time=%current_time: =%
set current_date=%current_date:/=-%
set current_date=%current_date: =%
set current_date=%current_date:.=%
set time="%current_date% %current_time% Windows平台"
git add .
git commit -m %time%

git remote set-url origin https://github.com/goodBoyPro/gamebaseX.git

git remote get-url tx > nul 2>&1
if %errorlevel% neq 0 (
    echo config repository tx
    git remote add tx https://cnb.cool/compass.lol/gameBaseX.git
)


start git push
git push tx main

pause