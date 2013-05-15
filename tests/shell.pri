shell_scripts.commands += $$system(touch tests.xml)
shell_scripts.files += runtests.sh tests.xml

# fixed path required by TATAM
shell_scripts.path += /usr/share/libmeegoreactionmap-tests
shell_scripts.depends = FORCE

INSTALLS    += \
              shell_scripts
