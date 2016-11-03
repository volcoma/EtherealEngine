// tracey is callstack based. no dirty new/delete macro tricks.
// tracey is a static library. requires no source modification. just link it.

#include "tracey.hpp"

int main() {
    {
        // monitor & report only this scope (will find one leak)
        // 
        tracey::scope sc;
        char *ptrs[10];
        for( int i = 0; i < 10; ++i )
        	ptrs[ i ] = new char [128];
        for( int i = 10; --i > 0; )
        	delete [] ptrs[ i ];
    }

    tracey::disable(); // do not show final report on exit

#if kTraceyWebserverPort
    fprintf(stdout, "webserver at http://localhost:%d\n", kTraceyWebserverPort );
    for(;;) fgetc(stdin);
#endif
}
