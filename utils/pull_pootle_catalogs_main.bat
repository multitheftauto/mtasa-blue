:: pull_pootle_catalogs with latest languages
:: **Don't forget to update installer.languages on the build server**
pull_pootle_catalogs   -p client         -o "../Shared/data/MTA San Andreas/MTA/locale"   -L ar,bg,cs,da,de,el,es,et,fr,hr,hu,id,it,ja,lt,lv,mk,nb,nl,pl,pt_BR,ro,ru,sk,sl,sv,tr,vi,uk,zh_CN,zh_TW    -e ccw,Talidan,qaisjp,botder
pull_pootle_catalogs   -p installer  -G  -o "../Shared/installer/locale/"                 -L ar,bg,cs,da,de,el,es,et,fr,hr,hu,id,it,ja,lt,lv,mk,nb,nl,pl,pt_BR,ro,ru,sk,sl,sv,tr,vi,uk,zh_CN,zh_TW
pause
