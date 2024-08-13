#!/bin/bash
rm -rf release
mkdir -p release

cp -rf Puara *.{hpp,cpp,txt,json} LICENSE release/

mv release score-addon-puara
7z a score-addon-puara.zip score-addon-puara
