# SOR2-TP0


Make clean
make

sudo rmod charDevice.ko
sudo insmod charDevice.ko

en /dev
sudo mknod /dev/chardev c 240 0
chmod 777 chardev


echo "holacomova" > chardev
cat chardev

![image](https://user-images.githubusercontent.com/20630925/226198759-55aa7c73-dd9b-41e6-b456-abb4603bc12c.png)
