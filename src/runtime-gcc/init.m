#include <objc/objc.h>

/* Generate references to Object and NXConstantString classes since they are
   needed by the runtime system to run correctly. */
void __objc_linking(void) {
  [Object name];
  [NXConstantString name];
}
