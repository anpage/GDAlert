@echo off

git submodule update --init --recursive

pushd external\godot-cpp
call scons platform=windows generate_bindings=yes use_custom_api_file=yes custom_api_file=../../GDAlert/api.json bits=32 target=release -j8
call scons platform=windows generate_bindings=yes use_custom_api_file=yes custom_api_file=../../GDAlert/api.json bits=32 target=debug -j8

popd

pushd external\Chronoshift\libs\baseconfig
mkdir build
cd build
cmake -A Win32 ..
msbuild baseconfig.sln /p:Configuration=Release /p:Platform=Win32
msbuild baseconfig.sln /p:Configuration=Debug /p:Platform=Win32
popd

pushd external\Chronoshift\libs\captnlog
mkdir build
cd build
cmake -A Win32 ..
msbuild captnlog.sln /p:Configuration=Release /p:Platform=Win32
msbuild captnlog.sln /p:Configuration=Debug /p:Platform=Win32
popd
