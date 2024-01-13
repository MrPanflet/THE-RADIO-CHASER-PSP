#include <pspkernel.h>
#include <pspctrl.h>
#include <resources\Scripts\callbacks.h>

void exit_callback(int arg1,int arg2, void* common){
sceKernelExitGame();
return ;
}
void callbackthread(SceSize args, void* argp){
int cbid = sceKernelCreateCallback("Exit callback", exit_callback, NULL);
sceKernelRegisterExitCallback(cbid);

sceKernelSleepThreadCB();
return ;
}
void setupCallbacks(){
    int thid = sceKernelCreateThread("update_thread",callbackthread,0x11,0xFA0,0,NULL);
if (thid >= 0)
{
    sceKernelStartThread(thid,0,NULL);
}
}
