@echo off
cd vendor/v8/sources
md out/x64.release
@REM echo "fetch v8"
@REM fetch v8
@REM echo "git pull"
@REM git pull
@REM echo "gclient sync"
@REM gclient sync
git pull && gclient sync

echo "compiling..."
python tools/dev/gm.py x64.release --args'is_debug=false is_component_build=true v8_use_external_startup_data=true v8_static_library=false v8_enable_i18n_support=false target_cpu="x64" v8_target_cpu="x64" use_custom_libcxx=false'
echo "compiled"

