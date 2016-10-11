:: Ensure to execute pip install pyinstaller
pyinstaller --distpath ../ --onefile build_gettext_catalog.py
pyinstaller --distpath ../ --onefile build_gettext_catalog_nsi.py
pyinstaller --distpath ../ --onefile build_locale_nsi.py
pyinstaller --distpath ../ --onefile gen_error_wikitable.py
pyinstaller --distpath ../ --onefile pull_pootle_catalogs.py
pyinstaller --distpath ../ --onefile pull_pootle_menu_pics.py
pause