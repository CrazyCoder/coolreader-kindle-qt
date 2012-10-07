@echo off
for %%f in (*.ts) do (
  echo %%~nf
  lrelease -compress -silent -nounfinished %%~nf.ts -qm %%~nf.qm
)
