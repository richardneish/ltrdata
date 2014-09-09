@echo off
setlocal
for /f %%a in ('nslookup referens.sth.ip-performance.se^|grep "Addresses:"^|cut -c13-^|cut -f1 -d^,') do set TPTEST_IP_ADDRESS=%%a
tp305con A %TPTEST_IP_ADDRESS% 1641 | ImportTPTEST DSN=TestDB tptest Server=%TPTEST_IP_ADDRESS%
endlocal
