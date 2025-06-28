#include <iostream>
#include <thread>
#include "../main/main.h"
#include "../kernel/coms.h"
#include <Windows.h>
void main()
{
    std::cout << "[-] Driver failed to initialize.\n";

    //uac::init();
  //  user::init();
    if (!kernel->Init()) {
        std::cout << "[-] Driver failed to initialize.\n";
        std::cout << "[+] Attempting to load driver for you...\n";
        if (!kernel->Init()) {
            std::cout << "[-] Still failed to initialize driver. Exiting.\n";
            return;
        }
        std::cout << "[+] Driver initialized.\n";
        std::cout << " \n";
        std::cout << "[+] App is Closing...\n";
        std::cout << " \n";
        std::cout << "[+] Reopen App After in lobby.\n";
        Sleep(4000);
    }
    kernel->Attach(L"FortniteClient-Win64-Shipping.exe");
    kernel->Base();
    if (!kernel->ProcessBase) {
        std::cout << "[-] Failed to get base address. Exiting.\n";
    }
    std::cout << "[+] Driver is loaded. Base address: 0x" << std::hex << kernel->ProcessBase << std::endl;
    std::cout << "[+] Waiting for Fortnite window...\n";
    while (MyWnd == 0) {
        Sleep(100);
        kernel->Attach(L"FortniteClient-Win64-Shipping.exe");
        MyWnd = GetProcWindow(kernel->ProcessId);
    }
    std::cout << "[+] Fortnite window found.\n";
    if (!kernel->Cr3()) {
        std::cout << "[-] loading CR3 Memory Class\n";
    }
    std::cout << "[+] CR3 is valid. Launching overlay.\n";
    uac::init();
    user::init();
    overlay();  
    std::thread(caching).detach(); // fic perforkanc ehere

    std::thread(watchdog_thread).detach(); // fic perforkanc ehere

    std::thread(world_reading_work).detach();
    directx_init();
    ImGui_ImplWin32_Init(MyWnd);  
    ImGui_ImplDX9_Init(p_device);
    RenderLoop();
    
}


