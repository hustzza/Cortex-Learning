## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,em4f linker.cmd \
  package/cfg/app_pem4f.oem4f \

linker.cmd: package/cfg/app_pem4f.xdl
	$(SED) 's"^\"\(package/cfg/app_pem4fcfg.cmd\)\"$""\"C:/Users/Greg/git/Cortex-Learning/SYSBIOS LED Toggle/.config/xconfig_app/\1\""' package/cfg/app_pem4f.xdl > $@
