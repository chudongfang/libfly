#!/bin/bash


#ls_date=`date +%Y-%m-%d`
ls_date='update'
#ctags -R .
git pull 
git add .
git commit -m ${ls_date}
git push origin
