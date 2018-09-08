@echo off

REM -MTd for debug build
set commonFlagsCompiler= -MT -nologo -Gm- -GR- -EHa -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4244 -FC -Z7 -DRAYCASTER_INTERNAL=0 -DRAYCASTER_SLOW=0
set commonFlagsLinker= -incremental:no -opt:ref -subsystem:console

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

cl %commonFlagsCompiler% ..\code\raytracer.cpp /link %commonFlagsLinker%
popd