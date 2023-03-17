## Recompile

*For ubuntu 22.04*

1. Install VM
    - I used ubuntu server iso (I don't need GUI)
    - allocate enough disk size (at least 30GB aside from LVM)
2. `sudo -s` 
    - with root permission
3. install dependencies
    ```bash
    apt update && \
    apt upgrade -y && \
    apt-get install libncurses-dev libssl-dev libelf-dev flex bison pkg-config dpkg-dev libssl-dev libelf-dev dwarves -y
    ```
4. `cd /usr/src && install linux-source`
5. `cd linux-source-xxxx` (xxx is the version that you downloaded)
6. Inside the directory, you may see the following files 
    - `debian`
    - `debian.master`
    - `linux-source-xxx.gz.bz2`
7. extract the `linux-source-xxx.gz.bz2` with `tar -xvf linux*`
8. `cd linux-source-xxx`
9. copy the `.config` from /boot
    ```
    cp /boot/config-`uname -r` .config
    ```
10. `make menuconfig`
11. select Local version (`press Enter` and `enter your student ID`)
    - do not use capital letter since that violate debian package's naming convention
12. `tick` the option below select local version (`press space`)
13. save the config and exit
14. `vim .config` to remove `CONFIG_SYSTEM_TRUSTED_KEYS` and `CONFIG_SYSTEM_REVOCATION_KEYS`
    - if you are not familiar with the vim editor
    - `/CONFIG_SYSTEM_TRUSTED_KEYS` (search for the word with `/`)
    - `di"` (delete the word inside "")
    - `:wq` (quit the vim editor)
15. `make -j12 deb-pkg`
    - This may take 20 minutes to finish
    - `-j12` specify the CPU number you want to utilize (the number depends on how many CPU you assign when you created the VM)
    - `deb-pkg` build debian package
16. `cd ..` to see the packages you built
17. `dpkg -i linux-image-xxx.deb` (the one with your student ID)
18. `dpkg -i linux-header-xxx.deb` (the one with your student ID)
19. update-grub || update-grub2
    - `not necessary` but `recommended`
    - `grub` is the boot manager managing the kernels you can boot from 
    - depending on your system `update-grub` is the older version of `update-grub2`
    - `||` means that if the previous command failed do the next command
20. reboot your VM and try the commands:
    a. `uname -a` and `uname -r`
    b. `ifconfig -a` (sudo apt install net-tools)
        - The command is actually deprecated. I recommend you use `ip a` for your daily usage
    c. `ping 140.117.11.1` (Ctrl-c or Ctrl-d to terminate it)
