## Recompile

*For ubuntu 22.04*

1. Install VM
    - I used ubuntu server iso (I don't need GUI)
    - Allocate enough disk size (at least 30GB aside from default os)
2. `sudo -s` 
    - Use root permission
3. Install dependencies
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
7. Extract the `linux-source-xxx.gz.bz2` with `tar -xvf linux*`
8. `cd linux-source-xxx`
9. Copy the `.config` from /boot
    ```
    cp /boot/config-`uname -r` .config
    ```
10. `make menuconfig`
    - Feel free to directly edit the info you want in the `.config` if you are familiar with it.
11. Select Local version (`press Enter` and `enter your student ID`)
    - **DO NOT** use capital letters. That violates debian package's naming convention
12. **Tick** *Automatically append the local version* (`press space`)
13. Save the config and exit
14. `vim .config` to remove `CONFIG_SYSTEM_TRUSTED_KEYS` and `CONFIG_SYSTEM_REVOCATION_KEYS`
    *if you are not familiar with the vim editor*
    - `/CONFIG_SYSTEM_TRUSTED_KEYS` (search words with `/`)
    - `di"` (delete the word inside "")
    - `:wq` (quit the vim editor)
15. `make -j12 deb-pkg`
    - This may take 20 minutes to finish
    - `-j12` specify the number of CPUs you want to utilize (the number depends on how many CPU you assign when you create a VM)
    - `deb-pkg` build debian package
16. `cd ..` to see the packages you built
17. `dpkg -i linux-image-xxx.deb` (the one with your student ID)
18. `dpkg -i linux-header-xxx.deb` (the one with your student ID)
19. update-grub || update-grub2
    - `not necessary` but `recommended`
    - `grub` is the kernel manager managing the kernels you can boot from 
    - `update-grub` is the older version of `update-grub2`
    - `||`: if the previous command failed do the next command
20. Reboot your VM and try the following commands:
    1. `uname -a` and `uname -r`
    2. `ifconfig -a` (sudo apt install net-tools)
        - The command is actually deprecated. I recommend you use `ip a` for your daily usage
    3. `ping 140.117.11.1` (Ctrl-c or Ctrl-d to terminate it)
