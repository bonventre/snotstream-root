#include <TApplication.h>
#include "dict.h"

/*
 * testing out threaded root gui application
 */

int main(int argc, char *argv[]) { 

  TApplication theApp("App", &argc, argv);
  // Popup the GUI... 
  RootApp *temp = new RootApp(gClient->GetRoot(),200,200); 
  theApp.Run();

  delete theApp;

  return 0;
}
