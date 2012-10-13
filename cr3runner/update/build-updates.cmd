@echo off

DEL *.bin
DEL *.azw2
DEL *.keystore

COPY /Y .\..\build\libs\cr3runner-2.7.azw2 cr3runner-2.7.azw2
COPY /Y .\..\conf\keystore\developer.keystore developer.keystore

echo ================================================================
echo Install updates:
echo ================================================================

kindletool create ota -d dxg install.sh cr3runner-2.7.azw2 developer.keystore external.policy update_cr3runner_2.7_dxg_install.bin
kindletool create ota -d k3g install.sh cr3runner-2.7.azw2 developer.keystore external.policy update_cr3runner_2.7_k3g_install.bin
kindletool create ota -d k3gb install.sh cr3runner-2.7.azw2 developer.keystore external.policy update_cr3runner_2.7_k3gb_install.bin
kindletool create ota -d k3w install.sh cr3runner-2.7.azw2 developer.keystore external.policy update_cr3runner_2.7_k3w_install.bin
kindletool create ota2 -d k4 -d k4b install.sh cr3runner-2.7.azw2 developer.keystore external.policy update_cr3runner_2.7_k4x_install.bin


echo ================================================================
echo Uninstall updates:
echo ================================================================

kindletool create ota -d dxg uninstall.sh update_cr3runner_2.7_dxg_uninstall.bin
kindletool create ota -d k3g uninstall.sh update_cr3runner_2.7_k3g_uninstall.bin
kindletool create ota -d k3gb uninstall.sh update_cr3runner_2.7_k3gb_uninstall.bin
kindletool create ota -d k3w uninstall.sh update_cr3runner_2.7_k3w_uninstall.bin
kindletool create ota2 -d k4 -d k4b uninstall.sh update_cr3runner_2.7_k4x_uninstall.bin
