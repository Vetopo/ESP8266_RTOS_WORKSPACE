# ESP8266_RTOS_WORKSPACE

* Pluto IOT Stack For ESP8266 

![image](https://github.com/vetopo/ESP8266_RTOS_WORKSPACE/blob/master/doc/img/ESP8266_EV_BOARD.png)  

* if you want to buy:

  https://item.taobao.com/item.htm?spm=2013.1.w4023-21495180077.16.11cd2804EUGeHC&id=590380418301  

* about us:

  http://www.vetopo.com

## Downdload: 

* git clone --recursive https://github.com/vetopo/ESP8266_RTOS_WORKSPACE.git 

## Build: 

* cd ESP8266_RTOS_WORKSPACE 

* source set_path.sh 

* cd app 	

* cd PL-Switch3-A01  

* make menuconfig 

* configure and save exit  

* make -j8 flash  

## Debug:

* make monitor  

* exit: ctrl+]

## Program running crash on chip:  

### (make sure the esp-sdk version is release V3.3 and try again)  

* enter director: ESP8266_RTOS_WORKSPACE/  
* rm -rf ESP8266_RTOS_SDK  
* git submodule update  
* cd ESP8266_RTOS_SDK/  
* git checkout 4c38ff314a22a61f127036abdccfa28cd5b25ec7    (release 3.3)

## More Document:  

* visit:  https://docs.espressif.com/projects/esp8266-rtos-sdk/en/latest/get-started/index.html 
 
