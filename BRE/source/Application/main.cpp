#include <memory>             
   
#include <general/Application.h>                                                                                          
             
#include "scenes/Scene.h"                                
            
#if defined(DEBUG) || defined(_DEBUG)                                                                                                                                            
#define _CRTDBG_MAP_ALLOC          
#include <cstdlib>            
#include <crtdbg.h>             
#endif    
  
int 
WINAPI WinMain(HINSTANCE instance, HINSTANCE /*previousInstance*/, LPSTR /*commandLine*/, int showCommand) {       
	// Memory leak checking in Visual Studio's Output panel.   
#if defined(DEBUG) | defined(_DEBUG)  
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);          
#endif

	BRE::Application app(instance, showCommand);  
	app.Add(new Scene());
	app.Run();
	return 0;
}