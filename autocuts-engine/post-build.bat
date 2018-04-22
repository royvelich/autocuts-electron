@echo off

SET hh=%time:~0,2%
IF "%time:~0,1%"==" " SET hh=0%hh:~1,1%

SET dt=%date:~10,4%_%date:~4,2%_%date:~7,2%_%hh%_%time:~3,2%_%time:~6,2%

SET sourceFilePath=%1
SET targetDirectoryPath=%2

FOR %%i IN ("%sourceFilePath%") DO (
  SET fileName=%%~ni
  SET fileExtension=%%~xi
)

SET appendedFileName=%fileName%_%dt%%fileExtension%
SET targetFilePath=%targetDirectoryPath%%appendedFileName%

REM ECHO %sourceFilePath%
REM ECHO %targetDirectoryPath%
REM ECHO %fileName%
REM ECHO %fileExtension%
REM ECHO %appendedFileName%
REM ECHO %targetFilePath%

ECHO xcopy %sourceFilePath% %targetFilePath%
xcopy %sourceFilePath% %targetFilePath%*